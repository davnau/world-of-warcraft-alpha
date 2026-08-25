#include "ENGINE/Source/Services/sysmessage.h"

#include <array>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>

namespace {
struct SeverityDisplay { unsigned char red, green, blue; float timeVisible; };
constexpr std::array<SeverityDisplay, SYSMSG_NUMTYPES> s_severityDisplay{{
    {255, 255, 255, 5.0F}, {255, 255, 0, 7.0F}, {255, 64, 64, 10.0F}, {255, 0, 0, 15.0F}}};
constexpr std::array<const char*, SYSMSG_NUMTYPES> s_severityStrings{{"INFO", "WARNING", "ERROR", "FATAL"}};
constexpr std::array<char, 7> s_categoryMaskLetters{{'S', 'G', 'N', 'R', 'A', 'O', 'M'}};
std::recursive_mutex s_mutex;
bool s_enabled = true;
SYSMSG_TYPE s_minSeverity = SYSMSG_INFO;
SYSMSG_TYPE s_maxSeverity = SYSMSG_FATAL;
unsigned s_categoryFilter = ~0U;
SYSMSG_CALLBACK s_callback = nullptr;
std::ofstream s_file;

bool IsSeverity(SYSMSG_TYPE value) { return value >= SYSMSG_INFO && value < SYSMSG_NUMTYPES; }
}

MSGBUFFER::~MSGBUFFER() { delete[] string; }

void MSGBUFFER::SetInfo(const char* newString, SYSMSG_TYPE newSeverity, unsigned categories) {
    std::unique_ptr<char[]> replacement;
    if (newString) {
        replacement = std::make_unique<char[]>(std::strlen(newString) + 1);
        strcpy_s(replacement.get(), std::strlen(newString) + 1, newString);
    }
    delete[] string;
    string = replacement.release();
    severity = newSeverity;
    categoryMask = categories;
}

void GenerateMaskString(char* buffer, unsigned size, unsigned mask) {
    if (!buffer || !size) return;
    char letters[8]{};
    for (unsigned i = 0; i < s_categoryMaskLetters.size(); ++i)
        letters[i] = (mask & (1U << i)) ? s_categoryMaskLetters[i] : '-';
    std::snprintf(buffer, size, "%s", letters);
}

int SysMsgAdd(const char* message, SYSMSG_TYPE severity, unsigned categoryMask) {
    if (!message) throw std::invalid_argument("SysMsgAdd message");
    if (!IsSeverity(severity)) throw std::out_of_range("SysMsgAdd severity");
    std::lock_guard lock(s_mutex);
    if (!s_enabled || !*message || severity < s_minSeverity || severity > s_maxSeverity || !(categoryMask & s_categoryFilter))
        return 1;
    char mask[8]{};
    GenerateMaskString(mask, sizeof(mask), categoryMask);
    if (s_file.is_open()) {
        s_file << mask << '|' << s_severityStrings[severity] << '|' << message << "\r\n";
        s_file.flush();
    }
    if (s_callback) s_callback(message, severity);
    return 1;
}

int SysMsgAdd(CStatus* status, unsigned categoryMask) {
    // CStatus is owned by Engine/Base. Keep the recovered overload linkable while
    // that class is reconstructed; null represents the original empty-status path.
    return status ? SysMsgAdd("Status reported an error", SYSMSG_ERROR, categoryMask) : 1;
}

int SysMsgVPrintf(SYSMSG_TYPE severity, unsigned categoryMask, const char* format, std::va_list args) {
    if (!format) throw std::invalid_argument("SysMsgVPrintf format");
    char buffer[256]{};
    std::vsnprintf(buffer, sizeof(buffer), format, args);
    buffer[sizeof(buffer) - 1] = '\0';
    return SysMsgAdd(buffer, severity, categoryMask);
}

int SysMsgPrintf(SYSMSG_TYPE severity, unsigned categoryMask, const char* format, ...) {
    std::va_list args;
    va_start(args, format);
    const int result = SysMsgVPrintf(severity, categoryMask, format, args);
    va_end(args);
    return result;
}

void SysMsgEnable(int enabled) { std::lock_guard lock(s_mutex); s_enabled = enabled != 0; }
int SysMsgEnabled() { std::lock_guard lock(s_mutex); return s_enabled; }
void SysMsgSetMinDisplayLevel(SYSMSG_TYPE severity) { if (!IsSeverity(severity)) throw std::out_of_range("minimum severity"); std::lock_guard lock(s_mutex); s_minSeverity = severity; if (s_maxSeverity < severity) s_maxSeverity = severity; }
void SysMsgSetMaxDisplayLevel(SYSMSG_TYPE severity) { if (!IsSeverity(severity)) throw std::out_of_range("maximum severity"); std::lock_guard lock(s_mutex); s_maxSeverity = severity; if (s_minSeverity > severity) s_minSeverity = severity; }
SYSMSG_TYPE SysMsgGetMinDisplayLevel() { std::lock_guard lock(s_mutex); return s_minSeverity; }
SYSMSG_TYPE SysMsgGetMaxDisplayLevel() { std::lock_guard lock(s_mutex); return s_maxSeverity; }
void SysMsgSetFilter(unsigned categoryMask) { std::lock_guard lock(s_mutex); s_categoryFilter = categoryMask; }
unsigned SysMsgGetFilter() { std::lock_guard lock(s_mutex); return s_categoryFilter; }

void SysMsgGetSeverityColor(SYSMSG_TYPE severity, float* red, float* green, float* blue) {
    if (!IsSeverity(severity)) throw std::out_of_range("severity color");
    const auto& display = s_severityDisplay[severity];
    if (red) *red = display.red / 255.0F;
    if (green) *green = display.green / 255.0F;
    if (blue) *blue = display.blue / 255.0F;
}

double SysMsgGetSeverityDuration(int severity) { return severity >= 0 && severity < SYSMSG_NUMTYPES ? s_severityDisplay[severity].timeVisible : 0.0; }

void SysMsgInitialize() { std::lock_guard lock(s_mutex); s_enabled = true; s_minSeverity = SYSMSG_INFO; s_maxSeverity = SYSMSG_FATAL; s_categoryFilter = ~0U; }
void SysMsgShutdown() { std::lock_guard lock(s_mutex); s_enabled = false; SysMsgDisableFileLog(); }

int DetermineFileName(const char* directory, char* buffer, unsigned size) {
    if (!directory || !buffer || !size) return 0;
    const std::filesystem::path base(directory);
    for (unsigned index = 0; index < 1000; ++index) {
        char name[32]{};
        std::snprintf(name, sizeof(name), "SysMsgLog%03u.txt", index);
        const auto candidate = base / name;
        if (!std::filesystem::exists(candidate)) {
            const std::string text = candidate.string();
            if (text.size() + 1 > size) return 0;
            std::memcpy(buffer, text.c_str(), text.size() + 1);
            return 1;
        }
    }
    return 0;
}

void SysMsgEnableFileLog(const char* directory) {
    std::lock_guard lock(s_mutex);
    const std::filesystem::path base = directory && *directory ? directory : ".";
    std::error_code error;
    std::filesystem::create_directories(base, error);
    if (error) { SysMsgPrintf(SYSMSG_ERROR, 1U, "Error cannot create directory %s", base.string().c_str()); return; }
    char path[260]{};
    SysMsgDisableFileLog();
    if (DetermineFileName(base.string().c_str(), path, sizeof(path))) s_file.open(path, std::ios::binary | std::ios::out);
}

void SysMsgDisableFileLog() { std::lock_guard lock(s_mutex); if (s_file.is_open()) s_file.close(); }
void SysMsgSetCallback(SYSMSG_CALLBACK callback) { std::lock_guard lock(s_mutex); s_callback = callback; }

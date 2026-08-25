#include "Storm/h/SLog.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <memory>
#include <mutex>
#include <new>
#include <string>
#include <vector>

namespace {

struct LogRecord {
    std::recursive_mutex lock;
    std::filesystem::path filename;
    FILE* file = nullptr;
    unsigned int flags = 0;
    int indent = 0;
    bool timestamp = true;
};

std::recursive_mutex s_logLock;
std::vector<LogRecord*> s_logs;
std::filesystem::path s_defaultDirectory;
bool s_initialized = false;

LogRecord* Log(HSLOG__* handle) { return reinterpret_cast<LogRecord*>(handle); }
HSLOG__* Public(LogRecord* log) { return reinterpret_cast<HSLOG__*>(log); }

std::filesystem::path ResolveLogPath(const char* filename) {
    std::filesystem::path path(filename ? filename : "");
    if (path.is_absolute()) return path;
    if (!s_defaultDirectory.empty()) return s_defaultDirectory / path;
    return std::filesystem::current_path() / path;
}

bool Open(LogRecord* log) {
    if (log->file || (log->flags & 2U)) return true;
    std::error_code error;
    std::filesystem::create_directories(log->filename.parent_path(), error);
    const wchar_t* mode = log->flags & 4U ? L"ab" : L"wb";
    return _wfopen_s(&log->file, log->filename.c_str(), mode) == 0 && log->file;
}

void Prefix(LogRecord* log) {
    if (log->timestamp) {
        SYSTEMTIME now{};
        GetLocalTime(&now);
        std::fprintf(log->file, "%u/%u %02u:%02u:%02u.%03u  ", now.wMonth, now.wDay,
            now.wHour, now.wMinute, now.wSecond, now.wMilliseconds);
    }
    const int spaces = std::clamp(log->indent, 0, 128);
    for (int index = 0; index < spaces; ++index) std::fputc(' ', log->file);
}

void WriteV(LogRecord* log, const char* format, va_list arguments) {
    if (!log || !format) return;
    std::lock_guard guard(log->lock);
    if (!Open(log) || !log->file) return;
    Prefix(log);
    std::vfprintf(log->file, format, arguments);
    std::fputs("\r\n", log->file);
}

} // namespace

void __stdcall SLogInitialize() {
    std::lock_guard lock(s_logLock);
    s_initialized = true;
}

HSLOG__* __stdcall SLogIsInitialized() {
    return s_initialized ? reinterpret_cast<HSLOG__*>(1) : nullptr;
}

int __stdcall SLogCreate(const char* filename, unsigned int flags, HSLOG__** output) {
    if (!filename || !*filename || !output) return 0;
    *output = nullptr;
    SLogInitialize();
    auto log = std::make_unique<LogRecord>();
    log->filename = ResolveLogPath(filename);
    log->flags = flags;
    if (!(flags & 1U) && !Open(log.get())) return 0;
    LogRecord* result = log.release();
    {
        std::lock_guard lock(s_logLock);
        s_logs.push_back(result);
    }
    *output = Public(result);
    return 1;
}

void __stdcall SLogClose(HSLOG__* handle) {
    LogRecord* log = Log(handle);
    if (!log) return;
    std::lock_guard lock(s_logLock);
    s_logs.erase(std::remove(s_logs.begin(), s_logs.end(), log), s_logs.end());
    {
        std::lock_guard guard(log->lock);
        if (log->file) { std::fflush(log->file); std::fclose(log->file); log->file = nullptr; }
    }
    delete log;
}

void __stdcall SLogDestroy() {
    std::vector<LogRecord*> logs;
    {
        std::lock_guard lock(s_logLock);
        logs.swap(s_logs);
        s_initialized = false;
    }
    for (LogRecord* log : logs) {
        if (log->file) { std::fflush(log->file); std::fclose(log->file); }
        delete log;
    }
}

void __stdcall SLogDump(HSLOG__* handle, const void* data, unsigned int bytes) {
    LogRecord* log = Log(handle);
    if (!log || (!data && bytes)) return;
    const auto* input = static_cast<const unsigned char*>(data);
    std::lock_guard guard(log->lock);
    if (!Open(log) || !log->file) return;
    for (unsigned int offset = 0; offset < bytes; offset += 8) {
        Prefix(log);
        std::fprintf(log->file, "%04x: ", offset);
        for (unsigned int index = 0; index < 8; ++index) {
            if (offset + index < bytes) std::fprintf(log->file, "%02x ", input[offset + index]);
            else std::fputs("   ", log->file);
            if (index == 3) std::fputc(' ', log->file);
        }
        std::fputs(" ", log->file);
        for (unsigned int index = 0; index < 8 && offset + index < bytes; ++index) {
            const unsigned char value = input[offset + index];
            std::fputc(value >= 32 && value != 127 ? value : '.', log->file);
            if (index == 3) std::fputc(' ', log->file);
        }
        std::fputs("\r\n", log->file);
    }
}

void __stdcall SLogFlush(HSLOG__* handle) {
    LogRecord* log = Log(handle);
    if (!log) return;
    std::lock_guard guard(log->lock);
    if (log->file) std::fflush(log->file);
}
void __stdcall SLogFlushAll() {
    std::lock_guard lock(s_logLock);
    for (LogRecord* log : s_logs) SLogFlush(Public(log));
}
void __stdcall SLogGetDefaultDirectory(char* directoryName, unsigned int directoryNameSize) {
    if (!directoryName || !directoryNameSize) return;
    std::lock_guard lock(s_logLock);
    strncpy_s(directoryName, directoryNameSize, s_defaultDirectory.string().c_str(), _TRUNCATE);
}
void __stdcall SLogSetDefaultDirectory(const char* directoryName) {
    std::lock_guard lock(s_logLock);
    s_defaultDirectory = directoryName ? std::filesystem::path(directoryName) : std::filesystem::path();
}
int __stdcall SLogSetAbsIndent(HSLOG__* handle, int indent) {
    LogRecord* log = Log(handle);
    if (!log) return 0;
    std::lock_guard guard(log->lock);
    const int previous = log->indent;
    log->indent = indent;
    return previous;
}
int __stdcall SLogSetIndent(HSLOG__* handle, int deltaIndent) {
    LogRecord* log = Log(handle);
    if (!log) return 0;
    std::lock_guard guard(log->lock);
    const int previous = log->indent;
    log->indent += deltaIndent;
    return previous;
}
void __stdcall SLogSetTimestamp(HSLOG__* handle, int timestamp) {
    LogRecord* log = Log(handle);
    if (log) { std::lock_guard guard(log->lock); log->timestamp = timestamp != 0; }
}
void __stdcall SLogVWrite(HSLOG__* handle, const char* format, va_list arguments) {
    WriteV(Log(handle), format, arguments);
}
void SLogWrite(HSLOG__* handle, const char* format, ...) {
    va_list arguments;
    va_start(arguments, format);
    SLogVWrite(handle, format, arguments);
    va_end(arguments);
}
void SLogPend(HSLOG__* handle, const char* format, ...) {
    va_list arguments;
    va_start(arguments, format);
    SLogVWrite(handle, format, arguments);
    va_end(arguments);
}

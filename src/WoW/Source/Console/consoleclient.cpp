#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "WoW/Source/Console/consoleclient.h"

#include "WoW/Source/Console/consoledetect.h"
#include "WoW/Source/Console/consolevar.h"
#include "WoW/Source/clientruntime.h"

#include "ENGINE/Source/Gx/gxapi/gxapi.h"
#include "ENGINE/Source/Os/W32/input.h"

void OsGuiSetGxWindow(HWND window);
void OsGuiSetWindowTitle(HWND window, const char* title);

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <limits>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace {
std::mutex g_consoleMutex;
std::vector<std::unique_ptr<CONSOLELINE>> g_lines;
std::array<NTempest::CImVector, 9> g_colors{};
std::array<int, 7> g_rates{30, 40, 50, 60, 70, 75, 85};
std::array<NTempest::C2iVector, 10> g_legalSizes{};
CGxFormat g_requestedFormat;
CGxFormat g_lastGoodFormat;
CGxFormat g_desktopFormat;
CGxFormat g_fallbackFormat;
std::string g_fontName = "Fonts\\ArialN.TTF";
std::string g_consoleTitle;
std::string g_repeatBuffer;
float g_fontHeight = 0.016F;
float g_charSpacing = 0.0F;
float g_consoleLines = 10.0F;
float g_consoleHeight = 0.16F;
float g_caretTime = 0.0F;
float g_highlightLeft = 0.0F;
float g_highlightRight = 0.0F;
int g_repeatCount = 0;
int g_active = 0;
CGxDevice* g_device = nullptr;
int g_screen = 0;
int g_proportional = 0;
int g_caret = 1;
int g_highlightState = HS_NONE;
int g_maxLines = 256;
int g_historyIndex = -1;
int g_completionMode = 0;
RECTF g_rect{0.0F, 0.0F, 1.0F, 1.0F};
RECTF g_highlightRect{0.0F, 0.0F, 0.0F, 0.0F};

CONSOLELINE* MakeLine(bool input) {
    auto line = std::make_unique<CONSOLELINE>();
    line->owned = new std::string(input ? "> " : "");
    line->buffer = line->owned->data();
    line->chars = line->owned->size();
    line->capacity = line->owned->capacity();
    line->input = input;
    line->inputstart = input ? 2 : 0;
    line->inputpos = line->chars;
    CONSOLELINE* result = line.get();
    g_lines.push_back(std::move(line));
    return result;
}

void SyncLine(CONSOLELINE* line) {
    if (!line || !line->owned) return;
    line->buffer = line->owned->data();
    line->chars = line->owned->size();
    line->capacity = line->owned->capacity();
    if (line->inputpos > line->chars) line->inputpos = line->chars;
}

void SetLineText(CONSOLELINE* line, const char* text) {
    if (!line) return;
    if (!line->owned) line->owned = new std::string();
    *line->owned = text ? text : "";
    SyncLine(line);
}

CONSOLELINE* InputLineNoLock() {
    if (!g_lines.empty() && g_lines.back()->input)
        return g_lines.back().get();
    return MakeLine(true);
}

void SetDefaultColors() {
    g_colors[0] = NTempest::CImVector(0xFFFFFFFFu);
    g_colors[1] = NTempest::CImVector(0xFFFFFFFFu);
    g_colors[2] = NTempest::CImVector(0xFF808080u);
    g_colors[3] = NTempest::CImVector(0xFFFF0000u);
    g_colors[4] = NTempest::CImVector(0xFFFFFF00u);
    g_colors[5] = NTempest::CImVector(0xFFFFFFFFu);
    g_colors[6] = NTempest::CImVector(0xFFFFFFFFu);
    g_colors[7] = NTempest::CImVector(0x80FFFFFFu);
    g_colors[8] = NTempest::CImVector(0xC0000000u);
}

bool ParseColor(const char* value, unsigned& alpha, unsigned& red,
                unsigned& green, unsigned& blue) {
    if (!value) return false;
    return std::sscanf(value, "%u %u %u %u", &alpha, &red, &green, &blue) == 4 &&
           alpha <= 255 && red <= 255 && green <= 255 && blue <= 255;
}

bool ParseColorType(const char* name, int& type) {
    if (!name) return false;
    static constexpr const char* names[] = {
        "default", "input", "echo", "error", "warning", "admin", "global"
    };
    for (int i = 0; i < 7; ++i) {
        if (SStrCmpI(name, names[i], 0x7FFFFFFFu) == 0) {
            type = i;
            return true;
        }
    }
    return false;
}

void SetCVarValue(const char* name, const char* value) {
    if (auto* variable = CVar::Lookup(name)) variable->Set(value ? value : "");
}

} // namespace

float INFINITY_294 = 0.0F;
float TWO_PI_289 = 0.0F;
float OO_TWO_PI_290 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_99 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_99 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_99 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_99 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_99 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_99 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_99 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_99 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_99 = 0.0F;
float SECONDS_PER_HEARTBEAT_99 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_98 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_99 = 0.0F;

int ConsoleCommand_ClearConsole() {
    std::lock_guard lock(g_consoleMutex);
    g_lines.clear();
    g_historyIndex = -1;
    return 1;
}

int __fastcall ConsoleCommand_Proportional(const char*) {
    g_proportional ^= 1;
    RegenerateFontStrings();
    return 1;
}

void RegenerateFontStrings() {
    std::lock_guard lock(g_consoleMutex);
    for (const auto& line : g_lines) GenerateNodeString(line.get());
}

void GenerateNodeString(CONSOLELINE* line) {
    SyncLine(line);
}

int __fastcall ConsoleCommand_CharSpacing(const char*, const char* value) {
    g_charSpacing = value && *value ? std::strtof(value, nullptr) : 0.0F;
    RegenerateFontStrings();
    return 1;
}

int ConsoleCommand_DefaultSettings(const char*) {
    SetDefaultColors();
    g_fontHeight = 0.016F;
    g_fontName = "Fonts\\ArialN.TTF";
    g_consoleLines = 10.0F;
    g_consoleHeight = g_fontHeight * g_consoleLines;
    RegenerateFontStrings();
    return 1;
}

int ConsoleCommand_CloseConsole() {
    g_active = 0;
    return 0;
}

int __fastcall ConsoleCommand_RepeatHandler(const char*, const char* value) {
    if (!value || !*value) {
        g_repeatCount = 0;
        g_repeatBuffer.clear();
        return 1;
    }
    char* end = nullptr;
    const auto count = std::strtoul(value, &end, 10);
    while (end && *end && std::isspace(static_cast<unsigned char>(*end))) ++end;
    if (count != 0 && end && *end) {
        g_repeatCount = static_cast<int>(std::min<unsigned long>(count, 0x7FFFFFFFu));
        g_repeatBuffer = end;
    }
    return 1;
}

int ConsoleCommand_CurrentSettings() {
    ConsolePrintf("Font Height is %f", g_fontHeight);
    ConsolePrintf("Font Name is %s", g_fontName.c_str());
    ConsolePrintf("Number of Console Lines %f", g_consoleLines);
    return 1;
}

int __fastcall ConsoleCommand_FontColor(const char*, const char* value) {
    char typeName[32]{};
    unsigned red = 0, green = 0, blue = 0;
    if (!value || std::sscanf(value, "%31s %u %u %u", typeName, &red, &green, &blue) != 4) return 0;
    int type = 0;
    if (!ParseColorType(typeName, type) || red > 255 || green > 255 || blue > 255) return 0;
    SetColor(type, NTempest::CImVector(0xFF000000u | (red << 16) | (green << 8) | blue));
    return 1;
}

void __fastcall SetColor(int colorType, NTempest::CImVector color) {
    if (colorType >= 0 && colorType < static_cast<int>(g_colors.size())) g_colors[colorType] = color;
}

int __fastcall ConsoleCommand_BufferSize(const char*, const char* value) {
    if (!value || !*value) return 1;
    const float lines = std::strtof(value, nullptr);
    if (!(lines > 0.0F)) return 0;
    g_consoleLines = lines;
    g_consoleHeight = g_fontHeight * lines;
    return 1;
}

int __fastcall ConsoleCommand_FontSize(const char*, const char* value) {
    float size = value ? std::strtof(value, nullptr) * 0.001F : g_fontHeight;
    size = std::clamp(size, 0.01F, 0.05F);
    const float lines = g_fontHeight > 0.0F ? g_consoleHeight / g_fontHeight : 10.0F;
    g_fontHeight = size;
    g_consoleLines = lines;
    g_consoleHeight = g_fontHeight * g_consoleLines;
    RegenerateFontStrings();
    return 1;
}

int __fastcall ConsoleCommand_Font(const char*, const char* value) {
    if (!value || !*value) return 0;
    g_fontName = "Fonts\\";
    g_fontName += value;
    if (g_fontName.size() < 4 || g_fontName.substr(g_fontName.size() - 4) != ".ttf") g_fontName += ".ttf";
    RegenerateFontStrings();
    return 1;
}

int __fastcall ConsoleCommand_BackGroundColor(const char*, const char* value) {
    unsigned alpha = 0, red = 0, green = 0, blue = 0;
    if (!ParseColor(value, alpha, red, green, blue)) return 0;
    SetColor(8, NTempest::CImVector((alpha << 24) | (red << 16) | (green << 8) | blue));
    return 1;
}

int __fastcall ConsoleCommand_HighLightColor(const char*, const char* value) {
    unsigned alpha = 0, red = 0, green = 0, blue = 0;
    if (!ParseColor(value, alpha, red, green, blue)) return 0;
    SetColor(7, NTempest::CImVector((alpha << 24) | (red << 16) | (green << 8) | blue));
    return 1;
}

void ConsoleDeviceInitialize(int) {
    SetDefaultColors();
    GxLogOpen();
    RegisterGxCVars();
    RegisterGxCCmds();

    SetDefaultsFormat(&g_fallbackFormat);
    SetDefaultsFormat(&g_desktopFormat);
    SetDefaultsFormat(&g_requestedFormat);
    SetDefaultsFormat(&g_lastGoodFormat);

    std::uint8_t hardwareChanged = 0;
    DetectHardware(&hardwareChanged, 0);
    if (g_hardware.format != nullptr) {
        g_requestedFormat = *g_hardware.format;
    }

    std::vector<CGxMonitorMode> monitorModes;
    GxAdapterMonitorModes(monitorModes);

    unsigned desktopMode[4]{};
    if (GxAdapterDesktopMode(desktopMode) != 0) {
        g_desktopFormat.size = NTempest::C2iVector{
            static_cast<int>(desktopMode[0]), static_cast<int>(desktopMode[1])};
        g_desktopFormat.colorFormat = desktopMode[2] <= 16
            ? CGxFormat::Fmt_Rgb565
            : CGxFormat::Fmt_ArgbX888;
        g_desktopFormat.depthFormat = desktopMode[2] <= 16
            ? CGxFormat::Fmt_Ds160
            : CGxFormat::Fmt_Ds24X;
        g_desktopFormat.refreshRate = desktopMode[3] != 0 ? desktopMode[3] : 60;
    }

    // Preserve the recovered preference order (configured API, then the
    // alternate API).  The D3D backend is accepted only when it owns the GX
    // window that the original ConsoleDeviceInitialize required.
    const CVar* apiVariable = CVar::Lookup("gxApi");
    const EGxApi preferredApi = apiVariable != nullptr
        && SStrCmpI(apiVariable->StringValue(), "opengl", 0x7FFFFFFFu) == 0
        ? GxApi_OpenGl
        : GxApi_Direct3d;

    for (unsigned attempt = 0; attempt < 2 && g_device == nullptr; ++attempt) {
        const EGxApi api = static_cast<EGxApi>(
            (static_cast<unsigned>(preferredApi) + attempt) & 1U);
        CGxFormat candidate = g_requestedFormat;
        OptimizeFormat(&candidate);
        ValidateFormatMonitor(&candidate);

        CGxDevice* device = GxDevCreate(
            api, reinterpret_cast<void*>(OsWindowProc), candidate);
        if (device != nullptr && GxDevWindow() != nullptr) {
            g_device = device;
            g_requestedFormat = candidate;
            g_lastGoodFormat = candidate;
            break;
        }
        if (device != nullptr) {
            GxDevDestroy(device);
        }
    }

    if (g_device == nullptr) {
        GxLog("ConsoleDeviceInitialize(): no output device");
        return;
    }

    SetGxCVars(g_lastGoodFormat);
    HWND window = GxDevWindow();
    OsGuiSetGxWindow(window);
    OsGuiSetWindowTitle(window, "World of Warcraft");
}

void SetGameCVars() {}

void SetCVar(CVar* variable, float value) {
    if (!variable) return;
    char text[32]{};
    std::snprintf(text, sizeof(text), "%f", value);
    variable->Set(text);
}

void SetCVar(CVar* variable, int value) { SetCVar_0(variable, value); }

void SetCVar(CVar* variable, char value) { SetCVar_1(variable, value); }

void SetCVar_0(CVar* variable, int value) {
    if (!variable) return;
    char text[32]{};
    std::snprintf(text, sizeof(text), "%d", value);
    variable->Set(text);
}

void SetCVar_1(CVar* variable, char value) { SetCVar_0(variable, value ? 1 : 0); }

void SetGxCVars(const CGxFormat& format) {
    SetCVarValue("gxColorBits", format.colorFormat == CGxFormat::Fmt_Rgb565 ? "16" : "24");
    SetCVarValue("gxDepthBits", format.depthFormat == CGxFormat::Fmt_Ds160 ? "16" : "24");
    char text[64]{};
    std::snprintf(text, sizeof(text), "%dx%d", format.size.x, format.size.y);
    SetCVarValue("gxResolution", text);
    std::snprintf(text, sizeof(text), "%u", format.refreshRate);
    SetCVarValue("gxRefresh", text);
    SetCVarValue("gxVSync", format.vsync ? "1" : "0");
    SetCVarValue("gxWindow", format.window ? "1" : "0");
}

unsigned char UpdateGxCVars() {
    const char* names[] = {"gxColorBits", "gxDepthBits", "gxResolution", "gxRefresh", "gxApi", "gxVSync", "gxWindow"};
    unsigned char changed = 0;
    for (const char* name : names) if (auto* cvar = CVar::Lookup(name)) changed |= cvar->Update();
    return changed;
}

void RegisterGxCVars() {
    CVar::Register("gxColorBits", "gxColorBits", 0, "24", nullptr, 3, 0, nullptr);
    CVar::Register("gxDepthBits", "gxDepthBits", 0, "24", nullptr, 3, 0, nullptr);
    CVar::Register("gxResolution", "gxResolution", 0, "1280x720", nullptr, 3, 0, nullptr);
    CVar::Register("gxRefresh", "gxRefresh", 0, "60", nullptr, 3, 0, nullptr);
    CVar::Register("gxApi", "gxApi", 0, "d3d11", nullptr, 3, 0, nullptr);
    CVar::Register("gxVSync", "gxVSync", 0, "0", nullptr, 3, 0, nullptr);
    CVar::Register("gxWindow", "gxWindow", 0, "1", nullptr, 3, 0, nullptr);
}

unsigned char CVGxResolutionCallback(const char* value, void*) {
    if (!value) return 0;
    int width = 0, height = 0;
    char separator = 0;
    if (std::sscanf(value, "%d%c%d", &width, &separator, &height) != 3 || separator != 'x') return 0;
    g_legalSizes = {NTempest::C2iVector{640, 480}, NTempest::C2iVector{800, 600}, NTempest::C2iVector{1024, 768},
                    NTempest::C2iVector{1152, 864}, NTempest::C2iVector{1280, 720}, NTempest::C2iVector{1280, 960},
                    NTempest::C2iVector{1280, 1024}, NTempest::C2iVector{1600, 900}, NTempest::C2iVector{1600, 1200},
                    NTempest::C2iVector{1920, 1080}};
    for (const auto& size : g_legalSizes)
        if (size.x == width && size.y == height) {
            g_requestedFormat.size = size;
            return 1;
        }
    return 0;
}

unsigned char CVGxColorBitsCallback(const char* value, void*) {
    const int bits = SStrToInt(value);
    if (bits == 16) g_requestedFormat.colorFormat = CGxFormat::Fmt_Rgb565;
    else if (bits == 24) g_requestedFormat.colorFormat = CGxFormat::Fmt_ArgbX888;
    else if (bits == 30) g_requestedFormat.colorFormat = CGxFormat::Fmt_Argb2101010;
    else return 0;
    return 1;
}

unsigned char CVGxDepthBitsCallback(const char* value, void*) {
    const int bits = SStrToInt(value);
    if (bits == 16) g_requestedFormat.depthFormat = CGxFormat::Fmt_Ds160;
    else if (bits == 24) g_requestedFormat.depthFormat = CGxFormat::Fmt_Ds24X;
    else if (bits == 32) g_requestedFormat.depthFormat = CGxFormat::Fmt_Ds320;
    else return 0;
    return 1;
}

unsigned char CVGxRefreshCallback(const char* value, void*) {
    const int rate = SStrToInt(value);
    if (std::find(g_rates.begin(), g_rates.end(), rate) == g_rates.end()) return 0;
    g_requestedFormat.refreshRate = static_cast<unsigned>(rate);
    return 1;
}

unsigned char CVGxApiCallback(const char* value, void*) {
    return value && (SStrCmpI(value, "direct3d", 16) == 0 || SStrCmpI(value, "opengl", 16) == 0);
}

unsigned char CVGxVSyncCallback(const char* value, void*) {
    g_requestedFormat.vsync = SStrToInt(value) != 0;
    return 1;
}

unsigned char CVGxWindowCallback(const char* value, void*) {
    g_requestedFormat.window = SStrToInt(value) != 0;
    return 1;
}

int RegisterGxCCmds() {
    return ConsoleCommandRegister(nullptr, "gxrestart", CLIENT_COMMAND_CATEGORY_GRAPHICS, nullptr) != nullptr;
}

int CCGxRestart(CGxFormat* format) {
    if (format) g_requestedFormat = *format;
    ValidateFormatMonitor(&g_requestedFormat);
    g_lastGoodFormat = g_requestedFormat;
    SetGxCVars(g_lastGoodFormat);
    return 1;
}

unsigned char ReduceFormat(CGxFormat* format) {
    if (!format || format->refreshRate == 0) return 0;
    if (format->refreshRate > 1) --format->refreshRate;
    if (format->depthFormat > CGxFormat::Fmt_Rgb565) format->depthFormat = static_cast<CGxFormat::Format>(static_cast<int>(format->depthFormat) - 1);
    return 1;
}

void OptimizeFormat(CGxFormat* format) {
    if (!format) return;
    if (format->size.x < 800 && format->size.y < 600 && g_desktopFormat.size.x >= 800 && g_desktopFormat.size.y >= 600) {
        format->size = NTempest::C2iVector{800, 600};
    }
    if (format->depthFormat < CGxFormat::Fmt_Ds24X && g_desktopFormat.depthFormat >= CGxFormat::Fmt_Ds24X)
        format->depthFormat = CGxFormat::Fmt_Ds24X;
    if (format->colorFormat < CGxFormat::Fmt_ArgbX888 && g_desktopFormat.colorFormat >= CGxFormat::Fmt_ArgbX888)
        format->colorFormat = CGxFormat::Fmt_ArgbX888;
}

void ValidateFormatMonitor(CGxFormat* format) {
    if (!format) return;
    if (format->size.x <= 0 || format->size.y <= 0) format->size = g_fallbackFormat.size;
    if (format->refreshRate == 0) format->refreshRate = 60;
    if (format->colorFormat >= CGxFormat::Formats_Last) format->colorFormat = CGxFormat::Fmt_ArgbX888;
    if (format->depthFormat >= CGxFormat::Formats_Last) format->depthFormat = CGxFormat::Fmt_Ds24X;
}

void ConsoleDeviceDestroy() {
    ConsoleScreenDestroy();
    OsGuiSetGxWindow(nullptr);
    if (g_device != nullptr) {
        GxDevDestroy(g_device);
        g_device = nullptr;
    }
    GxLogClose();
}

void ConsoleScreenInitialize() {
    std::lock_guard lock(g_consoleMutex);
    g_screen = 1;
    g_active = 1;
    if (g_lines.empty()) MakeLine(true);
}

void PaintBackground(const RECTF* rect, const RECTF*, float) {
    if (rect) g_rect = *rect;
    if (g_highlightState != HS_NONE) DrawHighLight();
}

void DrawBackground() {}
void DrawHighLight() {}

void PaintText(const RECTF* rect, float elapsedSec) {
    if (rect) g_rect = *rect;
    g_caretTime += elapsedSec;
    const float period = g_caret ? 0.3F : 0.2F;
    if (g_caretTime >= period) {
        g_caretTime = 0.0F;
        g_caret ^= 1;
    }
}

void DrawCaret(const float*) {}

CONSOLELINE* GetInputLine() {
    std::lock_guard lock(g_consoleMutex);
    return InputLineNoLock();
}

void EnforceMaxLines() {
    while (static_cast<int>(g_lines.size()) > g_maxLines + 1) {
        if (!g_lines.empty() && g_lines.front()->input) break;
        g_lines.erase(g_lines.begin());
    }
}

void RegisterHandlers(void*) {}

int OnIdle(const void*) { return g_active == 0; }

int OnChar(const void* data) {
    if (!g_active || !data) return 1;
    const auto ch = *static_cast<const char*>(data);
    if (ch < 32) return 0;
    auto* line = GetInputLine();
    ReserveInputSpace(line, 1);
    std::lock_guard lock(g_consoleMutex);
    line->owned->insert(line->inputpos, 1, ch);
    ++line->inputpos;
    SyncLine(line);
    return 0;
}

void PasteInInputLine(const char* text) {
    if (!text) return;
    auto* line = GetInputLine();
    std::lock_guard lock(g_consoleMutex);
    line->owned->insert(line->inputpos, text);
    line->inputpos += std::strlen(text);
    SyncLine(line);
}

void ReserveInputSpace(CONSOLELINE* line, std::size_t extra) {
    if (!line || !line->owned) return;
    line->owned->reserve(line->owned->size() + extra + 1);
    SyncLine(line);
}

void ResetHighlight() {
    g_highlightState = HS_NONE;
    g_highlightLeft = g_highlightRight = 0.0F;
    g_highlightRect = {};
}

int OnMouseDown(const void*) {
    g_highlightState = HS_ACTIVE;
    return 0;
}

CONSOLELINE* GetLineAtMousePosition(float y) {
    if (g_lines.empty()) return nullptr;
    const auto index = static_cast<std::size_t>(std::clamp(y / std::max(g_fontHeight, 0.001F), 0.0F,
                                                              static_cast<float>(g_lines.size() - 1)));
    return g_lines[index].get();
}

void UpdateHighlight(float left, float right) {
    g_highlightLeft = std::min(left, right);
    g_highlightRight = std::max(left, right);
    g_highlightState = g_highlightLeft == g_highlightRight ? HS_NONE : HS_ACTIVE;
}

int OnMouseUp(const void*) { return 0; }
int OnMouseMove(const void*) { return 0; }

int OnKeyDown(const void* data) {
    if (!g_active || !data) return 1;
    const int key = *static_cast<const int*>(data);
    auto* line = GetInputLine();
    std::lock_guard lock(g_consoleMutex);
    if (key == 8 && line->inputpos > line->inputstart) {
        line->owned->erase(line->inputpos - 1, 1);
        --line->inputpos;
    } else if (key == 127 && line->inputpos < line->chars) {
        line->owned->erase(line->inputpos, 1);
    } else if (key == 13) {
        const std::string command = line->owned->substr(line->inputstart);
        if (!command.empty()) ConsoleCommandExecute(command.c_str(), command.c_str());
        SetLineText(line, "> ");
        line->inputstart = line->inputpos = 2;
    }
    SyncLine(line);
    ResetHighlight();
    return 0;
}

void MakeCommandCurrent(CONSOLELINE* line, const char* text) {
    if (!line) return;
    const std::string prefix = line->input ? "> " : "";
    SetLineText(line, prefix.c_str());
    if (line->input) line->inputstart = prefix.size();
    ReserveInputSpace(line, text ? std::strlen(text) : 0);
    line->owned->append(text ? text : "");
    line->inputpos = line->owned->size();
    SyncLine(line);
}

void MoveLinePtr(int modifier, int direction) {
    auto* current = GetInputLine();
    if (!current || g_lines.empty()) return;
    auto it = std::find_if(g_lines.begin(), g_lines.end(), [current](const auto& item) { return item.get() == current; });
    if (it == g_lines.end()) return;
    const int delta = (direction ? 1 : -1) * std::max(1, modifier);
    const auto index = static_cast<std::ptrdiff_t>(it - g_lines.begin());
    const auto target = std::clamp(index + delta, std::ptrdiff_t(0), static_cast<std::ptrdiff_t>(g_lines.size() - 1));
    if (g_lines[target].get() != current) MakeCommandCurrent(current, g_lines[target]->owned->c_str());
}

int OnKeyDownRepeat(const void* data, const int* keyData) {
    return OnKeyDown(keyData ? static_cast<const void*>(keyData) : data);
}

int OnKeyUp() { return g_active == 0; }

int EventCloseCallback() {
    ConsolePostClose();
    return 0;
}

void ConsoleScreenDestroy() {
    std::lock_guard lock(g_consoleMutex);
    g_lines.clear();
    g_screen = 0;
    g_active = 0;
}

void UnregisterHandlers(void*) {}
int ConsoleIsActive() { return g_active; }
void ConsoleSetTitle(const char* title) { g_consoleTitle = title ? title : ""; }

void ConsoleWrite(const char* text, COLOR_T color) {
    if (!text || !*text || !g_device) return;
    std::lock_guard lock(g_consoleMutex);
    auto line = std::make_unique<CONSOLELINE>();
    line->owned = new std::string(text);
    line->buffer = line->owned->data();
    line->chars = line->owned->size();
    line->capacity = line->owned->capacity();
    line->colorType = static_cast<int>(color);
    line->input = false;
    if (!g_lines.empty() && g_lines.back()->input) {
        auto input = std::move(g_lines.back());
        g_lines.pop_back();
        g_lines.push_back(std::move(line));
        g_lines.push_back(std::move(input));
    } else {
        g_lines.push_back(std::move(line));
    }
    ++g_maxLines;
    EnforceMaxLines();
}

void ConsoleWriteA(const char* format, COLOR_T color, ...) {
    if (!format) return;
    char buffer[1024]{};
    va_list args;
    va_start(args, color);
    std::vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    ConsoleWrite(buffer, color);
}

void ConsoleWriteV(const char* format, va_list args) {
    if (!format) return;
    char buffer[1024]{};
    std::vsnprintf(buffer, sizeof(buffer), format, args);
    ConsoleWrite(buffer, 0);
}

void ConsolePostClose() { g_active = 0; }

int _E1_294() { INFINITY_294 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E2_294() { return _E1_294(); }
void _E4_291() { TWO_PI_289 = 6.28318530717958647692F; }
int _E5_291() { _E4_291(); return 0; }
void _E7_291() { OO_TWO_PI_290 = 1.0F / TWO_PI_289; }
int _E8_291() { _E7_291(); return 0; }
void _E19_49() { MAX_DUEL_DISTANCE_SQUARED_99 = 100.0F; }
int _E20_66() { _E19_49(); return 0; }
void _E22_57() { MAX_TRADE_DISTANCE_SQUARED_99 = 11.111111F * 11.111111F; }
int _E23_72() { _E22_57(); return 0; }
void _E25_58() { MAX_SHOP_DISTANCE_SQUARED_99 = 5.5555553F * 5.5555553F; }
int _E26_67() { _E25_58(); return 0; }
void _E28_97() { MAX_INSPECT_DISTANCE_SQUARED_99 = 5.5555553F * 5.5555553F; }
int _E29_68() { _E28_97(); return 0; }
void _E31_94() { MAX_LOOT_DISTANCE_SQUARED_99 = 25.0F; }
int _E32_60() { _E31_94(); return 0; }
void _E34_71() { MAX_BIND_DISTANCE_SQUARED_99 = 100.0F; }
int _E35_41() { _E34_71(); return 0; }
void _E37_66() { MAX_SITCHAIR_DISTANCE_SQUARED_99 = 0.25F; }
int _E38_40() { _E37_66(); return 0; }
void _E40_67() { MAX_SITCHAIRUSE_DISTANCE_SQUARED_99 = 9.0F; }
int _E41_36() { _E40_67(); return 0; }
void _E43_65() { MAX_SHARE_DISTANCE_SQUARED_99 = 10000.0F; }
int _E44_38() { _E43_65(); return 0; }
void _E46_63() { SECONDS_PER_HEARTBEAT_99 = 5.0F; }
int _E47_39() { _E46_63(); return 0; }
void _E49_63() { SPELL_FOCUS_DISTANCE_SQUARED_98 = 2500.0F; }
int _E50_32() { _E49_63(); return 0; }
void _E52_64() { MAX_OBJ_INTEREST_RADIUS_SQUARED_99 = 10000.0F; }
int _E53_33() { _E52_64(); return 0; }
void* _E80_16() { return nullptr; }
int _E82_7() { return 0; }
int _E83_6() { _E80_16(); return _E82_7(); }
void _E81_4() {}
unsigned _E85_3() { return 0; }
void _E86_0() { g_lines.clear(); }
int _E87_7() { return 0; }
int _E88_6() { _E85_3(); return _E87_7(); }
int _E90_6() { SetDefaultColors(); return -1; }
void _E91() {}
int _E92_1() { return 0; }
int _E93_3() { _E90_6(); return _E92_1(); }
void _E95_2() { g_consoleHeight = g_consoleLines * g_fontHeight; }
int _E96_1() { _E95_2(); return 0; }
CGxFormat* _E98_2() { return &g_fallbackFormat; }
void _E99_26() {}
int _E100_27() { return 0; }
int _E101_23() { _E98_2(); return _E100_27(); }
CGxFormat* _E103_6() { return &g_desktopFormat; }
void _E104_27() {}
int _E105_27() { return 0; }
int _E106_4() { _E103_6(); return _E105_27(); }
CGxFormat* _E108_23() { return &g_requestedFormat; }
void _E109_2() {}
int _E110_25() { return 0; }
int _E111_23() { _E108_23(); return _E110_25(); }
CGxFormat* _E113_25() { return &g_lastGoodFormat; }
void _E114_25() {}
int _E115_2() { return 0; }
int _E116_24() { _E113_25(); return _E115_2(); }
int _E118_1() { return 0; }
void _E119_25() {}
int _E120_24() { return 0; }
int _E121_0() { _E118_1(); return _E120_24(); }
void _E124_1() {}

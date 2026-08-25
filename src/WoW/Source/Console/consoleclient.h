#pragma once

#include "ENGINE/Source/Base/coordinate.h"
#include "ENGINE/Source/Gx/gxapi/gxformat.h"
#include "ENGINE/Source/Gx/gxapi/gxmonitor.h"
#include "ENGINE/Source/Tempest/cimvector.h"

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <string>

class CVar;

using COLOR_T = std::uint32_t;

enum ConsoleHighlightState : int {
    HS_NONE = 0,
    HS_ACTIVE = 1,
};

struct CONSOLELINE {
    std::string* owned = nullptr;
    char* buffer = nullptr;
    std::size_t chars = 0;
    std::size_t capacity = 0;
    std::size_t inputstart = 0;
    std::size_t inputpos = 0;
    int colorType = 0;
    bool input = false;
};

int ConsoleCommand_ClearConsole();
int __fastcall ConsoleCommand_Proportional(const char* args);
void RegenerateFontStrings();
void GenerateNodeString(CONSOLELINE* line);
int __fastcall ConsoleCommand_CharSpacing(const char* args, const char* value);
int ConsoleCommand_DefaultSettings(const char* arguments = nullptr);
int ConsoleCommand_CloseConsole();
int __fastcall ConsoleCommand_RepeatHandler(const char* arguments, const char* value);
int ConsoleCommand_CurrentSettings();
int __fastcall ConsoleCommand_FontColor(const char* arguments, const char* value);
void __fastcall SetColor(int colorType, NTempest::CImVector color);
int __fastcall ConsoleCommand_BufferSize(const char* command, const char* value);
int __fastcall ConsoleCommand_FontSize(const char* arguments, const char* value);
int __fastcall ConsoleCommand_Font(const char* arguments, const char* value);
int __fastcall ConsoleCommand_BackGroundColor(const char* arguments, const char* value);
int __fastcall ConsoleCommand_HighLightColor(const char* arguments, const char* value);

void ConsoleDeviceInitialize(int multithreaded = 0);
void SetGameCVars();
void SetCVar(CVar* variable, float value);
void SetCVar(CVar* variable, int value);
void SetCVar(CVar* variable, char value);
void SetCVar_0(CVar* variable, int value);
void SetCVar_1(CVar* variable, char value);
void SetGxCVars(const CGxFormat& format);
unsigned char UpdateGxCVars();
void RegisterGxCVars();
unsigned char CVGxResolutionCallback(const char* value, void* arg);
unsigned char CVGxColorBitsCallback(const char* value, void* arg);
unsigned char CVGxDepthBitsCallback(const char* value, void* arg);
unsigned char CVGxRefreshCallback(const char* value, void* arg);
unsigned char CVGxApiCallback(const char* value, void* arg);
unsigned char CVGxVSyncCallback(const char* value, void* arg);
unsigned char CVGxWindowCallback(const char* value, void* arg);
int RegisterGxCCmds();
int CCGxRestart(CGxFormat* format = nullptr);
unsigned char ReduceFormat(CGxFormat* format);
void OptimizeFormat(CGxFormat* format);
void ValidateFormatMonitor(CGxFormat* format);
void ConsoleDeviceDestroy();

void ConsoleScreenInitialize();
void PaintBackground(const RECTF* rect, const RECTF* clip, float elapsedSec);
void DrawBackground();
void DrawHighLight();
void PaintText(const RECTF* rect, float elapsedSec);
void DrawCaret(const float* position);
CONSOLELINE* GetInputLine();
void EnforceMaxLines();
void RegisterHandlers(void* eventContext = nullptr);
int OnIdle(const void* data = nullptr);
int OnChar(const void* data = nullptr);
void PasteInInputLine(const char* text);
void ReserveInputSpace(CONSOLELINE* line, std::size_t extra);
void ResetHighlight();
int OnMouseDown(const void* data = nullptr);
CONSOLELINE* GetLineAtMousePosition(float y);
void UpdateHighlight(float left, float right);
int OnMouseUp(const void* data = nullptr);
int OnMouseMove(const void* data = nullptr);
int OnKeyDown(const void* data = nullptr);
void MakeCommandCurrent(CONSOLELINE* line, const char* text);
void MoveLinePtr(int modifier, int direction);
int OnKeyDownRepeat(const void* data = nullptr, const int* keyData = nullptr);
int OnKeyUp();
int EventCloseCallback();
void ConsoleScreenDestroy();
void UnregisterHandlers(void* eventContext = nullptr);
int ConsoleIsActive();
void ConsoleSetTitle(const char* title);
void ConsoleWrite(const char* text, COLOR_T color);
void ConsoleWriteA(const char* format, COLOR_T color, ...);
void ConsoleWriteV(const char* format, va_list args);
void ConsolePostClose();

extern float INFINITY_294;
extern float TWO_PI_289;
extern float OO_TWO_PI_290;
extern float MAX_DUEL_DISTANCE_SQUARED_99;
extern float MAX_TRADE_DISTANCE_SQUARED_99;
extern float MAX_SHOP_DISTANCE_SQUARED_99;
extern float MAX_INSPECT_DISTANCE_SQUARED_99;
extern float MAX_LOOT_DISTANCE_SQUARED_99;
extern float MAX_BIND_DISTANCE_SQUARED_99;
extern float MAX_SITCHAIR_DISTANCE_SQUARED_99;
extern float MAX_SITCHAIRUSE_DISTANCE_SQUARED_99;
extern float MAX_SHARE_DISTANCE_SQUARED_99;
extern float SECONDS_PER_HEARTBEAT_99;
extern float SPELL_FOCUS_DISTANCE_SQUARED_98;
extern float MAX_OBJ_INTEREST_RADIUS_SQUARED_99;

int _E2_294(); int _E1_294(); int _E5_291(); void _E4_291(); int _E8_291(); void _E7_291();
int _E20_66(); void _E19_49(); int _E23_72(); void _E22_57(); int _E26_67(); void _E25_58();
int _E29_68(); void _E28_97(); int _E32_60(); void _E31_94(); int _E35_41(); void _E34_71();
int _E38_40(); void _E37_66(); int _E41_36(); void _E40_67(); int _E44_38(); void _E43_65();
int _E47_39(); void _E46_63(); int _E50_32(); void _E49_63(); int _E53_33(); void _E52_64();
int _E83_6(); void* _E80_16(); int _E82_7(); void _E81_4(); int _E88_6(); unsigned _E85_3(); int _E87_7(); void _E86_0();
int _E93_3(); int _E90_6(); int _E92_1(); void _E91(); int _E96_1(); void _E95_2();
int _E101_23(); CGxFormat* _E98_2(); int _E100_27(); void _E99_26(); int _E106_4(); CGxFormat* _E103_6();
int _E105_27(); void _E104_27(); int _E111_23(); CGxFormat* _E108_23(); int _E110_25(); void _E109_2();
int _E116_24(); CGxFormat* _E113_25(); int _E115_2(); void _E114_25(); int _E121_0(); int _E118_1(); int _E120_24(); void _E119_25();
void _E124_1();

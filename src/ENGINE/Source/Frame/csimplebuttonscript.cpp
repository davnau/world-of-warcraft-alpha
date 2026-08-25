#include "ENGINE/Source/Frame/frame.h"

#include <cstring>

// Typed script entry points.  The preserved client binds these to its Lua state at runtime.
int CSimpleButton_Enable(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_Enable"); }
int CSimpleButton_Disable(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_Disable"); }
int CSimpleButton_IsEnabled(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_IsEnabled"); }
int CSimpleButton_GetButtonState(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_GetButtonState"); }
const char* ButtonStateToString(CSimpleButtonState state)
{
    switch (state) {
    case BUTTONSTATE_DISABLED: return "DISABLED";
    case BUTTONSTATE_NORMAL: return "NORMAL";
    case BUTTONSTATE_PUSHED: return "PUSHED";
    default: return "UNKNOWN";
    }
}
int CSimpleButton_SetButtonState(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_SetButtonState"); }
bool StringToButtonState(const char* text, CSimpleButtonState* state)
{
    if (!text || !state) return false;
    if (_stricmp(text, "DISABLED") == 0) *state = BUTTONSTATE_DISABLED;
    else if (_stricmp(text, "NORMAL") == 0) *state = BUTTONSTATE_NORMAL;
    else if (_stricmp(text, "PUSHED") == 0) *state = BUTTONSTATE_PUSHED;
    else return false;
    return true;
}
int CSimpleButton_SetText(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_SetText"); }
int CSimpleButton_SetTextColor(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_SetTextColor"); }
int CSimpleButton_SetDisabledTextColor(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_SetDisabledTextColor"); }
int CSimpleButton_SetHighlightTextColor(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_SetHighlightTextColor"); }
int CSimpleButton_SetNormalTexture(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_SetNormalTexture"); }
int CSimpleButton_SetPushedTexture(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_SetPushedTexture"); }
int CSimpleButton_SetDisabledTexture(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_SetDisabledTexture"); }
int CSimpleButton_SetHighlightTexture(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_SetHighlightTexture"); }
int CSimpleButton_GetText(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_GetText"); }
int CSimpleButton_GetTextWidth(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_GetTextWidth"); }
int CSimpleButton_GetTextHeight(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_GetTextHeight"); }
int CSimpleButton_RegisterForClicks(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_RegisterForClicks"); }
int CSimpleButton_Click(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_Click"); }
int CSimpleButton_LockHighlight(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_LockHighlight"); }
int CSimpleButton_UnlockHighlight(lua_State* state) { return FrameScript_Invoke(state, "CSimpleButton_UnlockHighlight"); }
void CSimpleButton::RegisterScriptMethods()
{
    FrameScript_Object::FillScriptMethodTable(s_scriptMethods, {
        {"Enable", CSimpleButton_Enable}, {"Disable", CSimpleButton_Disable},
        {"IsEnabled", CSimpleButton_IsEnabled}, {"GetButtonState", CSimpleButton_GetButtonState},
        {"SetButtonState", CSimpleButton_SetButtonState}, {"SetText", CSimpleButton_SetText},
        {"SetTextColor", CSimpleButton_SetTextColor},
        {"SetDisabledTextColor", CSimpleButton_SetDisabledTextColor},
        {"SetHighlightTextColor", CSimpleButton_SetHighlightTextColor},
        {"SetNormalTexture", CSimpleButton_SetNormalTexture},
        {"SetPushedTexture", CSimpleButton_SetPushedTexture},
        {"SetDisabledTexture", CSimpleButton_SetDisabledTexture},
        {"SetHighlightTexture", CSimpleButton_SetHighlightTexture},
        {"GetText", CSimpleButton_GetText}, {"GetTextWidth", CSimpleButton_GetTextWidth},
        {"GetTextHeight", CSimpleButton_GetTextHeight},
        {"RegisterForClicks", CSimpleButton_RegisterForClicks},
        {"Click", CSimpleButton_Click}, {"LockHighlight", CSimpleButton_LockHighlight},
        {"UnlockHighlight", CSimpleButton_UnlockHighlight}
    });
}
void CSimpleButton::UnregisterScriptMethods() { s_scriptMethods.clear(); }
int CSimpleButton::LookupScriptMethod(lua_State* state, const char* name) {
    return FrameScript_Object::LookupScriptMethod(state, name, s_scriptMethods);
}

#include "ENGINE/Source/Frame/frame.h"

// Typed script entry points.  The preserved client binds these to its Lua state at runtime.
int CSimpleEditBox_Insert(lua_State* state) { return FrameScript_Invoke(state, "CSimpleEditBox_Insert"); }
int CSimpleEditBox_SetText(lua_State* state) { return FrameScript_Invoke(state, "CSimpleEditBox_SetText"); }
int CSimpleEditBox_GetText(lua_State* state) { return FrameScript_Invoke(state, "CSimpleEditBox_GetText"); }
int CSimpleEditBox_AddHistoryLine(lua_State* state) { return FrameScript_Invoke(state, "CSimpleEditBox_AddHistoryLine"); }
int CSimpleEditBox_SetTextInsets(lua_State* state) { return FrameScript_Invoke(state, "CSimpleEditBox_SetTextInsets"); }
int CSimpleEditBox_SetTextColor(lua_State* state) { return FrameScript_Invoke(state, "CSimpleEditBox_SetTextColor"); }
int CSimpleEditBox_SetFocus(lua_State* state) { return FrameScript_Invoke(state, "CSimpleEditBox_SetFocus"); }
int CSimpleEditBox_ClearFocus(lua_State* state) { return FrameScript_Invoke(state, "CSimpleEditBox_ClearFocus"); }
void CSimpleEditBox::RegisterScriptMethods()
{
    FrameScript_Object::FillScriptMethodTable(s_scriptMethods, {
        {"Insert", CSimpleEditBox_Insert}, {"SetText", CSimpleEditBox_SetText},
        {"GetText", CSimpleEditBox_GetText}, {"AddHistoryLine", CSimpleEditBox_AddHistoryLine},
        {"SetTextInsets", CSimpleEditBox_SetTextInsets}, {"SetTextColor", CSimpleEditBox_SetTextColor},
        {"SetFocus", CSimpleEditBox_SetFocus}, {"ClearFocus", CSimpleEditBox_ClearFocus}
    });
}
void CSimpleEditBox::UnregisterScriptMethods() { s_scriptMethods.clear(); }
int CSimpleEditBox::LookupScriptMethod(lua_State* state, const char* name) {
    return FrameScript_Object::LookupScriptMethod(state, name, s_scriptMethods);
}

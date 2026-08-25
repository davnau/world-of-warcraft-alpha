#include "ENGINE/Source/Frame/frame.h"

// Typed script entry points.  The preserved client binds these to its Lua state at runtime.
int CSimpleStatusBar_SetMinMaxValues(lua_State* state) { return FrameScript_Invoke(state, "CSimpleStatusBar_SetMinMaxValues"); }
int CSimpleStatusBar_GetMinMaxValues(lua_State* state) { return FrameScript_Invoke(state, "CSimpleStatusBar_GetMinMaxValues"); }
int CSimpleStatusBar_SetValue(lua_State* state) { return FrameScript_Invoke(state, "CSimpleStatusBar_SetValue"); }
int CSimpleStatusBar_GetValue(lua_State* state) { return FrameScript_Invoke(state, "CSimpleStatusBar_GetValue"); }
int CSimpleStatusBar_SetStatusBarColor(lua_State* state) { return FrameScript_Invoke(state, "CSimpleStatusBar_SetStatusBarColor"); }
void CSimpleStatusBar::RegisterScriptMethods()
{
    FrameScript_Object::FillScriptMethodTable(s_scriptMethods, {
        {"SetMinMaxValues", CSimpleStatusBar_SetMinMaxValues},
        {"GetMinMaxValues", CSimpleStatusBar_GetMinMaxValues},
        {"SetValue", CSimpleStatusBar_SetValue}, {"GetValue", CSimpleStatusBar_GetValue},
        {"SetStatusBarColor", CSimpleStatusBar_SetStatusBarColor}
    });
}
void CSimpleStatusBar::UnregisterScriptMethods() { s_scriptMethods.clear(); }
int CSimpleStatusBar::LookupScriptMethod(lua_State* state, const char* name) {
    return FrameScript_Object::LookupScriptMethod(state, name, s_scriptMethods);
}

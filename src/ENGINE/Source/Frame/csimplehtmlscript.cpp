#include "ENGINE/Source/Frame/frame.h"

// Typed script entry points.  The preserved client binds these to its Lua state at runtime.
int CSimpleHTML_SetText(lua_State* state) { return FrameScript_Invoke(state, "CSimpleHTML_SetText"); }
int CSimpleHTML_SetTextColor(lua_State* state) { return FrameScript_Invoke(state, "CSimpleHTML_SetTextColor"); }
void CSimpleHTML::RegisterScriptMethods()
{
    FrameScript_Object::FillScriptMethodTable(s_scriptMethods, {
        {"SetText", CSimpleHTML_SetText}, {"SetTextColor", CSimpleHTML_SetTextColor}
    });
}
void CSimpleHTML::UnregisterScriptMethods() { s_scriptMethods.clear(); }
int CSimpleHTML::LookupScriptMethod(lua_State* state, const char* name) {
    return FrameScript_Object::LookupScriptMethod(state, name, s_scriptMethods);
}

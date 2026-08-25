#include "ENGINE/Source/Frame/frame.h"

// Typed script entry points.  The preserved client binds these to its Lua state at runtime.
int CSimpleMessageFrame_AddMessage(lua_State* state) { return FrameScript_Invoke(state, "CSimpleMessageFrame_AddMessage"); }
void CSimpleMessageFrame::RegisterScriptMethods()
{
    FrameScript_Object::FillScriptMethodTable(s_scriptMethods, {
        {"AddMessage", CSimpleMessageFrame_AddMessage}
    });
}
void CSimpleMessageFrame::UnregisterScriptMethods() { s_scriptMethods.clear(); }
int CSimpleMessageFrame::LookupScriptMethod(lua_State* state, const char* name) {
    return FrameScript_Object::LookupScriptMethod(state, name, s_scriptMethods);
}

#include "ENGINE/Source/Frame/frame.h"

// Typed script entry points.  The preserved client binds these to its Lua state at runtime.
int CSimpleScrollFrame_SetHorizontalScroll(lua_State* state) { return FrameScript_Invoke(state, "CSimpleScrollFrame_SetHorizontalScroll"); }
int CSimpleScrollFrame_SetVerticalScroll(lua_State* state) { return FrameScript_Invoke(state, "CSimpleScrollFrame_SetVerticalScroll"); }
int CSimpleScrollFrame_GetHorizontalScroll(lua_State* state) { return FrameScript_Invoke(state, "CSimpleScrollFrame_GetHorizontalScroll"); }
int CSimpleScrollFrame_GetVerticalScroll(lua_State* state) { return FrameScript_Invoke(state, "CSimpleScrollFrame_GetVerticalScroll"); }
int CSimpleScrollFrame_GetHorizontalScrollRange(lua_State* state) { return FrameScript_Invoke(state, "CSimpleScrollFrame_GetHorizontalScrollRange"); }
int CSimpleScrollFrame_GetVerticalScrollRange(lua_State* state) { return FrameScript_Invoke(state, "CSimpleScrollFrame_GetVerticalScrollRange"); }
int CSimpleScrollFrame_UpdateScrollChildRect(lua_State* state) { return FrameScript_Invoke(state, "CSimpleScrollFrame_UpdateScrollChildRect"); }
void CSimpleScrollFrame::RegisterScriptMethods()
{
    FrameScript_Object::FillScriptMethodTable(s_scriptMethods, {
        {"SetHorizontalScroll", CSimpleScrollFrame_SetHorizontalScroll},
        {"SetVerticalScroll", CSimpleScrollFrame_SetVerticalScroll},
        {"GetHorizontalScroll", CSimpleScrollFrame_GetHorizontalScroll},
        {"GetVerticalScroll", CSimpleScrollFrame_GetVerticalScroll},
        {"GetHorizontalScrollRange", CSimpleScrollFrame_GetHorizontalScrollRange},
        {"GetVerticalScrollRange", CSimpleScrollFrame_GetVerticalScrollRange},
        {"UpdateScrollChildRect", CSimpleScrollFrame_UpdateScrollChildRect}
    });
}
void CSimpleScrollFrame::UnregisterScriptMethods() { s_scriptMethods.clear(); }
int CSimpleScrollFrame::LookupScriptMethod(lua_State* state, const char* name) {
    return FrameScript_Object::LookupScriptMethod(state, name, s_scriptMethods);
}

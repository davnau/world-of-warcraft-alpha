#include "ENGINE/Source/Frame/frame.h"

// Typed script entry points.  The preserved client binds these to its Lua state at runtime.
int CSimpleMessageScrollFrame_AddMessage(lua_State* state) { return FrameScript_Invoke(state, "CSimpleMessageScrollFrame_AddMessage"); }
int CSimpleMessageScrollFrame_ScrollUp(lua_State* state) { return FrameScript_Invoke(state, "CSimpleMessageScrollFrame_ScrollUp"); }
int CSimpleMessageScrollFrame_ScrollDown(lua_State* state) { return FrameScript_Invoke(state, "CSimpleMessageScrollFrame_ScrollDown"); }
int CSimpleMessageScrollFrame_PageUp(lua_State* state) { return FrameScript_Invoke(state, "CSimpleMessageScrollFrame_PageUp"); }
int CSimpleMessageScrollFrame_PageDown(lua_State* state) { return FrameScript_Invoke(state, "CSimpleMessageScrollFrame_PageDown"); }
int CSimpleMessageScrollFrame_ScrollToTop(lua_State* state) { return FrameScript_Invoke(state, "CSimpleMessageScrollFrame_ScrollToTop"); }
int CSimpleMessageScrollFrame_ScrollToBottom(lua_State* state) { return FrameScript_Invoke(state, "CSimpleMessageScrollFrame_ScrollToBottom"); }
int CSimpleMessageScrollFrame_AtBottom(lua_State* state) { return FrameScript_Invoke(state, "CSimpleMessageScrollFrame_AtBottom"); }
void CSimpleMessageScrollFrame::RegisterScriptMethods()
{
    FrameScript_Object::FillScriptMethodTable(s_scriptMethods, {
        {"AddMessage", CSimpleMessageScrollFrame_AddMessage},
        {"ScrollUp", CSimpleMessageScrollFrame_ScrollUp},
        {"ScrollDown", CSimpleMessageScrollFrame_ScrollDown},
        {"PageUp", CSimpleMessageScrollFrame_PageUp},
        {"PageDown", CSimpleMessageScrollFrame_PageDown},
        {"ScrollToTop", CSimpleMessageScrollFrame_ScrollToTop},
        {"ScrollToBottom", CSimpleMessageScrollFrame_ScrollToBottom},
        {"AtBottom", CSimpleMessageScrollFrame_AtBottom}
    });
}
void CSimpleMessageScrollFrame::UnregisterScriptMethods() { s_scriptMethods.clear(); }
int CSimpleMessageScrollFrame::LookupScriptMethod(lua_State* state, const char* name) {
    return FrameScript_Object::LookupScriptMethod(state, name, s_scriptMethods);
}

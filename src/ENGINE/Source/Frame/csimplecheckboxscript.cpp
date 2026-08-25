#include "ENGINE/Source/Frame/frame.h"

// Typed script entry points.  The preserved client binds these to its Lua state at runtime.
int CSimpleCheckbox_SetChecked(lua_State* state) { return FrameScript_Invoke(state, "CSimpleCheckbox_SetChecked"); }
int CSimpleCheckbox_GetChecked(lua_State* state) { return FrameScript_Invoke(state, "CSimpleCheckbox_GetChecked"); }
int CSimpleCheckbox_SetCheckedTexture(lua_State* state) { return FrameScript_Invoke(state, "CSimpleCheckbox_SetCheckedTexture"); }
int CSimpleCheckbox_SetDisabledCheckedTexture(lua_State* state) { return FrameScript_Invoke(state, "CSimpleCheckbox_SetDisabledCheckedTexture"); }
void CSimpleCheckbox::RegisterScriptMethods()
{
    FrameScript_Object::FillScriptMethodTable(s_scriptMethods, {
        {"SetChecked", CSimpleCheckbox_SetChecked}, {"GetChecked", CSimpleCheckbox_GetChecked},
        {"SetCheckedTexture", CSimpleCheckbox_SetCheckedTexture},
        {"SetDisabledCheckedTexture", CSimpleCheckbox_SetDisabledCheckedTexture}
    });
}
void CSimpleCheckbox::UnregisterScriptMethods() { s_scriptMethods.clear(); }
int CSimpleCheckbox::LookupScriptMethod(lua_State* state, const char* name) {
    return FrameScript_Object::LookupScriptMethod(state, name, s_scriptMethods);
}

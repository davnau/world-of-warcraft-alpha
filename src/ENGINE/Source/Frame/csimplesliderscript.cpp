#include "ENGINE/Source/Frame/frame.h"

// Typed script entry points.  The preserved client binds these to its Lua state at runtime.
int CSimpleSlider_SetMinMaxValues(lua_State* state) { return FrameScript_Invoke(state, "CSimpleSlider_SetMinMaxValues"); }
int CSimpleSlider_GetMinMaxValues(lua_State* state) { return FrameScript_Invoke(state, "CSimpleSlider_GetMinMaxValues"); }
int CSimpleSlider_SetValue(lua_State* state) { return FrameScript_Invoke(state, "CSimpleSlider_SetValue"); }
int CSimpleSlider_GetValue(lua_State* state) { return FrameScript_Invoke(state, "CSimpleSlider_GetValue"); }
int CSimpleSlider_SetValueStep(lua_State* state) { return FrameScript_Invoke(state, "CSimpleSlider_SetValueStep"); }
int CSimpleSlider_GetValueStep(lua_State* state) { return FrameScript_Invoke(state, "CSimpleSlider_GetValueStep"); }
void CSimpleSlider::RegisterScriptMethods()
{
    FrameScript_Object::FillScriptMethodTable(s_scriptMethods, {
        {"SetMinMaxValues", CSimpleSlider_SetMinMaxValues},
        {"GetMinMaxValues", CSimpleSlider_GetMinMaxValues},
        {"SetValue", CSimpleSlider_SetValue}, {"GetValue", CSimpleSlider_GetValue},
        {"SetValueStep", CSimpleSlider_SetValueStep}, {"GetValueStep", CSimpleSlider_GetValueStep}
    });
}
void CSimpleSlider::UnregisterScriptMethods() { s_scriptMethods.clear(); }
int CSimpleSlider::LookupScriptMethod(lua_State* state, const char* name) {
    return FrameScript_Object::LookupScriptMethod(state, name, s_scriptMethods);
}

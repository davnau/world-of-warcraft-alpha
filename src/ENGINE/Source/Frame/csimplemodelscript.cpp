#include "ENGINE/Source/Frame/frame.h"

// Typed script entry points.  The preserved client binds these to its Lua state at runtime.
int CSimpleModel_SetModel(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_SetModel"); }
int CSimpleModel_ClearModel(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_ClearModel"); }
int CSimpleModel_SetPosition(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_SetPosition"); }
int CSimpleModel_SetFacing(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_SetFacing"); }
int CSimpleModel_SetScale(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_SetScale"); }
int CSimpleModel_SetSequence(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_SetSequence"); }
int CSimpleModel_SetSequenceTime(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_SetSequenceTime"); }
int CSimpleModel_SetAlpha(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_SetAlpha"); }
int CSimpleModel_SetCamera(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_SetCamera"); }
int CSimpleModel_SetLight(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_SetLight"); }
int CSimpleModel_GetPosition(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_GetPosition"); }
int CSimpleModel_GetFacing(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_GetFacing"); }
int CSimpleModel_GetScale(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_GetScale"); }
int CSimpleModel_AdvanceTime(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_AdvanceTime"); }
int CSimpleModel_ReplaceIconTexture(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_ReplaceIconTexture"); }
int CSimpleModel_SetFogColor(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_SetFogColor"); }
int CSimpleModel_SetFogNear(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_SetFogNear"); }
int CSimpleModel_SetFogFar(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_SetFogFar"); }
int CSimpleModel_ClearFog(lua_State* state) { return FrameScript_Invoke(state, "CSimpleModel_ClearFog"); }
void CSimpleModel::RegisterScriptMethods()
{
    FrameScript_Object::FillScriptMethodTable(s_scriptMethods, {
        {"SetModel", CSimpleModel_SetModel}, {"ClearModel", CSimpleModel_ClearModel},
        {"SetPosition", CSimpleModel_SetPosition}, {"SetFacing", CSimpleModel_SetFacing},
        {"SetScale", CSimpleModel_SetScale}, {"SetSequence", CSimpleModel_SetSequence},
        {"SetSequenceTime", CSimpleModel_SetSequenceTime}, {"SetAlpha", CSimpleModel_SetAlpha},
        {"SetCamera", CSimpleModel_SetCamera}, {"SetLight", CSimpleModel_SetLight},
        {"GetPosition", CSimpleModel_GetPosition}, {"GetFacing", CSimpleModel_GetFacing},
        {"GetScale", CSimpleModel_GetScale}, {"AdvanceTime", CSimpleModel_AdvanceTime},
        {"ReplaceIconTexture", CSimpleModel_ReplaceIconTexture},
        {"SetFogColor", CSimpleModel_SetFogColor}, {"SetFogNear", CSimpleModel_SetFogNear},
        {"SetFogFar", CSimpleModel_SetFogFar}, {"ClearFog", CSimpleModel_ClearFog}
    });
}
void CSimpleModel::UnregisterScriptMethods() { s_scriptMethods.clear(); }
int CSimpleModel::LookupScriptMethod(lua_State* state, const char* name) {
    return FrameScript_Object::LookupScriptMethod(state, name, s_scriptMethods);
}

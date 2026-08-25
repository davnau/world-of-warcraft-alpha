#include "ENGINE/Source/Frame/frame.h"

namespace {
FrameScriptDispatcher g_frameScriptDispatcher = nullptr;
}

void FrameScript_SetDispatcher(FrameScriptDispatcher dispatcher)
{
    g_frameScriptDispatcher = dispatcher;
}

int FrameScript_Invoke(lua_State* state, const char* functionName)
{
    return g_frameScriptDispatcher ? g_frameScriptDispatcher(state, functionName)
                                   : FrameScript_DispatchBuiltin(state, functionName);
}

// Typed script entry points.  The preserved client binds these to its Lua state at runtime.
int CSimpleFrame_GetParent(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_GetParent"); }
int CSimpleFrame_GetName(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_GetName"); }
int CSimpleFrame_GetFrameLevel(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_GetFrameLevel"); }
int CSimpleFrame_SetFrameLevel(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_SetFrameLevel"); }
int CSimpleFrame_RegisterEvent(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_RegisterEvent"); }
int CSimpleFrame_UnregisterEvent(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_UnregisterEvent"); }
int CSimpleFrame_SetAlpha(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_SetAlpha"); }
int CSimpleFrame_GetAlpha(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_GetAlpha"); }
int CSimpleFrame_SetID(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_SetID"); }
int CSimpleFrame_GetID(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_GetID"); }
int CSimpleFrame_EnableDrawLayer(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_EnableDrawLayer"); }
int CSimpleFrame_DisableDrawLayer(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_DisableDrawLayer"); }
int CSimpleFrame_Show(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_Show"); }
int CSimpleFrame_Hide(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_Hide"); }
int CSimpleFrame_IsVisible(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_IsVisible"); }
int CSimpleFrame_IsShown(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_IsShown"); }
int CSimpleFrame_Raise(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_Raise"); }
int CSimpleFrame_Lower(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_Lower"); }
int CSimpleFrame_GetCenter(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_GetCenter"); }
int CSimpleFrame_GetWidth(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_GetWidth"); }
int CSimpleFrame_SetWidth(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_SetWidth"); }
int CSimpleFrame_GetHeight(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_GetHeight"); }
int CSimpleFrame_SetHeight(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_SetHeight"); }
int CSimpleFrame_SetPoint(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_SetPoint"); }
int CSimpleFrame_SetAllPoints(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_SetAllPoints"); }
int CSimpleFrame_ClearAllPoints(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_ClearAllPoints"); }
int CSimpleFrame_RegisterForDrag(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_RegisterForDrag"); }
int CSimpleFrame_EnableMouse(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_EnableMouse"); }
int CSimpleFrame_EnableKeyboard(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_EnableKeyboard"); }
int CSimpleFrame_SetBackdropColor(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_SetBackdropColor"); }
int CSimpleFrame_SetBackdropBorderColor(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFrame_SetBackdropBorderColor"); }
void CSimpleFrame::RegisterScriptMethods()
{
    FrameScript_Object::FillScriptMethodTable(s_scriptMethods, {
        {"GetParent", CSimpleFrame_GetParent}, {"GetName", CSimpleFrame_GetName},
        {"GetFrameLevel", CSimpleFrame_GetFrameLevel}, {"SetFrameLevel", CSimpleFrame_SetFrameLevel},
        {"RegisterEvent", CSimpleFrame_RegisterEvent}, {"UnregisterEvent", CSimpleFrame_UnregisterEvent},
        {"SetAlpha", CSimpleFrame_SetAlpha}, {"GetAlpha", CSimpleFrame_GetAlpha},
        {"SetID", CSimpleFrame_SetID}, {"GetID", CSimpleFrame_GetID},
        {"EnableDrawLayer", CSimpleFrame_EnableDrawLayer}, {"DisableDrawLayer", CSimpleFrame_DisableDrawLayer},
        {"Show", CSimpleFrame_Show}, {"Hide", CSimpleFrame_Hide},
        {"IsVisible", CSimpleFrame_IsVisible}, {"IsShown", CSimpleFrame_IsShown},
        {"Raise", CSimpleFrame_Raise}, {"Lower", CSimpleFrame_Lower},
        {"GetCenter", CSimpleFrame_GetCenter}, {"GetWidth", CSimpleFrame_GetWidth},
        {"SetWidth", CSimpleFrame_SetWidth}, {"GetHeight", CSimpleFrame_GetHeight},
        {"SetHeight", CSimpleFrame_SetHeight}, {"SetPoint", CSimpleFrame_SetPoint},
        {"SetAllPoints", CSimpleFrame_SetAllPoints}, {"ClearAllPoints", CSimpleFrame_ClearAllPoints},
        {"RegisterForDrag", CSimpleFrame_RegisterForDrag}, {"EnableMouse", CSimpleFrame_EnableMouse},
        {"EnableKeyboard", CSimpleFrame_EnableKeyboard},
        {"SetBackdropColor", CSimpleFrame_SetBackdropColor},
        {"SetBackdropBorderColor", CSimpleFrame_SetBackdropBorderColor}
    });
}
void CSimpleFrame::UnregisterScriptMethods() { s_scriptMethods.clear(); }
int CSimpleFrame::LookupScriptMethod(lua_State* state, const char* name) {
    return FrameScript_Object::LookupScriptMethod(state, name, s_scriptMethods);
}

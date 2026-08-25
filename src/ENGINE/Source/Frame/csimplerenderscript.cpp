#include "ENGINE/Source/Frame/frame.h"

// Typed script entry points.  The preserved client binds these to its Lua state at runtime.
int CSimpleTexture_GetAlpha(lua_State* state) { return FrameScript_Invoke(state, "CSimpleTexture_GetAlpha"); }
int CSimpleTexture_GetName(lua_State* state) { return FrameScript_Invoke(state, "CSimpleTexture_GetName"); }
int CSimpleTexture_SetVertexColor(lua_State* state) { return FrameScript_Invoke(state, "CSimpleTexture_SetVertexColor"); }
int CSimpleTexture_SetAlpha(lua_State* state) { return FrameScript_Invoke(state, "CSimpleTexture_SetAlpha"); }
int CSimpleTexture_Show(lua_State* state) { return FrameScript_Invoke(state, "CSimpleTexture_Show"); }
int CSimpleTexture_Hide(lua_State* state) { return FrameScript_Invoke(state, "CSimpleTexture_Hide"); }
int CSimpleTexture_IsVisible(lua_State* state) { return FrameScript_Invoke(state, "CSimpleTexture_IsVisible"); }
int CSimpleTexture_SetTexture(lua_State* state) { return FrameScript_Invoke(state, "CSimpleTexture_SetTexture"); }
int CSimpleTexture_SetTexCoord(lua_State* state) { return FrameScript_Invoke(state, "CSimpleTexture_SetTexCoord"); }
int CSimpleTexture_SetPoint(lua_State* state) { return FrameScript_Invoke(state, "CSimpleTexture_SetPoint"); }
int CSimpleTexture_ClearAllPoints(lua_State* state) { return FrameScript_Invoke(state, "CSimpleTexture_ClearAllPoints"); }
int CSimpleTexture_GetWidth(lua_State* state) { return FrameScript_Invoke(state, "CSimpleTexture_GetWidth"); }
int CSimpleTexture_SetWidth(lua_State* state) { return FrameScript_Invoke(state, "CSimpleTexture_SetWidth"); }
int CSimpleTexture_GetHeight(lua_State* state) { return FrameScript_Invoke(state, "CSimpleTexture_GetHeight"); }
int CSimpleTexture_SetHeight(lua_State* state) { return FrameScript_Invoke(state, "CSimpleTexture_SetHeight"); }
int CSimpleFontString_GetName(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_GetName"); }
int CSimpleFontString_SetVertexColor(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_SetVertexColor"); }
int CSimpleFontString_SetAlpha(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_SetAlpha"); }
int CSimpleFontString_SetAlphaGradient(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_SetAlphaGradient"); }
int CSimpleFontString_Show(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_Show"); }
int CSimpleFontString_Hide(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_Hide"); }
int CSimpleFontString_IsVisible(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_IsVisible"); }
int CSimpleFontString_SetText(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_SetText"); }
int CSimpleFontString_GetText(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_GetText"); }
int CSimpleFontString_SetTextColor(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_SetTextColor"); }
int CSimpleFontString_SetTextHeight(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_SetTextHeight"); }
int CSimpleFontString_SetWidth(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_SetWidth"); }
int CSimpleFontString_GetWidth(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_GetWidth"); }
int CSimpleFontString_SetHeight(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_SetHeight"); }
int CSimpleFontString_GetHeight(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_GetHeight"); }
int CSimpleFontString_SetPoint(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_SetPoint"); }
int CSimpleFontString_ClearAllPoints(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_ClearAllPoints"); }
int CSimpleFontString_SetJustifyH(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_SetJustifyH"); }
int CSimpleFontString_SetJustifyV(lua_State* state) { return FrameScript_Invoke(state, "CSimpleFontString_SetJustifyV"); }

void CSimpleTexture::RegisterScriptMethods()
{
    FrameScript_Object::FillScriptMethodTable(s_scriptMethods, {
        {"GetAlpha", CSimpleTexture_GetAlpha}, {"GetName", CSimpleTexture_GetName},
        {"SetVertexColor", CSimpleTexture_SetVertexColor}, {"SetAlpha", CSimpleTexture_SetAlpha},
        {"Show", CSimpleTexture_Show}, {"Hide", CSimpleTexture_Hide},
        {"IsVisible", CSimpleTexture_IsVisible}, {"SetTexture", CSimpleTexture_SetTexture},
        {"SetTexCoord", CSimpleTexture_SetTexCoord}, {"SetPoint", CSimpleTexture_SetPoint},
        {"ClearAllPoints", CSimpleTexture_ClearAllPoints}, {"GetWidth", CSimpleTexture_GetWidth},
        {"SetWidth", CSimpleTexture_SetWidth}, {"GetHeight", CSimpleTexture_GetHeight},
        {"SetHeight", CSimpleTexture_SetHeight}
    });
}
void CSimpleTexture::UnregisterScriptMethods() { s_scriptMethods.clear(); }
int CSimpleTexture::LookupScriptMethod(lua_State* state, const char* name) {
    return FrameScript_Object::LookupScriptMethod(state, name, s_scriptMethods);
}

void CSimpleFontString::RegisterScriptMethods()
{
    FrameScript_Object::FillScriptMethodTable(s_scriptMethods, {
        {"GetName", CSimpleFontString_GetName}, {"SetVertexColor", CSimpleFontString_SetVertexColor},
        {"SetAlpha", CSimpleFontString_SetAlpha}, {"SetAlphaGradient", CSimpleFontString_SetAlphaGradient},
        {"Show", CSimpleFontString_Show}, {"Hide", CSimpleFontString_Hide},
        {"IsVisible", CSimpleFontString_IsVisible}, {"SetText", CSimpleFontString_SetText},
        {"GetText", CSimpleFontString_GetText}, {"SetTextColor", CSimpleFontString_SetTextColor},
        {"SetTextHeight", CSimpleFontString_SetTextHeight}, {"SetWidth", CSimpleFontString_SetWidth},
        {"GetWidth", CSimpleFontString_GetWidth}, {"SetHeight", CSimpleFontString_SetHeight},
        {"GetHeight", CSimpleFontString_GetHeight}, {"SetPoint", CSimpleFontString_SetPoint},
        {"ClearAllPoints", CSimpleFontString_ClearAllPoints},
        {"SetJustifyH", CSimpleFontString_SetJustifyH}, {"SetJustifyV", CSimpleFontString_SetJustifyV}
    });
}
void CSimpleFontString::UnregisterScriptMethods() { s_scriptMethods.clear(); }
int CSimpleFontString::LookupScriptMethod(lua_State* state, const char* name) {
    return FrameScript_Object::LookupScriptMethod(state, name, s_scriptMethods);
}

#include "WoW/Source/UI/charactermodelbase.h"

extern "C" {
#include "lauxlib.h"
}

namespace {
FrameScript_Object* ScriptObject(lua_State* state) {
    if (!state || lua_type(state, 1) != LUA_TTABLE)
        return nullptr;
    lua_rawgeti(state, 1, 0);
    auto* object = static_cast<FrameScript_Object*>(lua_touserdata(state, -1));
    lua_pop(state, 1);
    return object;
}

int Script_SetUnit(lua_State* state) {
    auto* model = dynamic_cast<CGCharacterModelBase*>(ScriptObject(state));
    if (!model)
        return luaL_error(state, "SetUnit called on an invalid PlayerModel");
    if (!lua_isstring(state, 2))
        return luaL_error(state, "Usage: SetUnit(\"unit\")");

    // Script_GetGUIDFromName resolves this in the recovered client.  Object
    // binding is still performed by the world object manager; preserve the
    // requested unit state without substituting a glue-screen paper doll.
    const char* unit = lua_tostring(state, 2);
    model->SetUnit(unit && _stricmp(unit, "player") == 0 ? 1u : 0u);
    return 0;
}

int Script_UpdateModel(lua_State* state) {
    auto* model = dynamic_cast<CGCharacterModelBase*>(ScriptObject(state));
    if (!model)
        return luaL_error(state, "UpdateModel called on an invalid PlayerModel");
    model->UpdateModel();
    return 0;
}

int Script_SetRotation(lua_State* state) {
    auto* model = dynamic_cast<CGCharacterModelBase*>(ScriptObject(state));
    if (!model)
        return luaL_error(state, "SetRotation called on an invalid PlayerModel");
    if (!lua_isnumber(state, 2))
        return luaL_error(state, "Usage: SetRotation(rotation (in radians))");
    model->m_rotationScale = static_cast<float>(lua_tonumber(state, 2));
    return 0;
}
}

CGCharacterModelBase::CGCharacterModelBase(CSimpleFrame* parent) : CSimpleModel(parent) { }
CSimpleFrame* CGCharacterModelBase::Create(CSimpleFrame* parent) {
    return new CGCharacterModelBase(parent);
}
void CGCharacterModelBase::ConfigureCamera() { }
void CGCharacterModelBase::GetUniquePaperDollModel() { }
void CGCharacterModelBase::InitializeModel() { }
int CGCharacterModelBase::LookupScriptMethod(lua_State* state, const char* name) {
    if (FrameScript_Object::LookupScriptMethod(state, name, s_scriptMethods))
        return 1;
    return CSimpleModel::LookupScriptMethod(state, name);
}
void CGCharacterModelBase::RegisterScriptMethods() {
    FrameScript_Object::FillScriptMethodTable(s_scriptMethods, {
        {"SetUnit", Script_SetUnit},
        {"UpdateModel", Script_UpdateModel},
        {"SetRotation", Script_SetRotation},
    });
}
void CGCharacterModelBase::SetUnit(std::uint64_t unitGuid) { m_unit = unitGuid; }
void CGCharacterModelBase::UnregisterScriptMethods() { s_scriptMethods.clear(); }
void CGCharacterModelBase::UpdateModel() { }
CGCharacterModelBase::~CGCharacterModelBase() { SetUnit(0); }

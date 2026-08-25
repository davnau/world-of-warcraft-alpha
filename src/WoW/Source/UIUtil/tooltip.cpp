#include "WoW/Source/UIUtil/tooltip.h"

extern "C" {
#include "lauxlib.h"
}

namespace {
FrameScript_Object* ScriptObject(lua_State* state, int index) {
    if (!state || lua_type(state, index) != LUA_TTABLE)
        return nullptr;
    lua_rawgeti(state, index, 0);
    auto* object = static_cast<FrameScript_Object*>(lua_touserdata(state, -1));
    lua_pop(state, 1);
    return object;
}

CGTooltip* Tooltip(lua_State* state) {
    return dynamic_cast<CGTooltip*>(ScriptObject(state, 1));
}

int Tooltip_NoResult(lua_State*) { return 0; }
int Tooltip_AddLine(lua_State* state) {
    if (auto* tooltip = Tooltip(state))
        ++tooltip->m_numLines;
    return 0;
}
int Tooltip_ClearLines(lua_State* state) {
    if (auto* tooltip = Tooltip(state))
        tooltip->m_numLines = 0;
    return 0;
}
int Tooltip_NumLines(lua_State* state) {
    lua_pushnumber(state, Tooltip(state) ? Tooltip(state)->m_numLines : 0.0);
    return 1;
}
int Tooltip_SetPadding(lua_State* state) {
    if (auto* tooltip = Tooltip(state))
        tooltip->m_padding = lua_isnumber(state, 2)
            ? static_cast<float>(lua_tonumber(state, 2)) : 0.0f;
    return 0;
}
int Tooltip_SetOwner(lua_State* state) {
    if (auto* tooltip = Tooltip(state))
        tooltip->m_owner = ScriptObject(state, 2);
    return 0;
}
int Tooltip_IsOwned(lua_State* state) {
    auto* tooltip = Tooltip(state);
    if (tooltip && tooltip->m_owner && tooltip->m_owner == ScriptObject(state, 2))
        lua_pushnumber(state, 1.0);
    else
        lua_pushnil(state);
    return 1;
}
int Tooltip_SetText(lua_State* state) {
    if (auto* tooltip = Tooltip(state))
        tooltip->m_numLines = lua_isnil(state, 2) ? 0u : 1u;
    return 0;
}
}

void CGTooltip::AddLine() { }
void CGTooltip::AppendText() { }
CGTooltip::CGTooltip(CSimpleFrame* parent) : CSimpleFrame(parent) { }
CSimpleFrame* CGTooltip::Create(CSimpleFrame* parent) { return new CGTooltip(parent); }
void CGTooltip::CalculateSize() { }
void CGTooltip::ClearLines() { }
void CGTooltip::FadeOut() { }
void CGTooltip::GetAuraEffectString() { }
void CGTooltip::GetItemEnchantString() { }
void CGTooltip::GetItemQualityColorString() { }
void CGTooltip::GetSpellEffectString() { }
void CGTooltip::GetSpellTargetString() { }
void CGTooltip::GetSummonedByString() { }
void CGTooltip::HideThis() { }
int CGTooltip::LookupScriptMethod(lua_State* state, const char* name) {
    if (FrameScript_Object::LookupScriptMethod(state, name, s_scriptMethods))
        return 1;
    return CSimpleFrame::LookupScriptMethod(state, name);
}
void CGTooltip::OnLayerUpdate() { }
void CGTooltip::PostLoadXML() { }
void CGTooltip::RegisterScriptMethods() {
    FrameScript_Object::FillScriptMethodTable(s_scriptMethods, {
        {"AddLine", Tooltip_AddLine}, {"AppendText", Tooltip_NoResult},
        {"ClearLines", Tooltip_ClearLines}, {"FadeOut", Tooltip_NoResult},
        {"IsOwned", Tooltip_IsOwned}, {"NumLines", Tooltip_NumLines},
        {"SetAction", Tooltip_NoResult}, {"SetBagItem", Tooltip_NoResult},
        {"SetCraftItem", Tooltip_NoResult}, {"SetCraftSpell", Tooltip_NoResult},
        {"SetHyperlink", Tooltip_NoResult}, {"SetInventoryItem", Tooltip_NoResult},
        {"SetLootItem", Tooltip_NoResult}, {"SetMerchantItem", Tooltip_NoResult},
        {"SetOwner", Tooltip_SetOwner}, {"SetPadding", Tooltip_SetPadding},
        {"SetPlayerBuff", Tooltip_NoResult}, {"SetQuestItem", Tooltip_NoResult},
        {"SetQuestLogItem", Tooltip_NoResult}, {"SetSpell", Tooltip_NoResult},
        {"SetText", Tooltip_SetText}, {"SetTradePlayerItem", Tooltip_NoResult},
        {"SetTradeSkillItem", Tooltip_NoResult}, {"SetTradeTargetItem", Tooltip_NoResult},
        {"SetTrainerService", Tooltip_NoResult}, {"SetUnit", Tooltip_NoResult},
    });
}
void CGTooltip::SetBuff() { }
void CGTooltip::SetCorpse() { }
void CGTooltip::SetItem() { }
void CGTooltip::SetObject() { }
void CGTooltip::SetOwner() { }
void CGTooltip::SetPosition() { }
void CGTooltip::SetSpell() { }
void CGTooltip::SetTooltipPadding() { }
void CGTooltip::SetUnit() { }
void CGTooltip::ShowThis() { }
void CGTooltip::UnregisterScriptMethods() { s_scriptMethods.clear(); }
CGTooltip::~CGTooltip() { }
void CGTooltip_AddLine() { }
void CGTooltip_AppendText() { }
void CGTooltip_ClearLines() { }
void CGTooltip_FadeOut() { }
void CGTooltip_IsOwned() { }
void CGTooltip_NumLines() { }
void CGTooltip_SetAction() { }
void CGTooltip_SetBagItem() { }
void CGTooltip_SetCraftItem() { }
void CGTooltip_SetCraftSpell() { }
void CGTooltip_SetHyperlink() { }
void CGTooltip_SetInventoryItem() { }
void CGTooltip_SetLootItem() { }
void CGTooltip_SetMerchantItem() { }
void CGTooltip_SetOwner() { }
void CGTooltip_SetPadding() { }
void CGTooltip_SetPlayerBuff() { }
void CGTooltip_SetQuestItem() { }
void CGTooltip_SetQuestLogItem() { }
void CGTooltip_SetSpell() { }
void CGTooltip_SetText() { }
void CGTooltip_SetTradePlayerItem() { }
void CGTooltip_SetTradeSkillItem() { }
void CGTooltip_SetTradeTargetItem() { }
void CGTooltip_SetTrainerService() { }
void CGTooltip_SetUnit() { }
void FrameScriptGetEnchantString() { }
void FrameScriptGetSpellString() { }
void GetAuraNameRec() { }
void GetEffectNameRec() { }
void HealthUpdateHandler() { }
void TooltipCorpseNameCallback() { }
void TooltipItemCreatorCallback() { }
void TooltipItemPetitionCallback() { }
void TooltipItemStatsCallback() { }
void TooltipObjectLockItemStatsCallback() { }
void TooltipSpellCreatureStatsCallback() { }
void TooltipSpellGameObjectStatsCallback() { }
void TooltipSpellItemStatsCallback() { }
void empty() { }

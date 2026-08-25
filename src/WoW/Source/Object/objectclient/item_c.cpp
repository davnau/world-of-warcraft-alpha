#include "WoW/Source/Object/objectclient/item_c.h"

CGItem_C::CGItem_C() = default;
void CGItem_C::CanBeUsed() { }
void CGItem_C::CanGoInSlot() { }
void CGItem_C::Disable() { m_disabled = true; }
std::uint32_t CGItem_C::GetClassID() const { return m_classID; }
void CGItem_C::GetDisplayID() { }
void CGItem_C::GetEnchantmentTimeLeft() { }
void CGItem_C::GetExpirationTimeLeft() { }
void CGItem_C::GetInventoryArt() { }
void CGItem_C::GetInventoryType() { }
void CGItem_C::GetItemStaticFlag() { }
void CGItem_C::GetMaterial() { }
void CGItem_C::GetMaxCount() { }
void CGItem_C::GetModelFileName() { }
void CGItem_C::GetObjectName() { }
void CGItem_C::GetPageTextID() { }
void CGItem_C::GetSelectionHighlightColor() { }
void CGItem_C::GetSheatheInvisible() { }
void CGItem_C::GetSheatheType() { }
void CGItem_C::GetStats() { }
std::uint32_t CGItem_C::GetSubtypeID() const { return m_subtypeID; }
void CGItem_C::GetUseSpell() { }
void CGItem_C::Initialize() { }
void CGItem_C::InstallItemIDMirrorHandler() { }
void CGItem_C::InstallObjMirrorHandlers() { }
void CGItem_C::IsExotic() { }
void CGItem_C::IsMetal() { }
void CGItem_C::IsWrapper() { }
void CGItem_C::OffsetOf() { }
void CGItem_C::OnRightClick() { }
void CGItem_C::PostInit() { }
void CGItem_C::PostInitWithStats() { }
void CGItem_C::Reenable() { m_disabled = false; }
void CGItem_C::SetBlock() { }
void CGItem_C::SetData() { }
void CGItem_C::SetStorage() { }
void CGItem_C::SetTranslated() { }
void CGItem_C::Shutdown() { }
void CGItem_C::UninstallItemIDMirrorHandler() { }
void CGItem_C::UpdateEnchantmentTime() { }
void CGItem_C::UpdateEnchantments() { }
void CGItem_C::UpdateExpirationTime() { }
void CGItem_C::Use() { }
CGItem_C::~CGItem_C() { }

std::uint32_t CGItem_C::GetStackCount() const { return m_stackCount; }
std::uint32_t CGItem_C::GetEntryID() const { return m_entryID; }
bool CGItem_C::IsDisabled() const { return m_disabled; }
CGBag_C* CGItem_C::GetBag() const { return m_bag; }
void CGItem_C::SetItemIdentity(std::uint32_t entryID, std::uint32_t classID,
                               std::uint32_t subtypeID, std::uint32_t stackCount) {
    m_entryID = entryID;
    m_classID = classID;
    m_subtypeID = subtypeID;
    m_stackCount = stackCount;
}
void CGItem_C::SetBag(CGBag_C* bag) { m_bag = bag; }
void CGObject_C::CanBeTargetted() { }
void CGObject_C::CanHighlight() { }
void CGObject_C::FloatingTooltip() { }
void CGObject_C::GetBag() { }
void CGObject_C::GetFacing() { }
void CGObject_C::GetGroundNormal() { }
void CGObject_C::GetMatrix() { }
void CGObject_C::GetPosition() { }
void CGObject_C::GetRenderFacing() { }
void CGObject_C::GetScale() { }
void CGObject_C::IsSolidCollidable() { }
void CGObject_C::IsSolidSelectable() { }
void CGObject_C::ObjectPostAnimate() { }
void CGObject_C::OnLeftClick() { }
void CGObject_C::OnSpecialMountAnim() { }
void CGObject_C::PostAnimate() { }
void CGObject_C::PreRender() { }
void CGObject_C::RenderTargetSelection() { }
void CGObject_C::ShouldFadeIn() { }
void CGObject_C::UpdatePlayerName() { }
void CGObject_C::UpdateRenderFacing() { }
void CGObject_C::UpdateTexComponentLoadStatus() { }
void AddInventoryArtHash() { }
void GetInventoryArtHash() { }
void ItemIDChangedCacheCallback() { }
void LoadItemCacheCallback() { }
void OnUpdateEnchantments() { }
void OnUpdateItemID() { }
void OnUpdateOwner() { }
void OnUpdateStackCount() { }

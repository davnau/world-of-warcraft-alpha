#pragma once

#include <cstdint>

class CGBag_C;

class CGItem_C {
public:
    CGItem_C();
    void CanBeUsed();
    void CanGoInSlot();
    void Disable();
    std::uint32_t GetClassID() const;
    void GetDisplayID();
    void GetEnchantmentTimeLeft();
    void GetExpirationTimeLeft();
    void GetInventoryArt();
    void GetInventoryType();
    void GetItemStaticFlag();
    void GetMaterial();
    void GetMaxCount();
    void GetModelFileName();
    void GetObjectName();
    void GetPageTextID();
    void GetSelectionHighlightColor();
    void GetSheatheInvisible();
    void GetSheatheType();
    void GetStats();
    std::uint32_t GetSubtypeID() const;
    void GetUseSpell();
    void Initialize();
    void InstallItemIDMirrorHandler();
    void InstallObjMirrorHandlers();
    void IsExotic();
    void IsMetal();
    void IsWrapper();
    void OffsetOf();
    void OnRightClick();
    void PostInit();
    void PostInitWithStats();
    void Reenable();
    void SetBlock();
    void SetData();
    void SetStorage();
    void SetTranslated();
    void Shutdown();
    void UninstallItemIDMirrorHandler();
    void UpdateEnchantmentTime();
    void UpdateEnchantments();
    void UpdateExpirationTime();
    void Use();
    std::uint32_t GetStackCount() const;
    std::uint32_t GetEntryID() const;
    bool IsDisabled() const;
    CGBag_C* GetBag() const;
    void SetItemIdentity(std::uint32_t entryID, std::uint32_t classID,
                         std::uint32_t subtypeID, std::uint32_t stackCount);
    void SetBag(CGBag_C* bag);
    ~CGItem_C();

private:
    std::uint32_t m_entryID = 0;
    std::uint32_t m_classID = 0;
    std::uint32_t m_subtypeID = 0;
    std::uint32_t m_stackCount = 0;
    bool m_disabled = false;
    CGBag_C* m_bag = nullptr;
};

class CGObject_C {
public:
    void CanBeTargetted();
    void CanHighlight();
    void FloatingTooltip();
    void GetBag();
    void GetFacing();
    void GetGroundNormal();
    void GetMatrix();
    void GetPosition();
    void GetRenderFacing();
    void GetScale();
    void IsSolidCollidable();
    void IsSolidSelectable();
    void ObjectPostAnimate();
    void OnLeftClick();
    void OnSpecialMountAnim();
    void PostAnimate();
    void PreRender();
    void RenderTargetSelection();
    void ShouldFadeIn();
    void UpdatePlayerName();
    void UpdateRenderFacing();
    void UpdateTexComponentLoadStatus();
};

// Recovered symbol declarations for objectclient/item_c.cpp

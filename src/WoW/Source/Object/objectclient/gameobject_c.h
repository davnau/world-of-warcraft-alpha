#pragma once

class CGGameObject_C {
public:
    void ActivateCustomAnim();
    CGGameObject_C();
    void CanChangeCursor();
    void CanHighlight();
    void CanUse();
    void CanUseNow();
    void CloseInteraction();
    void CreateWorldObject();
    void Disable();
    void FloatingTooltip();
    void GetCurrentMoveVector();
    void GetDebugStatus();
    void GetFacing();
    void GetLockRec();
    void GetMatrix();
    void GetModelFileName();
    void GetModelFileNameInternal();
    void GetName();
    void GetObjectName();
    void GetPageTextID();
    void GetPageTextLanguage();
    void GetPageTextMaterial();
    void GetPosition();
    void GetPropertyValue();
    void GetType();
    void GetTypeName();
    void GetWorldMatrix();
    void Initialize();
    void IsLocked();
    void IsPointInside();
    void IsSolidCollidable();
    void IsSolidSelectable();
    void IsTransport();
    void IsValidOpenAction();
    void IsValidTargetForSpell();
    void LoadBaseObject();
    void ObjectPostAnimate();
    void ObjectReaction();
    void OffsetOf();
    void OnRightClick();
    void PostInit();
    void PostPostInit();
    void PostReenable();
    void Reenable();
    void SetBlock();
    void SetData();
    void SetMirrorHandlers();
    void SetStorage();
    void Shutdown();
    void StartInteraction();
    void UnsetMirrorHandlers();
    void UpdateMatrix();
    void UpdateModelLoadStatus();
    ~CGGameObject_C();
};

class CGGameObject_C_TypeAnimated {
public:
    void ActivateCustomAnim();
    void CloseLoopingSound();
    void DebugStatus();
    void Disable();
    void HandleAnimEvent();
    void HandleAnimFinished();
    void ModelJustLoaded();
    void PlayAnimatedSound();
    void PostInit();
    void SetSequence();
    void UpdateAnimState();
    void UpdateState();
    ~CGGameObject_C_TypeAnimated();
};

class CGGameObject_C_TypeBase {
public:
    void ActivateCustomAnim();
    void AddPassenger();
    CGGameObject_C_TypeBase();
    void CanChangeCursor();
    void CanHighlight();
    void CanUse();
    void CanUseNow();
    void CloseInteraction();
    void DebugStatus();
    void Disable();
    void GetCurrentMoveVector();
    void GetFacing();
    void GetPosition();
    void HandleAnimEvent();
    void HandleAnimFinished();
    void IsPointInside();
    void ModelJustLoaded();
    void PostInit();
    void PostReenable();
    void Reenable();
    void StartInteraction();
    void UpdateMovement();
    void UpdateState();
    void Use();
};

class CGGameObject_C_Type_AreaDamage {
public:
    CGGameObject_C_Type_AreaDamage();
    void ModelJustLoaded();
};

class CGGameObject_C_Type_Binder {
public:
    CGGameObject_C_Type_Binder();
};

class CGGameObject_C_Type_Button {
public:
    CGGameObject_C_Type_Button();
};

class CGGameObject_C_Type_Camera {
public:
    CGGameObject_C_Type_Camera();
};

class CGGameObject_C_Type_Chair {
public:
    CGGameObject_C_Type_Chair();
    void CanUseNow();
    void PostInit();
};

class CGGameObject_C_Type_Chest {
public:
    CGGameObject_C_Type_Chest();
};

class CGGameObject_C_Type_Door {
public:
    CGGameObject_C_Type_Door();
    void CanUseNow();
    void IsAtRest();
    void UpdateAnimState();
};

class CGGameObject_C_Type_DuelArbiter {
public:
    CGGameObject_C_Type_DuelArbiter();
    void CanHighlight();
    void CanUse();
};

class CGGameObject_C_Type_FishingNode {
public:
    CGGameObject_C_Type_FishingNode();
    void CanUse();
};

class CGGameObject_C_Type_Generic {
public:
    CGGameObject_C_Type_Generic();
    void CanHighlight();
    void CanUse();
};

class CGGameObject_C_Type_Goober {
public:
    CGGameObject_C_Type_Goober();
};

class CGGameObject_C_Type_MapObj {
public:
    CGGameObject_C_Type_MapObj();
    void CanHighlight();
    void CanUse();
    void PostInit();
    ~CGGameObject_C_Type_MapObj();
};

class CGGameObject_C_Type_MapObjTransport {
public:
    void AddPassenger();
    CGGameObject_C_Type_MapObjTransport();
    void Disable();
    void GetFacing();
    void GetPosition();
    void IsPointInside();
    void Reenable();
    void UpdateMovement();
};

class CGGameObject_C_Type_Null {
public:
    void CanUse();
    void CanUseNow();
    void DebugStatus();
};

class CGGameObject_C_Type_QuestGiver {
public:
    CGGameObject_C_Type_QuestGiver();
    void CloseInteraction();
    void StartInteraction();
};

class CGGameObject_C_Type_Ritual {
public:
    CGGameObject_C_Type_Ritual();
    void CanUseNow();
};

class CGGameObject_C_Type_SpellFocus {
public:
    CGGameObject_C_Type_SpellFocus();
    void CanHighlight();
    void CanUse();
};

class CGGameObject_C_Type_Text {
public:
    CGGameObject_C_Type_Text();
    void CloseInteraction();
    void PostInit();
    void StartInteraction();
    void Use();
};

class CGGameObject_C_Type_Transport {
public:
    void AddPassenger();
    CGGameObject_C_Type_Transport();
    void CanUse();
    void Disable();
    void FindAnimData();
    void GetCurrentMoveVector();
    void GetMovement();
    void GetPosition();
    void IsPointInside();
    void ModelJustLoaded();
    void NextKeyID();
    void Reenable();
    void UpdateMovement();
};

class CGGameObject_C_Type_Trap {
public:
    CGGameObject_C_Type_Trap();
};

class CGObject_C {
public:
    void GetSelectionHighlightColor();
};

// Recovered symbol declarations for objectclient/gameobject_c.cpp


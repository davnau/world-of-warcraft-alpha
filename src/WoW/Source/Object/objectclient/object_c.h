#pragma once

#include <cstddef>

// ObjectModelCreate supplies this recovered name ordering to ModelCreate. The
// numeric ANIMENUMERATION values index this table; model files may store their
// sequence records in another order.
extern const char* g_animationNames[];
extern const std::size_t g_animationNameCount;

class CGObject_C {
public:
    void AddAttachment();
    void AddWorldObject();
    void Animate();
    void AreAttachmentsLoaded();
    CGObject_C();
    void Disable();
    void DoFade();
    void GetCharacterModel();
    void GetObjectName();
    void GetWorldMatrix();
    void HideHighlightType();
    void InitModelFileName();
    void Initialize();
    void IsDisabled();
    void IsInReenable();
    void IsObjectModelLoaded();
    void IsPostInited();
    void ObjectIsRendering();
    void ObjectModelSetBoneSequence();
    void ObjectModelSetSequence();
    void ObjectSetNotRendering();
    void OffsetOf();
    void OnRightClick();
    void PostInit();
    void PostReenable();
    void PreAnimate();
    void Reenable();
    void RemoveWorldObject();
    void ReportMissingAnimObj();
    void ReportMissingAnimation();
    void ReportMissingAttachment();
    void ReportMissingBone();
    void ReportMissingEventObject();
    void ReportNoAnimation();
    void SetAnimated();
    void SetBlock();
    void SetCircleRenderStates();
    void SetData();
    void SetObjectModel();
    void SetStorage();
    void SetTypeID();
    void ShouldRender();
    void ShowHighlightType();
    void Shutdown();
    void UpdateAllWorldObjects();
    void UpdateAttachmentLoadStatus();
    void UpdateModelLoadStatus();
    void UpdateObjectHeight();
    void UpdateWorldObject();
    ~CGObject_C();
};

// Recovered symbol declarations for objectclient/object_c.cpp

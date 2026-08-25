#include "WoW/Source/Object/objectclient/object_c.h"

const char* g_animationNames[] = {
    "Stand", "Death", "Spell", "Stop", "Walk", "Run", "Dead", "Rise",
    "StandWound", "CombatWound", "CombatCritical", "ShuffleLeft",
    "ShuffleRight", "WalkBackwards", "Stun", "HandsClosed",
    "AttackUnarmed", "Attack1H", "Attack2HTight", "Attack2HLoose",
    "ParryUnarmed", "Parry1H", "Parry2HTight", "Parry2HLoose",
    "ParryShield", "ReadyUnarmed", "Ready1H", "Ready2HTight",
    "Ready2HLoose", "ReadyBow", "Dodge", "SpellPrecast", "SpellCast",
    "SpellCastArea", "NPCWelcome", "NPCGoodbye", "Block", "JumpStart",
    "Jump", "JumpEnd", "Fall", "SwimIdle", "Swim", "SwimLeft",
    "SwimRight", "SwimBackwards", "AttackBow", "FireBow", "ReadyRifle",
    "AttackRifle", "Loot", "SpellPrecastDirected", "SpellPrecastOmni",
    "SpellCastDirected", "SpellCastOmni", "SpellBattleRoar",
    "SpellReadyAbility", "SpellSpecial1H", "SpellSpecial2H",
    "SpellShieldBash", "EmoteTalk", "EmoteEat", "EmoteWork",
    "EmoteUseStanding", "EmoteExclamation", "EmoteQuestion", "EmoteBow",
    "EmoteWave", "EmoteCheer", "EmoteDance", "EmoteLaugh", "EmoteSleep",
    "EmoteSitGround", "EmoteRude", "EmoteRoar", "EmoteKneel",
    "EmoteKiss", "EmoteCry", "EmoteChicken", "EmoteBeg", "EmoteApplaud",
    "EmoteShout", "EmoteFlex", "EmoteShy", "EmotePoint",
    "Attack1HPierce", "Attack2HLoosePierce", "AttackOff",
    "AttackOffPierce", "Sheathe", "HipSheathe", "Mount", "RunLeanRight",
    "RunLeanLeft", "MountSpecial", "Kick", "SitDown", "Sitting", "SitUp",
    "SleepDown", "Sleeping", "SleepUp", "SitChairLow", "SitChairMedium",
    "SitChairHigh", "LoadBow", "LoadRifle", "AttackThrown", "ReadyThrown",
    "HoldBow", "HoldRifle", "HoldThrown", "LoadThrown", "EmoteSalute",
    "KneelDown", "Kneeling", "KneelUp", "AttackUnarmedOff",
    "SpecialUnarmed", "StealthWalk", "StealthStand", "Knockdown", "Eating",
    "UseStandingLoop", "ChannelCastDirected", "ChannelCastOmni", "Whirlwind",
    "Birth", "UseStandingStart", "UseStandingEnd", "Howl", "Drown",
    "Drowned", "FishingCast", "FishingLoop"
};

const std::size_t g_animationNameCount =
    sizeof(g_animationNames) / sizeof(g_animationNames[0]);

static_assert(sizeof(g_animationNames) / sizeof(g_animationNames[0]) == 0x87,
              "recovered NUM_OBJECTANIMATIONS");

void CGObject_C::AddAttachment() { }
void CGObject_C::AddWorldObject() { }
void CGObject_C::Animate() { }
void CGObject_C::AreAttachmentsLoaded() { }
CGObject_C::CGObject_C() { }
void CGObject_C::Disable() { }
void CGObject_C::DoFade() { }
void CGObject_C::GetCharacterModel() { }
void CGObject_C::GetObjectName() { }
void CGObject_C::GetWorldMatrix() { }
void CGObject_C::HideHighlightType() { }
void CGObject_C::InitModelFileName() { }
void CGObject_C::Initialize() { }
void CGObject_C::IsDisabled() { }
void CGObject_C::IsInReenable() { }
void CGObject_C::IsObjectModelLoaded() { }
void CGObject_C::IsPostInited() { }
void CGObject_C::ObjectIsRendering() { }
void CGObject_C::ObjectModelSetBoneSequence() { }
void CGObject_C::ObjectModelSetSequence() { }
void CGObject_C::ObjectSetNotRendering() { }
void CGObject_C::OffsetOf() { }
void CGObject_C::OnRightClick() { }
void CGObject_C::PostInit() { }
void CGObject_C::PostReenable() { }
void CGObject_C::PreAnimate() { }
void CGObject_C::Reenable() { }
void CGObject_C::RemoveWorldObject() { }
void CGObject_C::ReportMissingAnimObj() { }
void CGObject_C::ReportMissingAnimation() { }
void CGObject_C::ReportMissingAttachment() { }
void CGObject_C::ReportMissingBone() { }
void CGObject_C::ReportMissingEventObject() { }
void CGObject_C::ReportNoAnimation() { }
void CGObject_C::SetAnimated() { }
void CGObject_C::SetBlock() { }
void CGObject_C::SetCircleRenderStates() { }
void CGObject_C::SetData() { }
void CGObject_C::SetObjectModel() { }
void CGObject_C::SetStorage() { }
void CGObject_C::SetTypeID() { }
void CGObject_C::ShouldRender() { }
void CGObject_C::ShowHighlightType() { }
void CGObject_C::Shutdown() { }
void CGObject_C::UpdateAllWorldObjects() { }
void CGObject_C::UpdateAttachmentLoadStatus() { }
void CGObject_C::UpdateModelLoadStatus() { }
void CGObject_C::UpdateObjectHeight() { }
void CGObject_C::UpdateWorldObject() { }
CGObject_C::~CGObject_C() { }
void GenerateAnimFlags() { }
void ObjectModelCreate() { }
void Object_C_AnimHasHitEvent() { }
void Object_C_GetAnimIndex() { }
void UpdateAllWorldObjectsCallback() { }
void s_BlobFadeTex() { }

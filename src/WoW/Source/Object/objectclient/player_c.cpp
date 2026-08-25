#include "WoW/Source/Object/objectclient/player_c.h"

#include "ENGINE/Source/Services/asyncfileread.h"
#include "WoW/Source/UI/gameui.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowSvcs/wowsvcsclient/clientservices.h"
#include "WoW/Source/loadingscreen.h"

#include <cstdint>

namespace {
constexpr unsigned kSmsgResurrectRequest = 0x014EU;
constexpr unsigned kCmsgRepopRequest = 0x014DU;
constexpr unsigned kCmsgResurrectResponse = 0x014FU;

std::uint64_t s_resurrectOffer = 0U;

int __fastcall ResurrectRequestHandler(void*, unsigned msgId, unsigned,
                                       CDataStore* message) {
    if (msgId != kSmsgResurrectRequest || !message)
        return 0;
    std::uint64_t guid = 0U;
    message->Get(&guid);
    if (message->Failed() || guid == 0U)
        return 0;
    s_resurrectOffer = guid;
    CGPlayer_C player;
    // The recovered client resolves this GUID through its asynchronous name
    // cache before signaling RESURRECT_REQUEST. Preserve the request and UI
    // flow even when that cache has not populated yet.
    player.ReceiveResurrectRequest("A player");
    return 1;
}
}

void CGObject_C::GetPageTextID() { }
void CGPlayer_C::AcceptGroup() { }
void CGPlayer_C::AcceptGuild() { }
void CGPlayer_C::AcceptQuest() { }
void CGPlayer_C::AcceptResurrectRequest(bool accept) {
    if (s_resurrectOffer == 0U)
        return;
    CDataStore message;
    message.Put<std::uint32_t>(kCmsgResurrectResponse)
        ->Put<std::uint64_t>(s_resurrectOffer)
        ->Put<std::uint8_t>(accept ? 1U : 0U);
    ClientServices_Send(&message);
    s_resurrectOffer = 0U;
}
void CGPlayer_C::AddComponent() { }
void CGPlayer_C::AddDeferredDamage() { }
void CGPlayer_C::AddDeferredSpellMiss() { }
void CGPlayer_C::AddKnownSpell() { }
void CGPlayer_C::AttachObjComponent() { }
void CGPlayer_C::AutoEquipCursorItem() { }
void CGPlayer_C::AutoEquipItem() { }
void CGPlayer_C::AutoStoreItemInBag() { }
void CGPlayer_C::AutoStoreLootItem() { }
void CGPlayer_C::BotMove() { }
void CGPlayer_C::BotSpline() { }
void CGPlayer_C::BuyPetition() { }
CGPlayer_C::CGPlayer_C() { }
void CGPlayer_C::CanBeMounted() { }
void CGPlayer_C::CanEngageTarget() { }
void CGPlayer_C::CanLoot() { }
void CGPlayer_C::CanTrack() { }
void CGPlayer_C::CanUseItem() { }
void CGPlayer_C::CancelGiftWrap() { }
void CGPlayer_C::CancelQuest() { }
void CGPlayer_C::ChangeStandState() { }
void CGPlayer_C::CheckDefenseRankChange() { }
void CGPlayer_C::CheckKillerFeedback() { }
void CGPlayer_C::CheckWeaponDefenseRankChange() { }
void CGPlayer_C::CheckWeaponRankChange() { }
void CGPlayer_C::CleanupUnitArtwork() { }
void CGPlayer_C::ClearFarSight() { }
void CGPlayer_C::ClearItemVisuals() { }
void CGPlayer_C::ClearPendingEquip() { }
void CGPlayer_C::CombatLoggingFlagChanged() { }
void CGPlayer_C::CommitTexture() { }
void CGPlayer_C::CompleteQuest() { }
void CGPlayer_C::DeathBindDistanceCompare() { }
void CGPlayer_C::DeclineGroup() { }
void CGPlayer_C::DeclineGuild() { }
void CGPlayer_C::DecrementPendingItemStats() { }
void CGPlayer_C::DelKnownSpell() { }
void CGPlayer_C::DeleteWornItems() { }
void CGPlayer_C::DetermineWoundSequence() { }
void CGPlayer_C::Disable() { }
void CGPlayer_C::DropItemInCursor() { }
void CGPlayer_C::FindItemSlot() { }
void CGPlayer_C::FindSlotIndex() { }
void CGPlayer_C::FixComponenting() { }
void CGPlayer_C::GetAFKText() { }
void CGPlayer_C::GetAttackSkillRank() { }
void CGPlayer_C::GetBag() { }
void CGPlayer_C::GetBindPoint() { }
void CGPlayer_C::GetBlockChance() { }
void CGPlayer_C::GetCombatModeTimerInterval() { }
void CGPlayer_C::GetCraftSkillActivator() { }
void CGPlayer_C::GetCraftSkills() { }
void CGPlayer_C::GetDNDText() { }
void CGPlayer_C::GetDefaultLanguage() { }
void CGPlayer_C::GetDefendingItem() { }
void CGPlayer_C::GetDefenseSkillRank() { }
void CGPlayer_C::GetDodgeChance() { }
void CGPlayer_C::GetExpandedSkillRank() { }
void CGPlayer_C::GetGMText() { }
void CGPlayer_C::GetGUIDAffiliation() { }
void CGPlayer_C::GetLanguageSkill() { }
void CGPlayer_C::GetLocalTarget() { }
void CGPlayer_C::GetLootItem() { }
void CGPlayer_C::GetLootItemDisplayID() { }
void CGPlayer_C::GetLootItemQuantity() { }
void CGPlayer_C::GetModelFileName() { }
void CGPlayer_C::GetMountScale() { }
void CGPlayer_C::GetNewContinentID() { }
void CGPlayer_C::GetPackAndSlot() { }
void CGPlayer_C::GetParryChance() { }
void CGPlayer_C::GetPendingItemExpirationNode() { }
void CGPlayer_C::GetPlayerAnimState() { }
void CGPlayer_C::GetPossessedUnit() { }
void CGPlayer_C::GetProficiency() { }
void CGPlayer_C::GetQuestReward() { }
void CGPlayer_C::GetSkillIndex() { }
void CGPlayer_C::GetSkillRank() { }
void CGPlayer_C::GetSoulstone() { }
void CGPlayer_C::GetSpellCastingTime() { }
void CGPlayer_C::GetSpellRank() { }
void CGPlayer_C::GetTradeSkills() { }
void CGPlayer_C::GetUnitBeingLooted() { }
void CGPlayer_C::GetVirtualItem() { }
void CGPlayer_C::GetVirtualItemDisplayID() { }
void CGPlayer_C::GetWeaponSpell() { }
void CGPlayer_C::GiftWrap() { }
void CGPlayer_C::GiveQuestItems() { }
void CGPlayer_C::GuildInfoLoaded() { }
void CGPlayer_C::HandleActivateTaxiReply() { }
void CGPlayer_C::HandleDismountResult() { }
void CGPlayer_C::HandleMirrorTimerDamage() { }
void CGPlayer_C::HandleMountResult() { }
void CGPlayer_C::HandleRepopRequest() {
    if (!CWorld::IsPlayerDead())
        return;
    CDataStore message;
    message.Put<std::uint32_t>(kCmsgRepopRequest);
    ClientServices_Send(&message);
}
void CGPlayer_C::HasEquipped() { }
void CGPlayer_C::IncrementPendingItemStats() { }
void CGPlayer_C::InitComponents() { }
void CGPlayer_C::InitPreferredGeosets() { }
void CGPlayer_C::Initialize() { }
void CGPlayer_C::InspectPlayer() { }
void CGPlayer_C::InviteToGroup() { }
void CGPlayer_C::IsGiftWrapping() { }
void CGPlayer_C::IsQuestUnit() { }
void CGPlayer_C::ItemReceived() { }
void CGPlayer_C::KillCombatModeTimer() { }
void CGPlayer_C::KillExitCombatModeSheatheTimer() { }
void CGPlayer_C::LeaveGroup() { }
void CGPlayer_C::LootAnimEndHandler() { }
void CGPlayer_C::LootMoney() { }
void CGPlayer_C::LootUnit() { }
void CGPlayer_C::MoveItem() { }
void CGPlayer_C::OffsetOf() { }
void CGPlayer_C::OnAttackBreakHandler() { }
void CGPlayer_C::OnAttackIconPressed() { }
void CGPlayer_C::OnAttackStart() { }
void CGPlayer_C::OnAttackStop() { }
void CGPlayer_C::OnAttackerStateChange() { }
void CGPlayer_C::OnBadAttackFacing() { }
void CGPlayer_C::OnBadAttackPosition() { }
void CGPlayer_C::OnBadAttackTarget() { }
void CGPlayer_C::OnBuyFailed() { }
void CGPlayer_C::OnBuySucceeded() { }
void CGPlayer_C::OnDeath() { }
void CGPlayer_C::OnDeathAnimate() { }
void CGPlayer_C::OnDismount() { }
void CGPlayer_C::OnGuildChanged() { }
void CGPlayer_C::OnItemDelete() { }
void CGPlayer_C::OnLevelChange() { }
void CGPlayer_C::OnLootClearMoney() { }
void CGPlayer_C::OnLootGameObject() { }
void CGPlayer_C::OnLootItemNotify() { }
void CGPlayer_C::OnLootMoneyNotify() { }
void CGPlayer_C::OnLootReleaseResponse() { }
void CGPlayer_C::OnLootRemoved() { }
void CGPlayer_C::OnLootResponse() { }
void CGPlayer_C::OnMount() { }
void CGPlayer_C::OnNotStanding() { }
void CGPlayer_C::OnObjectDestruct() { }
void CGPlayer_C::OnPetitionShowList() { }
void CGPlayer_C::OnPetitionShowSignatures() { }
void CGPlayer_C::OnQuestGiverChooseReward() { }
void CGPlayer_C::OnQuestGiverInvalidQuest() { }
void CGPlayer_C::OnQuestGiverListQuests() { }
void CGPlayer_C::OnQuestGiverQuestComplete() { }
void CGPlayer_C::OnQuestGiverQuestFailed() { }
void CGPlayer_C::OnQuestGiverRequestItems() { }
void CGPlayer_C::OnQuestGiverSendQuest() { }
void CGPlayer_C::OnQuestGiverStatus() { }
void CGPlayer_C::OnSellResponse() { }
void CGPlayer_C::OnSignedResults() { }
void CGPlayer_C::OnSpellFailed() { }
void CGPlayer_C::OnSplitMoneyNotify() { }
void CGPlayer_C::OnStandStateChanged() { }
void CGPlayer_C::OnTaxiNodeStatus() { }
void CGPlayer_C::OnTerrainClick() { }
void CGPlayer_C::OnTrainerList() { }
void CGPlayer_C::OnTurnInPetitionResults() { }
void CGPlayer_C::OnUnitDeath() { }
void CGPlayer_C::OnVendorInventory() { }
void CGPlayer_C::OpenLootItem() { }
void CGPlayer_C::OpenWrappedItem() { }
void CGPlayer_C::PlayDeathThudCameraShake() { }
void CGPlayer_C::PlayMacroSound() { }
void CGPlayer_C::PlayVocalMacro() { }
void CGPlayer_C::PlayerFlagsChanged() { }
void CGPlayer_C::PostInit() { }
void CGPlayer_C::PostReenable() { }
void CGPlayer_C::PostReinitializeArtwork() { }
void CGPlayer_C::PreAnimate() { }
void CGPlayer_C::ProcessDeferredDamage() { }
void CGPlayer_C::ProcessDeferredSpellMiss() { }
void CGPlayer_C::PutLootInSlot() { }
void CGPlayer_C::QueryQuest() { }
void CGPlayer_C::QueryTaxiNodes() { }
void CGPlayer_C::QuestLogRemoveQuest() { }
void CGPlayer_C::QuestLogSwapQuest() { }
void CGPlayer_C::ReadItem() { }
void CGPlayer_C::ReadItemResult() { }
void CGPlayer_C::ReceiveResurrectRequest(const char* name) {
    if (name && *name && CWorld::IsPlayerDead())
        CGGameUI::OpenResurrectRequest(name);
}
void CGPlayer_C::Reenable() { }
void CGPlayer_C::ReinitializeUnitArtwork() { }
void CGPlayer_C::RemoveComponent() { }
void CGPlayer_C::ReportBagItemSubtypeMismatch() { }
void CGPlayer_C::RequestPetitionSignatures() { }
void CGPlayer_C::ResetCombatModeTimer() { }
void CGPlayer_C::SaveBindPoint() { }
void CGPlayer_C::SaveDeathMessage() { }
void CGPlayer_C::SaveTabard() { }
void CGPlayer_C::SellItem() { }
void CGPlayer_C::SendTextEmote() { }
void CGPlayer_C::SetActive() { }
void CGPlayer_C::SetActiveMirrorHandlers() { }
void CGPlayer_C::SetBaseAnimState() { }
void CGPlayer_C::SetBlock() { }
void CGPlayer_C::SetCombatMode() { }
void CGPlayer_C::SetData() { }
void CGPlayer_C::SetEmoteState() { }
void CGPlayer_C::SetFarSightFocus() { }
void CGPlayer_C::SetInventoryMirrorHandler() { }
void CGPlayer_C::SetItemVisuals() { }
void CGPlayer_C::SetLastWeaponModeSent() { }
void CGPlayer_C::SetLootMethod() { }
void CGPlayer_C::SetNewLeader() { }
void CGPlayer_C::SetPlayerMirrorHandlers() { }
void CGPlayer_C::SetStorage() { }
void CGPlayer_C::SetTorsoAnimState() { }
void CGPlayer_C::SheatheWeapon() { }
void CGPlayer_C::ShopFromMerchant() { }
void CGPlayer_C::ShouldRender() { }
void CGPlayer_C::ShouldRenderUnitName() { }
void CGPlayer_C::ShowTaxiNodes() { }
void CGPlayer_C::Shutdown() { }
void CGPlayer_C::SplitItem() { }
void CGPlayer_C::StartGiftWrap() { }
void CGPlayer_C::StartSheatheAnim() { }
void CGPlayer_C::StartTaxi() { }
void CGPlayer_C::SwapInventorySlots() { }
void CGPlayer_C::SwapItems() { }
void CGPlayer_C::TalkToBanker() { }
void CGPlayer_C::TalkToBinder() { }
void CGPlayer_C::TalkToNpcPetition() { }
void CGPlayer_C::TalkToQuestUnit() { }
void CGPlayer_C::TalkToTabardVendor() { }
void CGPlayer_C::TalkToTrainer() { }
void CGPlayer_C::ToggleFarSight() { }
void CGPlayer_C::TogglePlayerBounds() { }
void CGPlayer_C::ToggleSheathe() { }
void CGPlayer_C::TrainerBuySpell() { }
void CGPlayer_C::TrySheathingWeapon() { }
void CGPlayer_C::TurnInGuildCharter() { }
void CGPlayer_C::Uninvite() { }
void CGPlayer_C::UnitHit() { }
void CGPlayer_C::UnsetActiveMirrorHandlers() { }
void CGPlayer_C::UnsetInventoryMirrorHandler() { }
void CGPlayer_C::UnsetPlayerMirrorHandlers() { }
void CGPlayer_C::UpdateBindStatus() { }
void CGPlayer_C::UpdateBindStatusAll() { }
void CGPlayer_C::UpdateObjComponentVisuals() { }
void CGPlayer_C::UpdatePendingItemExpiration() { }
void CGPlayer_C::UpdateQuestStatus() { }
void CGPlayer_C::UpdateQuestStatusAll() { }
void CGPlayer_C::UpdateTaxiStatus() { }
void CGPlayer_C::UpdateTaxiStatusAll() { }
void CGPlayer_C::UpdateUnitNameString() { }
void CGPlayer_C::UseSoulstone() { }
void CGPlayer_C::ValidateSlot() { }
void CGPlayer_C::XBuyItem() { }
void CGPlayer_C::XBuyItemInSlot() { }
CGPlayer_C::~CGPlayer_C() { }
void CGUnit_C::CanBeTargetted() { }
void CGUnit_C::GetFacing() { }
void CGUnit_C::GetGroundNormal() { }
void CGUnit_C::GetObjectName() { }
void CGUnit_C::GetPosition() { }
void CGUnit_C::IsSolidCollidable() { }
void CGUnit_C::IsSolidSelectable() { }
void CGUnit_C::OnGetAttacked() { }
void CGUnit_C::ShouldFadeIn() { }
void AnimEventCallback() { }
void AreaTriggerCheck() { }
void AreaTriggersInitialize() { }
void AreaTriggersShutdown() { }
void AutoEquipStatsCallback() { }
void BankInvHandler() { }
void BindUpdateProc() { }
void BootMeHandler() { }
void BuyCommandHandler() { }
void CCommand_Accept() { }
void CCommand_AcceptRes() { }
void CCommand_Cinematic() { }
void CCommand_Decline() { }
void CCommand_DeclineRes() { }
void CCommand_Disband() { }
void CCommand_DumpDeathHoldLogs() { }
void CCommand_ForceActionOnOtherSet() { }
void CCommand_ForceActionOnOtherUnset() { }
void CCommand_ForceActionSet() { }
void CCommand_ForceActionShowFlags() { }
void CCommand_ForceActionUnset() { }
void CCommand_ForceMonsterAnim() { }
void CCommand_GuildCreate() { }
void CCommand_Invite() { }
void CCommand_LevelUp() { }
void CCommand_NewLeader() { }
void CCommand_ResetMonsterAnim() { }
void CCommand_SetFaction() { }
void CCommand_ShowPet() { }
void CCommand_TaxiShowNodes() { }
void CCommand_TogglePVP() { }
void CCommand_Uninvite() { }
void CharmChangeHandler() { }
void CountWeaponItemSubclasses() { }
void DuelTeamUpdateHandler() { }
void DumpDeathLogEnumHandler() { }
void FarsightChangeHandler() { }
void FindEmptySwapIndex() { }
void FindFirstSetBit() { }
void GodmodeHandler() { }
void GuildCallback() { }
void GuildCharterTurnInCallback() { }
void GuildIDUpdateHandler() { }
void GuildTimestampChanged() { }
void HandlePartyMemberStats() { }
void InitializeWeaponSubclassSpells() { }
void MirrorTimerLabel() { }
void MirrorTimerToName() { }
void OnActionButtons() { }
void OnCancelCombat() { }
void OnFactionUpdate() { }
void OnGroupCancel() { }
void OnGroupCommandResult() { }
void OnGroupDecline() { }
void OnGroupDestroy() { }
void OnGroupInvite() { }
void OnGroupList() { }
void OnGroupNewLeader() { }
void OnGroupUninvite() { }
void OnGuildCommandResult() { }
void OnGuildDecline() { }
void OnGuildEmblemActivate() { }
void OnGuildEmblemError() { }
void OnGuildEvent() { }
void OnGuildInfo() { }
void OnGuildInvite() { }
void OnGuildRoster() { }
void OnInitialSpells() { }
void OnInspectNotify() { }
void OnItemEvent() { }
void OnLearnedSpell() { }
void OnLootEvent() { }
void OnMirrorTimerEvent() { }
void OnNpcPetitionEvent() { }
void OnPetSpells() { }
void OnPlayEmote() { }
void OnPlayerEvent() { }
void OnProficiency() { }
void OnQuestConfirm() { }
void OnQuestGiverEvent() { }
void OnQuestItemLoot() { }
void OnQuestUpdate() { }
void OnReadItemResult() { }
void OnResurrectRequest() { }
void OnSupercededSpell() { }
void OnTrainerEvent() { }
void OnUpdateGuild() { }
void OnUpdateInventoryComponent() { }
void OnUpdateMoney() { }
void OnUpdatePlayerFlags() { }
void OnUpdateQuest() { }
void OnUpdateShapeshiftForm() { }
void OnVendorEvent() { }
void PetChangeHandler() { }
void PlayerAttackBreakHandler() { }
void PlayerClientInitialize() {
    s_resurrectOffer = 0U;
    ClientServices_SetMessageHandler(kSmsgResurrectRequest,
                                     ResurrectRequestHandler, nullptr);
}
void PlayerClientShutdown() { }
void PlayerCombatModeHandler() { }
void Player_C_AppFocusMovementHandler() { }
void Player_C_ClearGuildIDs() { }
void Player_C_GetDisplayId() { }
void Player_C_GetModelName() { }
void Player_C_RegisterGuildUpdate() { }
void Player_C_SetPlayerRender() { }
void Player_C_TogglePlayerRender() { }
void Player_C_ZoneUpdateHandler() { }
void PrintForceActionUsage() { }
void QuestAcceptedCallback() { }
void QuestCompleteCallback() { }
void QuestFailedCallback() { }
void QuestLootQuestQueryCallback() { }
void QuestRewardItemCallback() { }
void QuestUpdateProc() { }
void RandomRollNameQueryCallback() { }
void RepopPlayerHandler() { }
void ResurrectNameQueryCallback() { }
void SendForceActionMessage() { }
int SetLocalPlayerInGame() {
    ClientServices_CharacterSetInGame(1);
    AsyncFileReadWaitAll();
    DisableLoadingScreen();
    return 1;
}
void ShowForceActionFlags() { }
void SkillMaxRankChangeHandler() { }
void SkillModifierChangeHandler() { }
void SkillRankChangeHandler() { }
void SoulStoneCompare() { }
void SwapItemsStatsCallback() { }
void TaxiUpdateProc() { }
void UndressMeHandler() { }
void WhoCommandHandler() { }

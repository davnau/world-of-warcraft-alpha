#pragma once

#include <cstdint>

class CGGameUI {
public:
    void AddErrorMessage();
    void AddPartyMember();
    void AssistByName();
    void BeginCinematic();
    void BeginCinematicInternal();
    void CancelGuildInvite();
    void CancelPartyInvite();
    void ClearClientControls();
    void ClearCursor();
    void ClearInteractTarget();
    void ClearLootSlot();
    void ClearTarget();
    void CloseInteraction();
    void CloseLoot();
    void ClosestObjectMatch();
    void DeleteCursorItem();
    void DisplayError();
    void DropCursorPetAction();
    void DropCursorSpell();
    void EnablePartyMember();
    static void EnterWorld();
    void FilterMouseDown();
    void FollowByName();
    void GetCursorItem();
    void GetCursorSpell();
    void GetCursorVirtualItem();
    void GetLastErrorString();
    void GetPartyMember();
    void HandleDisplaySizeChanged();
    static int HandleMouseDown();
    static int HandleMouseUp();
    void HandleObjectTrackChange();
    void HandleScreenshot();
    void HandleSpriteClick();
    void HandleSpriteTrack();
    void HandleTerrainClick();
    void HandleWorldClick();
    void HideCursor();
    void Idle();
    static void Initialize();
    static void InitializeGame();
    void IsPartyMember();
    void LeaveWorld();
    void LockItem();
    void NamePlateClicked();
    void NewZoneFeedback();
    void NextCinematic();
    void NextCinematicInternal();
    void OnClientControlChanged();
    void OnItemPush();
    void OnSpriteLeftClick();
    void OnSpriteRightClick();
    void OnTargetContextAction();
    void OnTerrainClick();
    void OpenGuildInvite();
    void OpenLoot();
    void OpenPartyInvite();
    static void OpenResurrectRequest(const char* name);
    void PlayerCombatModeChanged();
    static void RegisterFrameFactories();
    void Reload();
    void RemoveAllPartyMembers();
    void ResetCamera();
    void ScaleUI();
    void SetCursorItem();
    void SetCursorMoney();
    void SetCursorPetAction();
    void SetCursorSpell();
    void SetCursorVirtualItem();
    void SetInteractTarget();
    void SetLootMethod();
    void SetMinimapZoneText();
    void SetPartyLeader();
    void ShowAutoFollowChange();
    void ShowCombatFeedback();
    void ShowCursor();
    void ShowHealingFeedback();
    void ShowSpellMissFeedback();
    void Shutdown();
    void ShutdownGame();
    void StartCinematic();
    void StartCinematicCamera();
    void StopCinematic();
    void StopCinematicInternal();
    void SysMsgDisplay();
    static void Target(std::uint64_t guid, bool usingNearest = false);
    void TargetNearestEnemy();
    void UnitNameUpdate();
    void UnitPortraitUpdate();
    void UnlockAllItems();
    void UnlockItem();
    void UpdateActivePlayer();
    void UpdateInteractTarget();
    void UpdateObjectHighlightColor();
};

class CGTabardModelFrame {
public:
    void Create();
};

class GAMEERRORDESC {
public:
    GAMEERRORDESC();
};

#pragma once

class CGQuestLog {
public:
    void AbandonQuest();
    void AbandonSelectedQuest();
    void ClearQuest();
    void CollapseHeader();
    void EnterWorld();
    void FilterAndSortQuests();
    void GetAbandonQuestName();
    void GetQuestItemID();
    void GetQuestLevel();
    void GetQuestName();
    void GetQuestSortIndex();
    void GetQuestTag();
    void GetSelectedLogEntry();
    void GetSelectionIndex();
    void InitializeGame();
    void IsQuestExpired();
    void IsSelectedQuestExpired();
    void LeaveWorld();
    void SetQuestExpired();
    void SetSelectedQuest();
    void ShutdownGame();
    void Update();
    void UpdateSelection();
    void UpdateServerTime();
};



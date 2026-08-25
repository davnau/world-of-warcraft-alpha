#pragma once

class CGQuestInfo {
public:
    void AcceptQuest();
    void AddItemRequest();
    void AddQuest();
    void AddQuestInProgress();
    void AddReward();
    void CompleteQuest();
    void ConfirmAcceptQuest();
    void DeclineQuest();
    void EndQuestList();
    void EnterWorld();
    void GetNumQuestChoices();
    void GetNumQuestItems();
    void GetNumQuestRewards();
    void GetQuestItemID();
    void GetQuestItemInfo();
    void GetReward();
    void GiveQuestItems();
    void IsCompletable();
    void LeaveWorld();
    void QueryQuest();
    void QuestGiverFinished();
    void SetLogDescription();
    void SetState();
};



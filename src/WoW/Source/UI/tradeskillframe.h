#pragma once

class CGTradeSkillInfo {
public:
    void ClearItemCallbacks();
    void Close();
    void EnterWorld();
    void FilterAndSortSkills();
    void GetSelectionIndex();
    void GetSubClassIndexFromSkill();
    void IsCollpasedHeader();
    void LeaveWorld();
    void RefreshList();
    void SetCollapseFilter();
    void SetInvTypeFilter();
    void SetSelection();
    void SetSkillLine();
    void SetSubClassFilter();
    void ShutdownGame();
};



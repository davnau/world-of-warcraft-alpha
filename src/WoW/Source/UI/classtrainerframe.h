#pragma once

class CGClassTrainer {
public:
    void AddServices();
    void EnterWorld();
    void FilterAndSortServices();
    void GetSelectionIndex();
    void GetService();
    void GetServiceName();
    void GetServiceSubtext();
    void GetServiceType();
    void GetSkillLineIndexFromService();
    void InitializeGame();
    void IsCollpasedHeader();
    void LeaveWorld();
    void RefreshList();
    void SetCollapseFilter();
    void SetSelection();
    void SetServiceTypeFilter();
    void SetSkillLineFilter();
    void SetTrainer();
    void ShutdownGame();
};



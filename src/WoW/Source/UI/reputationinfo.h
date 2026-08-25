#pragma once

class CGReputationInfo {
public:
    void EnterWorld();
    void FactionToIndex();
    void GetFactionFromSortIndex();
    void GetFactionStanding();
    void GetFactionStandingReaction();
    void IndexToFaction();
    void IsAtWar();
    void LeaveWorld();
    void OnInitializeFactions();
    void OnSetFactionStanding();
    void OnSetFactionVisible();
    void SetAtWar();
    void SetFactionFlags();
    void SetFactionStanding();
    void ShutdownGame();
    void SortFactions();
};



#pragma once

class FriendList {
public:
    void AddFriend();
    void AddFriends();
    void AddIgnore();
    void AddOrDelIgnore();
    void Added();
    void DecrementPendingFriendName();
    void DecrementPendingIgnoreName();
    void DelIgnore();
    void Destroy();
    FriendList();
    void GetFriend();
    void GetFriendSelectionIndex();
    void GetIgnore();
    void GetIgnoreSelectionIndex();
    void GetNumFriends();
    void GetNumIgnores();
    void HandleStatus();
    void IgnoreAdded();
    void IgnoreList();
    void IgnoreRemoved();
    void Initialize();
    void IsIgnored();
    void RegisterScriptFunctions();
    void RemoveFriend();
    void Removed();
    void SendWho();
    void SetConnected();
    void SetFriendSelectionIndex();
    void SetIgnoreSelectionIndex();
    void SetName();
    void ShowFriends();
    void SortFriends();
    void SortIgnore();
    void UnregisterScriptFunctions();
    ~FriendList();
};



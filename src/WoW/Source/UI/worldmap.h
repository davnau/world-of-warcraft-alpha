#pragma once

class CGWorldMap {
public:
    void EnterWorld();
    void GetBindPosition();
    void GetContinentName();
    void GetMapAreaFromPos();
    void GetMapFilename();
    void GetMapHeight();
    void GetMapHighlight();
    void GetPOIPosition();
    void GetPlayerPosition();
    void GetPortLocPosition();
    void GetWorldLocFromPos();
    void GetWorldPosition();
    void GetZoneName();
    void InitializeGame();
    void LeaveWorld();
    void ProcessClick();
    void RunNearestPortLoc();
    void SetMap();
    void SetMapToCurrentZone();
    void ShutdownGame();
};



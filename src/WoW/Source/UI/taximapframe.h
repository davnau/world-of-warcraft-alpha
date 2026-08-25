#pragma once

class CGTaxiMap {
public:
    void CloseMap();
    void EnterWorld();
    void InitializeGame();
    void LeaveWorld();
    void RegisterScriptFunctions();
    void SetupMap();
    void ShutdownGame();
    void TakeTaxiNode();
    void TaxiNodeCost();
    void TaxiNodeName();
    void TaxiNodePosition();
    void TaxiNodeType();
    void UnregisterScriptFunctions();
};



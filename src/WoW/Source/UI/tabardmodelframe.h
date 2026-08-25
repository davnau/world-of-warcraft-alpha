#pragma once

class CGTabardModelFrame {
public:
    CGTabardModelFrame();
    void CanSaveTabard();
    void CycleVariation();
    void GetUniquePaperDollModel();
    void InitializeModel();
    void InitializeTabardColors();
    void LookupScriptMethod();
    void RegisterScriptMethods();
    void SaveTabard();
    void UnregisterScriptMethods();
    void UpdateTabard();
};

class CSimpleFrame {
public:
    void OnLayerMouseMoveRelative();
};



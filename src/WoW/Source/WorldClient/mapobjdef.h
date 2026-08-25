#pragma once

class CMapObjDef {
public:
    CMapObjDef();
    ~CMapObjDef();
    bool initialized = false;
};

class CMapObjDefGroup {
public:
    CMapObjDefGroup();
    void SelectLights();
    void Update();
    void UpdateLights();
    ~CMapObjDefGroup();
    bool lightsSelected = false;
    unsigned updateCount = 0;
};

// Recovered symbol declarations for mapobjdef.cpp

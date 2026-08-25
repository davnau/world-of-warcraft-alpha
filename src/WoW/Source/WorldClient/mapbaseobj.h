#pragma once

class CMapBaseObj {
public:
    CMapBaseObj();
    void SelectLights();
    void TestAABox();
    ~CMapBaseObj();
    bool initialized = false;
    unsigned lightSelectionCount = 0;
};

// Recovered symbol declarations for mapbaseobj.cpp

#pragma once

class CMap {
public:
    void SnapBaseObjToSubChunk();
    void Update();
    void UpdateChunks();
    void UpdateDoodadDef();
    void UpdateMapObjDef();
    void UpdateMapObjDefs();
    unsigned updateCount = 0;
    bool baseObjectsSnapped = false;
};

// Recovered symbol declarations for mapupdate.cpp

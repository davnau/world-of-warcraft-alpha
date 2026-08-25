#pragma once

class CMap {
public:
    void Purge();
    void PurgeArea();
    void PurgeChunk();
    void PurgeDoodadDef();
    void PurgeMapObjDef();
    void PurgeMapObjDefGroup();
    unsigned purgeCount = 0;
};

class CMapArea {
public:
    void Purge();
    void PurgeChunks();
    unsigned purgeCount = 0;
};

class CMapChunk {
public:
    void Purge();
    void PurgeLayer();
    unsigned purgeCount = 0;
};

// Recovered symbol declarations for mappurge.cpp

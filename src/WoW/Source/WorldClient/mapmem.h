#pragma once

class CMap {
public:
    void AllocArea();
    void AllocBaseObjLink();
    void AllocCacheLight();
    void AllocChunk();
    void AllocChunkLiquid();
    void AllocDoodadDef();
    void AllocEntity();
    void AllocLight();
    void AllocMapObj();
    void AllocMapObjDef();
    void AllocMapObjDefGroup();
    void AllocMapObjGroup();
    void AllocSoundEmitter();
    void FreeArea();
    void FreeBaseObjLink();
    void FreeCacheLight();
    void FreeChunk();
    void FreeChunkLiquid();
    void FreeDoodadDef();
    void FreeEntity();
    void FreeLayer();
    void FreeLight();
    void FreeMapObj();
    void FreeMapObjDef();
    void FreeMapObjDefGroup();
    void FreeMapObjGroup();
    void FreeSoundEmitter();
    void FreeTex();
    void GetLayer();
    void GetTex();
};

class SWFlowv {
public:
    SWFlowv();
};

// Recovered symbol declarations for mapmem.cpp


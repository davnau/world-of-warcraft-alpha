#pragma once

class CMapArea {
public:
    void AllocAsyncLoadBuffer();
    void AsyncCallback();
    void AsyncPollHandler();
    CMapArea();
    void Create();
    void Destroy();
    void FreeAsyncLoadBuffer();
    void InitAsyncLoadBuffers();
    void Initialize();
    void Load();
    void LoadTextures();
    void PrepareLocalRect();
    ~CMapArea();
};

class SMDoodadDef {
public:
    ~SMDoodadDef();
};

class SMMapObjDef {
public:
    ~SMMapObjDef();
};

// Recovered symbol declarations for maparea.cpp


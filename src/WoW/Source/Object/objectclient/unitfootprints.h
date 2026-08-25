#pragma once

class CHUNKDATA {
public:
    void Add();
    void GetVertCount();
    void RecycleSplat();
    void Render();
    ~CHUNKDATA();
};

class LISTBASE {
public:
    void Add();
    void FindChunk();
    LISTBASE();
    void Render();
    void SetTexture();
    ~LISTBASE();
};

class PERSISTENTTEXTURE {
public:
    void MakeSpace();
    PERSISTENTTEXTURE();
};

class SPLATDATA {
public:
    void Update();
};

class TIMEDTEXTURE {
public:
    void MakeSpace();
};

class VERTDATA {
public:
    VERTDATA();
};

// Recovered symbol declarations for objectclient/unitfootprints.cpp


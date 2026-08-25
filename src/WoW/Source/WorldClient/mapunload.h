#pragma once

class CMap {
public:
    void Unload();
    bool loaded = true;
    unsigned unloadCount = 0;
};

class CMapAreaLow {
public:
    ~CMapAreaLow();
    bool released = false;
};

class DNGlare {
public:
    void Destroy();
    bool destroyed = false;
};

class DNPlanet {
public:
    void Destroy();
    bool destroyed = false;
};

class DNStars {
public:
    void Destroy();
    bool destroyed = false;
};

// Recovered symbol declarations for mapunload.cpp

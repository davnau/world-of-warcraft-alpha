#pragma once

class CMap {
public:
    void CreateDoodadDef();
    void CreateMapObjDef();
    void CreateMapObjDefGroupDoodads();
    void CreateMapObjDefGroups();
    void CreateMapObjDefLights();
    void EnableDoodadFullAlpha();
    void InitializeDoodadBounds();
    void Load();
    void LoadDoodadModel();
    void LoadDoodadNames();
    void LoadMapObjNames();
    void LoadWdl();
    void LoadWdt();
    void Open();
    void Preload();
    void ReloadDoodadModels();
};

class DNGlare {
public:
    void Initialize();
};

class DNPlanet {
public:
    void Initialize();
};

class DNStars {
public:
    void Initialize();
};

// Recovered symbol declarations for mapload.cpp


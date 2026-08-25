#pragma once

class CMap {
public:
    void CreateAreaLowDetailIndices();
    void CreateAreaLowDetailVertices();
    void GxBufDynLowDetailCallback();
    void RenderAreaLow();
    void RenderLow();
    void TestQueryRender();
};

class DNClouds {
public:
    void Render();
};

class DNGlare {
public:
    void IsVisible();
    void Render();
};

class DNPlanet {
public:
    void Render();
};

class DNSky {
public:
    void Render();
};

class DNStars {
public:
    void Render();
};

// Recovered symbol declarations for maprender.cpp


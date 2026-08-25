#pragma once

class CMapChunk {
public:
    void LodCreateTree();
    bool lodTreeCreated = false;
    unsigned lodRevision = 0;
};

// Recovered symbol declarations for maplod.cpp

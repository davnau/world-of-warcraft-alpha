#pragma once

class CMapChunk {
public:
    void UpdateClipBuffer();
    bool clipBufferValid = false;
    unsigned clipRevision = 0;
};

// Recovered symbol declarations for mapclip.cpp

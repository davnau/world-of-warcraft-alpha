#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "ENGINE/Source/Gx/gxapi/gxbase.h"

class CGxBuf {
public:
    virtual ~CGxBuf() = default;
    EGxBufWriteFreq writeFreq = GxBWF_Dynamic;
    EGxVertexBufferFormat vertexFormat = GxVBF_PN;
    std::vector<std::uint8_t> vertexData;
    std::vector<std::uint8_t> indexData;
    unsigned vertexStride = 0;
    unsigned vertexCount = 0;
    unsigned indexCount = 0;
    bool vertexLocked = false;
    bool indexLocked = false;
    void* userArg = nullptr;
    CGxBufUserCallback userCallback = nullptr;
};

class CGxBufD3d : public CGxBuf {
public:
    ~CGxBufD3d() override;
    bool Create(void* device, unsigned vertexBytes, unsigned indexBytes);
    void Release();
    bool LockVB(void** data, unsigned bytes, unsigned offset = 0);
    bool LockIB(void** data, unsigned bytes, unsigned offset = 0);
    void UnlockVB();
    void UnlockIB();
    bool VBLValid() const;
    bool IBValid() const;
    void SetVBL(void* buffer);
    void SetVB(void* buffer);
    void SetIB(void* buffer);
    void UnsetVB();
    void UnsetIB();

    void* vertexBuffer = nullptr;
    void* indexBuffer = nullptr;
};

class CGxBufOgl : public CGxBuf {
public:
    bool LockVB(void** data, unsigned bytes, unsigned offset = 0);
    bool LockIB(void** data, unsigned bytes, unsigned offset = 0);
    void UnlockVB();
    void UnlockIB();
};

class CGxMemBuffer_VAR {
public:
    CGxMemBuffer_VAR(unsigned countValue, void* memoryValue)
        : count(countValue), memory(memoryValue) {}
    void Lock(void** destination, unsigned bytes, unsigned base = 0);
    void Unlock();
    void Fence();

    unsigned count = 0;
    void* memory = nullptr;
    bool locked = false;
};

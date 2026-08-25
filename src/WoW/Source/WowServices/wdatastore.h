#pragma once

#include <cstdint>

class WDataStore {
public:
    static void StaticInitialize();
    static void StaticDestroy();

    void InternalInitialize(std::uint8_t** data, unsigned* base, unsigned* alloc);
    void InternalDestroy(std::uint8_t** data, unsigned* base, unsigned* alloc);
    int InternalFetchWrite(unsigned pos, unsigned bytes, std::uint8_t** data,
                           unsigned* base, unsigned* alloc,
                           const char* fileName, int lineNumber);

    static int AllocBuffer(unsigned bytes);
    static void FreeBuffer(void* data, unsigned bytes);

    void* m_bufferObj = nullptr;
};

int _E2_260();
int _E1_260();
int _E5_257();
void _E4_257();
int _E8_257();
void _E7_257();


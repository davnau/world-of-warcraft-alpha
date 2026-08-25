#pragma once

#include <cstdint>

struct ObjDataDescriptor {
    const char* debugName = nullptr;
    std::uint32_t fieldName = 0;
    std::uint32_t fieldSize = 0;
    std::uint32_t fieldMirrorType = 0;
    std::uint32_t fieldMirrorFlags = 0;
};

void CopyAndExpandDescriptors(ObjDataDescriptor* source,
                              const ObjDataDescriptor* descriptors,
                              unsigned int count, unsigned int destinationSize,
                              ObjDataDescriptor* destination);
void MirrorInitialize(const ObjDataDescriptor* objectDescriptors);
const ObjDataDescriptor* MirrorGetObjDataDescriptor(unsigned int blockID,
                                                    unsigned int index);

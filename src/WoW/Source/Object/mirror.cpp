#include "WoW/Source/Object/mirror.h"

#include <algorithm>
#include <array>
#include <vector>

namespace {

std::vector<ObjDataDescriptor> s_objDescriptors;
std::vector<ObjDataDescriptor> s_itemDescriptors;
std::vector<ObjDataDescriptor> s_containerDescriptors;
std::vector<ObjDataDescriptor> s_unitDescriptors;
std::vector<ObjDataDescriptor> s_playerDescriptors;
std::vector<ObjDataDescriptor> s_gameObjectDescriptors;
std::vector<ObjDataDescriptor> s_dynamicObjectDescriptors;
std::vector<ObjDataDescriptor> s_corpseDescriptors;

void Expand(std::vector<ObjDataDescriptor>& destination,
            const ObjDataDescriptor* descriptors, unsigned int count,
            unsigned int destinationSize) {
    destination.clear();
    destination.reserve(destinationSize);
    for (unsigned int i = 0; i < count; ++i) {
        const ObjDataDescriptor& descriptor = descriptors[i];
        const unsigned int repeats = descriptor.fieldSize ? descriptor.fieldSize : 1;
        for (unsigned int j = 0; j < repeats && destination.size() < destinationSize; ++j)
            destination.push_back(descriptor);
    }
    while (destination.size() < destinationSize)
        destination.push_back({});
}

} // namespace

void CopyAndExpandDescriptors(ObjDataDescriptor* source,
                              const ObjDataDescriptor* descriptors,
                              unsigned int count, unsigned int destinationSize,
                              ObjDataDescriptor* destination) {
    if (!descriptors || !destination) return;
    unsigned int written = 0;
    for (unsigned int i = 0; i < count && written < destinationSize; ++i) {
        const unsigned int repeats = descriptors[i].fieldSize ? descriptors[i].fieldSize : 1;
        for (unsigned int j = 0; j < repeats && written < destinationSize; ++j)
            destination[written++] = source ? source[i] : descriptors[i];
    }
    while (written < destinationSize) destination[written++] = {};
}

void MirrorInitialize(const ObjDataDescriptor* objectDescriptors) {
    if (objectDescriptors)
        s_objDescriptors.assign(objectDescriptors, objectDescriptors + 5);
    else
        s_objDescriptors.assign(5, {});
    Expand(s_itemDescriptors, s_objDescriptors.data(), 5, 30);
    Expand(s_containerDescriptors, s_itemDescriptors.data(),
           std::min(36u, static_cast<unsigned>(s_itemDescriptors.size())), 42);
    Expand(s_unitDescriptors, s_objDescriptors.data(), 5, 178);
    Expand(s_playerDescriptors, s_unitDescriptors.data(),
           std::min(184u, static_cast<unsigned>(s_unitDescriptors.size())), 634);
    Expand(s_gameObjectDescriptors, s_objDescriptors.data(), 5, 14);
    Expand(s_dynamicObjectDescriptors, s_objDescriptors.data(), 5, 10);
    Expand(s_corpseDescriptors, s_objDescriptors.data(), 5, 30);
}

const ObjDataDescriptor* MirrorGetObjDataDescriptor(unsigned int blockID,
                                                    unsigned int index) {
    const std::vector<ObjDataDescriptor>* descriptors = nullptr;
    switch (blockID) {
    case 1: descriptors = &s_objDescriptors; break;
    case 3: descriptors = &s_itemDescriptors; break;
    case 7: descriptors = &s_containerDescriptors; break;
    case 9: descriptors = &s_unitDescriptors; break;
    case 0x19: descriptors = &s_playerDescriptors; break;
    case 0x21: descriptors = &s_gameObjectDescriptors; break;
    case 0x41: descriptors = &s_dynamicObjectDescriptors; break;
    case 0x81: descriptors = &s_corpseDescriptors; break;
    default: return nullptr;
    }
    if (!descriptors || index >= descriptors->size()) return nullptr;
    return &(*descriptors)[index];
}

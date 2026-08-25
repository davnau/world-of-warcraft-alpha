#include "WoW/Source/Object/gameobject.h"

#include <algorithm>

void GenerateChairPoints(const NTempest::C44Matrix* slots, unsigned slotCount,
                         NTempest::C3Vector* output) {
    if (!slots || !output || slotCount == 0)
        return;
    slotCount = std::min(slotCount, 5u);
    NTempest::C3Vector current(0.0F, -0.5F * static_cast<float>(slotCount - 1), 0.0F);
    for (unsigned i = 0; i < slotCount; ++i) {
        output[i] = (*slots) * current;
        current.y += 1.0F;
    }
}

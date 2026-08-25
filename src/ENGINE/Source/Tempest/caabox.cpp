#include "ENGINE/Source/Tempest/caabox.h"

namespace NTempest {

CAaBox& CAaBox::Bounding(const C3Vector* vectors, unsigned int count) {
    if (!vectors || count == 0) {
        b = C3Vector();
        t = C3Vector();
        return *this;
    }

    b = vectors[0];
    t = vectors[0];
    for (unsigned int index = 1; index < count; ++index) {
        b = C3Vector::Min(b, vectors[index]);
        t = C3Vector::Max(t, vectors[index]);
    }
    return *this;
}

} // namespace NTempest

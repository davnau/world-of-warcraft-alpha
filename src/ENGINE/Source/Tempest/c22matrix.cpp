#include "ENGINE/Source/Tempest/c22matrix.h"

#include <cmath>

namespace NTempest {

// PDB source: D:\build\buildWoW\ENGINE\Source\Tempest\c22matrix.cpp
// RVA: 0x003C8280
// Evidence: WoWClient_recovered\_external\d\build\buildwow\engine\source\tempest\c22matrix.cpp/.asm
C22Matrix& C22Matrix::Rotation(float angle) {
    const float cosine = std::cos(angle);
    const float sine = std::sin(angle);

    a0 = cosine;
    a1 = sine;
    b0 = -sine;
    b1 = cosine;
    return *this;
}

} // namespace NTempest

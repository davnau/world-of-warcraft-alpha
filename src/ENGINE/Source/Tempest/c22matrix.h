#pragma once

namespace NTempest {

// Reconstructed from the PDB type record for NTempest::C22Matrix.
// Field order is retained from the recovered layout.
class C22Matrix {
public:
    float a0;
    float a1;
    float b0;
    float b1;

    C22Matrix& Rotation(float angle);
};

static_assert(sizeof(C22Matrix) == sizeof(float) * 4, "C22Matrix layout must remain binary-compatible.");

} // namespace NTempest

#include "ENGINE/Source/Tempest/c44matrix.h"

#include "ENGINE/Source/Tempest/c33matrix.h"
#include "ENGINE/Source/Tempest/c4quaternion.h"

#include <algorithm>
#include <cmath>

namespace NTempest {

double C44Matrix::Det(float a, float b, float c,
                      float d, float e, float f,
                      float g, float h, float i) {
    return b * f * g + c * d * h + a * e * i
         - c * e * g - b * d * i - a * f * h;
}

bool operator==(const C44Matrix& l, const C44Matrix& r) {
    return std::equal(l.Data(), l.Data() + 16, r.Data());
}

bool operator!=(const C44Matrix& l, const C44Matrix& r) {
    return !(l == r);
}

C44Matrix operator*(const C44Matrix& l, const C44Matrix& r) {
    // Tempest composes matrices in expression order for its row-vector API.
    // Its storage is also passed directly to OpenGL, so this is the reverse
    // of conventional column-vector multiplication: result = r * l.
    C44Matrix result;
    for (int column = 0; column < 4; ++column) {
        for (int row = 0; row < 4; ++row) {
            result.Data()[column * 4 + row] = 0.0F;
            for (int k = 0; k < 4; ++k) {
                result.Data()[column * 4 + row] +=
                    r.Data()[k * 4 + row] * l.Data()[column * 4 + k];
            }
        }
    }
    return result;
}

C4Vector operator*(const C44Matrix& matrix, const C4Vector& value) {
    return {
        matrix.a0 * value.x + matrix.b0 * value.y
            + matrix.c0 * value.z + matrix.d0 * value.w,
        matrix.a1 * value.x + matrix.b1 * value.y
            + matrix.c1 * value.z + matrix.d1 * value.w,
        matrix.a2 * value.x + matrix.b2 * value.y
            + matrix.c2 * value.z + matrix.d2 * value.w,
        matrix.a3 * value.x + matrix.b3 * value.y
            + matrix.c3 * value.z + matrix.d3 * value.w,
    };
}

C3Vector operator*(const C44Matrix& matrix, const C3Vector& value) {
    const C4Vector result = matrix * C4Vector(value.x, value.y, value.z, 1.0F);
    if (result.w != 0.0F && result.w != 1.0F)
        return {result.x / result.w, result.y / result.w, result.z / result.w};
    return {result.x, result.y, result.z};
}

C44Matrix& C44Matrix::operator+=(const C44Matrix& value) {
    for (int i = 0; i < 16; ++i) Data()[i] += value.Data()[i];
    return *this;
}

C44Matrix& C44Matrix::operator-=(const C44Matrix& value) {
    for (int i = 0; i < 16; ++i) Data()[i] -= value.Data()[i];
    return *this;
}

C44Matrix& C44Matrix::operator*=(const C44Matrix& value) {
    return *this = *this * value;
}

C44Matrix& C44Matrix::operator*=(float value) {
    for (int i = 0; i < 16; ++i) Data()[i] *= value;
    return *this;
}

C44Matrix& C44Matrix::operator/=(float value) {
    return *this *= 1.0F / value;
}

C44Matrix operator+(C44Matrix l, const C44Matrix& r) { return l += r; }
C44Matrix operator-(C44Matrix l, const C44Matrix& r) { return l -= r; }
C44Matrix operator*(C44Matrix matrix, float value) { return matrix *= value; }
C44Matrix operator*(float value, C44Matrix matrix) { return matrix *= value; }
C44Matrix operator/(C44Matrix matrix, float value) { return matrix /= value; }

C44Matrix C44Matrix::Transpose() const {
    C44Matrix result;
    for (int column = 0; column < 4; ++column) {
        for (int row = 0; row < 4; ++row)
            result.Data()[column * 4 + row] = Data()[row * 4 + column];
    }
    return result;
}

double C44Matrix::Determinant() const {
    double result = 0.0;
    for (int column = 0; column < 4; ++column) {
        float minor[9];
        int index = 0;
        for (int minorRow = 1; minorRow < 4; ++minorRow) {
            for (int minorColumn = 0; minorColumn < 4; ++minorColumn) {
                if (minorColumn != column)
                    minor[index++] = Data()[minorRow * 4 + minorColumn];
            }
        }
        result += (column & 1 ? -1.0 : 1.0) * Data()[column]
            * Det(minor[0], minor[1], minor[2], minor[3], minor[4],
                  minor[5], minor[6], minor[7], minor[8]);
    }
    return result;
}

C44Matrix C44Matrix::Cofactors() const {
    C44Matrix result;
    for (int row = 0; row < 4; ++row) {
        for (int column = 0; column < 4; ++column) {
            float minor[9];
            int index = 0;
            for (int minorRow = 0; minorRow < 4; ++minorRow) {
                if (minorRow == row) continue;
                for (int minorColumn = 0; minorColumn < 4; ++minorColumn) {
                    if (minorColumn != column)
                        minor[index++] = Data()[minorRow * 4 + minorColumn];
                }
            }
            result.Data()[row * 4 + column] = static_cast<float>(
                ((row + column) & 1 ? -1.0 : 1.0)
                * Det(minor[0], minor[1], minor[2], minor[3], minor[4],
                      minor[5], minor[6], minor[7], minor[8]));
        }
    }
    return result;
}

C44Matrix C44Matrix::Adjoint() const { return Cofactors().Transpose(); }

C44Matrix C44Matrix::Inverse(float epsilon) const {
    const float determinant = static_cast<float>(Determinant());
    if (std::fabs(determinant) <= epsilon) return {};
    return Adjoint() / determinant;
}

C44Matrix C44Matrix::AffineInverse() const {
    C44Matrix result(
        a0, b0, c0, 0.0F,
        a1, b1, c1, 0.0F,
        a2, b2, c2, 0.0F,
        0.0F, 0.0F, 0.0F, 1.0F);
    result.Translate({-d0, -d1, -d2});
    return result;
}

C44Matrix C44Matrix::AffineInverse(float scale) const {
    if (std::fabs(scale - 1.0F) < 0.00000095367432F)
        return AffineInverse();
    C44Matrix result(
        a0, b0, c0, 0.0F,
        a1, b1, c1, 0.0F,
        a2, b2, c2, 0.0F,
        0.0F, 0.0F, 0.0F, 1.0F);
    result.Scale(1.0F / (scale * scale));
    result.Translate({-d0, -d1, -d2});
    return result;
}

C44Matrix C44Matrix::AffineInverse(const C3Vector& scale) const {
    const C3Vector reciprocal{1.0F / scale.x, 1.0F / scale.y, 1.0F / scale.z};
    C33Matrix rotationScale(a0, a1, a2,
                            b0, b1, b2,
                            c0, c1, c2);
    rotationScale.Scale(reciprocal);
    const C33Matrix transposed(rotationScale.a0, rotationScale.b0, rotationScale.c0,
                               rotationScale.a1, rotationScale.b1, rotationScale.c1,
                               rotationScale.a2, rotationScale.b2, rotationScale.c2);
    C44Matrix result(
        transposed.a0, transposed.a1, transposed.a2, 0.0F,
        transposed.b0, transposed.b1, transposed.b2, 0.0F,
        transposed.c0, transposed.c1, transposed.c2, 0.0F,
        0.0F, 0.0F, 0.0F, 1.0F);
    result.Scale(reciprocal);
    result.Translate({-d0, -d1, -d2});
    return result;
}

C44Matrix C44Matrix::Rotation(const C3Vector& axis, float angle, bool unit) {
    const C33Matrix rotation = C33Matrix::Rotation(axis, angle, unit);
    return {
        rotation.a0, rotation.a1, rotation.a2, 0.0F,
        rotation.b0, rotation.b1, rotation.b2, 0.0F,
        rotation.c0, rotation.c1, rotation.c2, 0.0F,
        0.0F, 0.0F, 0.0F, 1.0F,
    };
}

void C44Matrix::Translate(const C3Vector& move) {
    d0 += c0 * move.z + b0 * move.y + a0 * move.x;
    d1 += c1 * move.z + b1 * move.y + a1 * move.x;
    d2 += c2 * move.z + b2 * move.y + a2 * move.x;
}

void C44Matrix::Scale(const C3Vector& scale) {
    a0 *= scale.x; a1 *= scale.x; a2 *= scale.x;
    b0 *= scale.y; b1 *= scale.y; b2 *= scale.y;
    c0 *= scale.z; c1 *= scale.z; c2 *= scale.z;
}

void C44Matrix::Scale(float scale) {
    a0 *= scale; a1 *= scale; a2 *= scale;
    b0 *= scale; b1 *= scale; b2 *= scale;
    c0 *= scale; c1 *= scale; c2 *= scale;
}

void C44Matrix::Rotate(const C3Vector& axis, const C3Vector& angle, char unit) {
    *this = Rotation(axis, angle.x, unit != 0) * *this;
}

void C44Matrix::Rotate(const C4Quaternion& rotation) {
    C33Matrix matrix;
    matrix.Rotate(rotation);
    const C44Matrix rotationMatrix(
        matrix.a0, matrix.a1, matrix.a2, 0.0F,
        matrix.b0, matrix.b1, matrix.b2, 0.0F,
        matrix.c0, matrix.c1, matrix.c2, 0.0F,
        0.0F, 0.0F, 0.0F, 1.0F);
    *this = rotationMatrix * *this;
}

} // namespace NTempest

#include "ENGINE/Source/Gx/cgxdeviceopengl/cgxoglxform.h"

#include <cmath>

void CGxDeviceOpenGl::IXformSetProjection(const NTempest::C44Matrix& matrix)
{
    NTempest::C44Matrix glMatrix = matrix;
    glMatrix.c0 = -matrix.c0;
    glMatrix.c1 = -matrix.c1;
    glMatrix.c2 = -matrix.c2;
    glMatrix.c3 = -matrix.c3;

    // The recovered client evaluates glMatrix * shrink through Tempest's
    // expression-order multiplication.  In conventional OpenGL terms that
    // is shrink * glMatrix, so the first three rows are scaled while the
    // perspective row remains untouched.
    if ((CGxDevice::m_appState.m_masterEnables
         & (1U << GxMasterEnable_NormalProjection)) == 0
        && matrix.d3 != 1.0F) {
        glMatrix.a0 *= 0.2F;
        glMatrix.a1 *= 0.2F;
        glMatrix.a2 *= 0.2F;
        glMatrix.b0 *= 0.2F;
        glMatrix.b1 *= 0.2F;
        glMatrix.b2 *= 0.2F;
        glMatrix.c0 *= 0.2F;
        glMatrix.c1 *= 0.2F;
        glMatrix.c2 *= 0.2F;
        glMatrix.d0 *= 0.2F;
        glMatrix.d1 *= 0.2F;
        glMatrix.d2 *= 0.2F;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glMatrix.Data());
    GetError("Projection");
}

void CGxDeviceOpenGl::IXformGLModelView(
    const NTempest::C44Matrix& input,
    NTempest::C44Matrix& output) const
{
    output = input;
    output.a2 = -input.a2;
    output.b2 = -input.b2;
    output.c2 = -input.c2;
    output.d2 = -input.d2;
}

void CGxDeviceOpenGl::IXformSetModelView(const NTempest::C44Matrix& matrix)
{
    NTempest::C44Matrix glMatrix;
    IXformGLModelView(matrix, glMatrix);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glMatrix.Data());
    GetError("ModelView");
}

void CGxDeviceOpenGl::XformSetViewport(
    float minX,
    float maxX,
    float minY,
    float maxY,
    float minZ,
    float maxZ)
{
    CGxDevice::XformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);

    const NTempest::CRect* window = DeviceCurWindow();
    const GLint x = static_cast<GLint>(minX * window->r);
    const GLint y = static_cast<GLint>(minY * window->b);
    const GLint right = static_cast<GLint>(maxX * window->r);
    const GLint bottom = static_cast<GLint>(maxY * window->b);
    const GLsizei width = right - x;
    const GLsizei height = bottom - y;

    glViewport(x, y, width, height);
    glDepthRange(minZ, maxZ);
    glScissor(x, y, width, height);
    GetError("Viewport");
}

void CGxDeviceOpenGl::XformSetProjection(const NTempest::C44Matrix& matrix)
{
    CGxDevice::XformSetProjection(matrix);
    IXformSetProjection(matrix);
}

void CGxDeviceOpenGl::XformSetView(const NTempest::C44Matrix& matrix)
{
    CGxDevice::XformSetView(matrix);
    m_worldViewChange = true;
}

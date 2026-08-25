#include "ENGINE/Source/Gx/cgxdeviceopengl/glextsupport.h"

#include <cstdio>
#include <cstring>

unsigned glVersion = 0;
bool glExtCVA = false;
bool glExtBgra = false;
bool glExtClampToEdge = false;
bool glExtTextureLodBias = false;
bool glExtTextureCompression = false;
bool glExtTextureCompressionS3tc = false;
bool glSGISGenerateMipmap = false;
bool glExtTextureFilterAnisotropic = false;
bool glExtDrawRangeElements = false;
bool glSGISTextureLod = false;
bool glNVRegisterCombiners = false;
bool glNVRegisterCombiners2 = false;
bool glNVTextureShader = false;
bool glNVTextureShader2 = false;
bool glNVTextureShader3 = false;
bool glATIFragmentShader = false;
bool glARBFragmentProgram = false;
bool wglARBPixelFormat = false;
bool wglARBPbuffer = false;
bool wglEXTSwapControl = false;
int glExtMultiTextureCount = 1;

PFNGLLOCKARRAYSEXTPROC glLockArraysEXT = nullptr;
PFNGLUNLOCKARRAYSEXTPROC glUnlockArraysEXT = nullptr;
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB = nullptr;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB = nullptr;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB = nullptr;
PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2DARB = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC glCompressedTexSubImage2DARB = nullptr;
PFNGLDRAWRANGEELEMENTSEXTPROC glDrawRangeElementsEXT = nullptr;
PFNGLVERTEXARRAYRANGENVPROC glVertexArrayRangeNV = nullptr;
PFNGLFLUSHVERTEXARRAYRANGENVPROC glFlushVertexArrayRangeNV = nullptr;
PFNWGLALLOCATEMEMORYNVPROC wglAllocateMemoryNV = nullptr;
PFNWGLFREEMEMORYNVPROC wglFreeMemoryNV = nullptr;
PFNGLGENFENCESNVPROC glGenFencesNV = nullptr;
PFNGLDELETEFENCESNVPROC glDeleteFencesNV = nullptr;
PFNGLFINISHFENCENVPROC glFinishFenceNV = nullptr;
PFNGLSETFENCENVPROC glSetFenceNV = nullptr;
PFNGLTESTFENCENVPROC glTestFenceNV = nullptr;
PFNGLCOMBINERINPUTNVPROC glCombinerInputNV = nullptr;
PFNGLCOMBINEROUTPUTNVPROC glCombinerOutputNV = nullptr;
PFNGLFINALCOMBINERINPUTNVPROC glFinalCombinerInputNV = nullptr;
PFNGLCOMBINERPARAMETERFVNVPROC glCombinerParameterfvNV = nullptr;
PFNGLCOMBINERPARAMETERINVPROC glCombinerParameteriNV = nullptr;
PFNGLCOMBINERSTAGEPARAMETERFVNVPROC glCombinerStageParameterfvNV = nullptr;
PFNGLPROGRAMSTRINGARBPROC glProgramStringARB = nullptr;
PFNGLBINDPROGRAMARBPROC glBindProgramARB = nullptr;
PFNGLPROGRAMLOCALPARAMETER4FVARBPROC glProgramLocalParameter4fvARB = nullptr;
PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB = nullptr;
PFNGLGENPROGRAMSARBPROC glGenProgramsARB = nullptr;
PFNGLISPROGRAMARBPROC glIsProgramARB = nullptr;
PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = nullptr;
PFNWGLGETPIXELFORMATATTRIBIVARBPROC wglGetPixelFormatAttribivARB = nullptr;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC wglGetPixelFormatAttribfvARB = nullptr;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
PFNWGLCREATEPBUFFERARBPROC wglCreatePbufferARB = nullptr;
PFNWGLGETPBUFFERDCARBPROC wglGetPbufferDCARB = nullptr;
PFNWGLRELEASEPBUFFERDCARBPROC wglReleasePbufferDCARB = nullptr;
PFNWGLDESTROYPBUFFERARBPROC wglDestroyPbufferARB = nullptr;
PFNWGLQUERYPBUFFERARBPROC wglQueryPbufferARB = nullptr;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = nullptr;

namespace {

const char* s_glExts = nullptr;
const char* s_wglExts = nullptr;

template <typename T>
T LoadProc(const char* name)
{
    return reinterpret_cast<T>(wglGetProcAddress(name));
}

void ResetPointers()
{
    glLockArraysEXT = nullptr;
    glUnlockArraysEXT = nullptr;
    glActiveTextureARB = nullptr;
    glClientActiveTextureARB = nullptr;
    glMultiTexCoord2fARB = nullptr;
    glCompressedTexImage2DARB = nullptr;
    glCompressedTexSubImage2DARB = nullptr;
    glDrawRangeElementsEXT = nullptr;
    glVertexArrayRangeNV = nullptr;
    glFlushVertexArrayRangeNV = nullptr;
    wglAllocateMemoryNV = nullptr;
    wglFreeMemoryNV = nullptr;
    glGenFencesNV = nullptr;
    glDeleteFencesNV = nullptr;
    glFinishFenceNV = nullptr;
    glSetFenceNV = nullptr;
    glTestFenceNV = nullptr;
    glCombinerInputNV = nullptr;
    glCombinerOutputNV = nullptr;
    glFinalCombinerInputNV = nullptr;
    glCombinerParameterfvNV = nullptr;
    glCombinerParameteriNV = nullptr;
    glCombinerStageParameterfvNV = nullptr;
    glProgramStringARB = nullptr;
    glBindProgramARB = nullptr;
    glProgramLocalParameter4fvARB = nullptr;
    glDeleteProgramsARB = nullptr;
    glGenProgramsARB = nullptr;
    glIsProgramARB = nullptr;
    wglGetExtensionsStringARB = nullptr;
    wglGetPixelFormatAttribivARB = nullptr;
    wglGetPixelFormatAttribfvARB = nullptr;
    wglChoosePixelFormatARB = nullptr;
    wglCreatePbufferARB = nullptr;
    wglGetPbufferDCARB = nullptr;
    wglReleasePbufferDCARB = nullptr;
    wglDestroyPbufferARB = nullptr;
    wglQueryPbufferARB = nullptr;
    wglSwapIntervalEXT = nullptr;
}

} // namespace

bool ScanString(const char* extensions, const char* name)
{
    if (extensions == nullptr || name == nullptr || *name == '\0') {
        return false;
    }

    const std::size_t nameLength = std::strlen(name);
    const char* cursor = extensions;
    while (*cursor != '\0') {
        while (*cursor == ' ') {
            ++cursor;
        }
        const char* end = cursor;
        while (*end != '\0' && *end != ' ') {
            ++end;
        }
        if (static_cast<std::size_t>(end - cursor) == nameLength
            && std::strncmp(cursor, name, nameLength) == 0) {
            return true;
        }
        cursor = end;
    }
    return false;
}

bool FindGlExt(const char* name)
{
    if (s_glExts == nullptr) {
        s_glExts = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
    }
    return ScanString(s_glExts, name);
}

bool FindWglExt(const char* name)
{
    if (wglGetExtensionsStringARB == nullptr) {
        return false;
    }
    if (s_wglExts == nullptr) {
        s_wglExts = wglGetExtensionsStringARB(wglGetCurrentDC());
    }
    return ScanString(s_wglExts, name);
}

void BindGlExtensions()
{
    UnbindGlExtensions();

    const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    int major = -1;
    int minor = -1;
    if (version != nullptr) {
        sscanf_s(version, "%d.%d", &major, &minor);
    }
    if (major >= 0 && minor >= 0) {
        glVersion = (static_cast<unsigned>(major) << 16) | static_cast<unsigned>(minor);
    }

    glExtCVA = FindGlExt("GL_EXT_compiled_vertex_array");
    if (glExtCVA) {
        glLockArraysEXT = LoadProc<PFNGLLOCKARRAYSEXTPROC>("glLockArraysEXT");
        glUnlockArraysEXT = LoadProc<PFNGLUNLOCKARRAYSEXTPROC>("glUnlockArraysEXT");
    }

    if (FindGlExt("GL_ARB_multitexture")) {
        glGetIntegerv(0x84E2, &glExtMultiTextureCount);
        glActiveTextureARB = LoadProc<PFNGLACTIVETEXTUREARBPROC>("glActiveTextureARB");
        glClientActiveTextureARB = LoadProc<PFNGLCLIENTACTIVETEXTUREARBPROC>("glClientActiveTextureARB");
        glMultiTexCoord2fARB = LoadProc<PFNGLMULTITEXCOORD2FARBPROC>("glMultiTexCoord2fARB");
    } else {
        glExtMultiTextureCount = 1;
    }

    glExtBgra = FindGlExt("GL_EXT_bgra");
    glExtClampToEdge = FindGlExt("GL_SGIS_texture_edge_clamp")
        || FindGlExt("GL_EXT_texture_edge_clamp");
    glExtTextureLodBias = FindGlExt("GL_EXT_texture_lod_bias");
    glSGISGenerateMipmap = FindGlExt("GL_SGIS_generate_mipmap");
    glExtTextureFilterAnisotropic = FindGlExt("GL_EXT_texture_filter_anisotropic");
    glExtTextureCompressionS3tc = FindGlExt("GL_EXT_texture_compression_s3tc")
        || FindGlExt("GL_S3_s3tc");
    glExtTextureCompression = FindGlExt("GL_ARB_texture_compression") || glExtTextureCompressionS3tc;
    if (glExtTextureCompressionS3tc) {
        glCompressedTexImage2DARB = LoadProc<PFNGLCOMPRESSEDTEXIMAGE2DARBPROC>("glCompressedTexImage2DARB");
        glCompressedTexSubImage2DARB = LoadProc<PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC>("glCompressedTexSubImage2DARB");
        if (glCompressedTexImage2DARB == nullptr || glCompressedTexSubImage2DARB == nullptr) {
            glExtTextureCompressionS3tc = false;
        }
    }

    glExtDrawRangeElements = FindGlExt("GL_EXT_draw_range_elements");
    if (glExtDrawRangeElements) {
        glDrawRangeElementsEXT = LoadProc<PFNGLDRAWRANGEELEMENTSEXTPROC>("glDrawRangeElementsEXT");
    }

    if (FindGlExt("GL_NV_vertex_array_range")) {
        glVertexArrayRangeNV = LoadProc<PFNGLVERTEXARRAYRANGENVPROC>("glVertexArrayRangeNV");
        glFlushVertexArrayRangeNV = LoadProc<PFNGLFLUSHVERTEXARRAYRANGENVPROC>("glFlushVertexArrayRangeNV");
        wglAllocateMemoryNV = LoadProc<PFNWGLALLOCATEMEMORYNVPROC>("wglAllocateMemoryNV");
        wglFreeMemoryNV = LoadProc<PFNWGLFREEMEMORYNVPROC>("wglFreeMemoryNV");
    }
    if (FindGlExt("GL_NV_fence")) {
        glGenFencesNV = LoadProc<PFNGLGENFENCESNVPROC>("glGenFencesNV");
        glDeleteFencesNV = LoadProc<PFNGLDELETEFENCESNVPROC>("glDeleteFencesNV");
        glFinishFenceNV = LoadProc<PFNGLFINISHFENCENVPROC>("glFinishFenceNV");
        glSetFenceNV = LoadProc<PFNGLSETFENCENVPROC>("glSetFenceNV");
        glTestFenceNV = LoadProc<PFNGLTESTFENCENVPROC>("glTestFenceNV");
    }

    glNVRegisterCombiners = FindGlExt("GL_NV_register_combiners");
    if (glNVRegisterCombiners) {
        glCombinerInputNV = LoadProc<PFNGLCOMBINERINPUTNVPROC>("glCombinerInputNV");
        glCombinerOutputNV = LoadProc<PFNGLCOMBINEROUTPUTNVPROC>("glCombinerOutputNV");
        glFinalCombinerInputNV = LoadProc<PFNGLFINALCOMBINERINPUTNVPROC>("glFinalCombinerInputNV");
        glCombinerParameterfvNV = LoadProc<PFNGLCOMBINERPARAMETERFVNVPROC>("glCombinerParameterfvNV");
        glCombinerParameteriNV = LoadProc<PFNGLCOMBINERPARAMETERINVPROC>("glCombinerParameteriNV");
    }
    glNVRegisterCombiners2 = FindGlExt("GL_NV_register_combiners2");
    if (glNVRegisterCombiners2) {
        glCombinerStageParameterfvNV = LoadProc<PFNGLCOMBINERSTAGEPARAMETERFVNVPROC>("glCombinerStageParameterfvNV");
    }
    glNVTextureShader = FindGlExt("GL_NV_texture_shader");
    glNVTextureShader2 = FindGlExt("GL_NV_texture_shader2");
    glNVTextureShader3 = FindGlExt("GL_NV_texture_shader3");
    glATIFragmentShader = FindGlExt("GL_ATI_fragment_shader");

    glARBFragmentProgram = FindGlExt("GL_ARB_fragment_program");
    if (glARBFragmentProgram) {
        glProgramStringARB = LoadProc<PFNGLPROGRAMSTRINGARBPROC>("glProgramStringARB");
        glBindProgramARB = LoadProc<PFNGLBINDPROGRAMARBPROC>("glBindProgramARB");
        glProgramLocalParameter4fvARB = LoadProc<PFNGLPROGRAMLOCALPARAMETER4FVARBPROC>("glProgramLocalParameter4fvARB");
        glDeleteProgramsARB = LoadProc<PFNGLDELETEPROGRAMSARBPROC>("glDeleteProgramsARB");
        glGenProgramsARB = LoadProc<PFNGLGENPROGRAMSARBPROC>("glGenProgramsARB");
        glIsProgramARB = LoadProc<PFNGLISPROGRAMARBPROC>("glIsProgramARB");
    }

    wglGetExtensionsStringARB = LoadProc<PFNWGLGETEXTENSIONSSTRINGARBPROC>("wglGetExtensionsStringARB");
    if (FindWglExt("WGL_ARB_pixel_format")) {
        wglARBPixelFormat = true;
        wglGetPixelFormatAttribivARB = LoadProc<PFNWGLGETPIXELFORMATATTRIBIVARBPROC>("wglGetPixelFormatAttribivARB");
        wglGetPixelFormatAttribfvARB = LoadProc<PFNWGLGETPIXELFORMATATTRIBFVARBPROC>("wglGetPixelFormatAttribfvARB");
        wglChoosePixelFormatARB = LoadProc<PFNWGLCHOOSEPIXELFORMATARBPROC>("wglChoosePixelFormatARB");
    }
    if (FindWglExt("WGL_ARB_pbuffer")) {
        wglARBPbuffer = true;
        wglCreatePbufferARB = LoadProc<PFNWGLCREATEPBUFFERARBPROC>("wglCreatePbufferARB");
        wglGetPbufferDCARB = LoadProc<PFNWGLGETPBUFFERDCARBPROC>("wglGetPbufferDCARB");
        wglReleasePbufferDCARB = LoadProc<PFNWGLRELEASEPBUFFERDCARBPROC>("wglReleasePbufferDCARB");
        wglDestroyPbufferARB = LoadProc<PFNWGLDESTROYPBUFFERARBPROC>("wglDestroyPbufferARB");
        wglQueryPbufferARB = LoadProc<PFNWGLQUERYPBUFFERARBPROC>("wglQueryPbufferARB");
    }
    if (FindWglExt("WGL_EXT_swap_control")) {
        wglEXTSwapControl = true;
        wglSwapIntervalEXT = LoadProc<PFNWGLSWAPINTERVALEXTPROC>("wglSwapIntervalEXT");
    }
}

void UnbindGlExtensions()
{
    s_glExts = nullptr;
    s_wglExts = nullptr;
    glVersion = 0;
    glExtCVA = false;
    glExtBgra = false;
    glExtClampToEdge = false;
    glExtTextureLodBias = false;
    glExtTextureCompression = false;
    glExtTextureCompressionS3tc = false;
    glSGISGenerateMipmap = false;
    glExtTextureFilterAnisotropic = false;
    glExtDrawRangeElements = false;
    glSGISTextureLod = false;
    glNVRegisterCombiners = false;
    glNVRegisterCombiners2 = false;
    glNVTextureShader = false;
    glNVTextureShader2 = false;
    glNVTextureShader3 = false;
    glATIFragmentShader = false;
    glARBFragmentProgram = false;
    wglARBPixelFormat = false;
    wglARBPbuffer = false;
    wglEXTSwapControl = false;
    glExtMultiTextureCount = 1;
    ResetPointers();
}

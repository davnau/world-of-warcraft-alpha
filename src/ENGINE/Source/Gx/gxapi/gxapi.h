#pragma once

#include "ENGINE/Source/Gx/cgxdevice/cgxdevice.h"

#include <vector>

extern CGxDevice* g_theGxDevicePtr;

int GxAdapterID(unsigned short* vendorID, unsigned short* deviceID, unsigned* driverVersionHi, unsigned* driverVersionLow);
int GxAdapterInfer(unsigned short* deviceClass);
bool GxAdapterMonitorModes(std::vector<CGxMonitorMode>& modes);
int GxAdapterDesktopMode(unsigned mode[4]);
const std::vector<CGxFormat>* GxEnumFormats(EGxApi api);

CGxDevice* GxDevCreate(EGxApi api, void* windowProc, const CGxFormat& format);
CGxDevice* GxDevCreate(EGxApi api, HWND window, const CGxFormat& format);
void GxDevWM(EGxWM message, const NTempest::CRect& rect, int param);
void GxDevDestroy(CGxDevice* device);
bool GxDevSetFormat(const CGxFormat& format);
void GxDevSetBaseMipLevel(unsigned baseMipLevel);
void GxDevSetGamma(float gamma);
void GxDevSetGamma(const CGxGammaRamp& ramp);
void GxDevSetTextureQuality(int force32);
const CGxFormat* GxDevFormat();
unsigned GxDevBaseMipLevel();
void GxDevGammaRamp(CGxGammaRamp* ramp);
void GxDevSystemGammaRamp(CGxGammaRamp* ramp);
int GxDevTextureQuality();
HWND GxDevWindow();
EGxApi GxDevApi();
void GxDevTakeScreenShot();
void GxDevReadScreenShot(unsigned* width, unsigned* height, const NTempest::CImVector** pixels);
void GxDevClearScreenShot();
void GxDevReadPixels(const NTempest::CiRect& rect, std::vector<NTempest::CImVector>& pixels);
void GxDevReadDepth(const NTempest::CiRect& rect, std::vector<float>& depths);
void GxDevSetRenderTarget(EGxBuffer buffer, CGxTex* texture, unsigned plane);
void GxDevOverride(EGxOverride value, unsigned flags);

void GxLightSet(unsigned which, const CGxLight* light, const NTempest::C3Vector& cameraPosition);
void GxLight(unsigned which, CGxLight* light);
void GxLightEnable(unsigned which, bool enable);

void GxRsSet(EGxRenderState state, NTempest::CImVector value);
void GxRsSet(EGxRenderState state, float value);
void GxRsSet(EGxRenderState state, int value);
void GxRsSet(EGxRenderState state, EGxBlend value);
void GxRsSet(EGxRenderState state, CGxTex* texture);
void GxRsGet(EGxRenderState state, NTempest::CImVector* value);
void GxRsGet(EGxRenderState state, float* value);
void GxRsGet(EGxRenderState state, int* value);
void GxRsGet(EGxRenderState state, void** value);
void GxRsPush();
void GxRsPop();
void GxRsInit();
unsigned GxRsStackOffset();

void GxVertexShaderSelect(EGxVertexShader shader);
unsigned GxVertexSize(EGxVertexBufferFormat format) noexcept;
unsigned GxVertexMemberOffset(EGxVertexBufferFormat format, unsigned member) noexcept;

CGxBuf* GxBufCreate(EGxBufWriteFreq writeFreq, EGxVertexBufferFormat format, unsigned numVertices, unsigned numIndices, CGxBufUserCallback userCallback = nullptr, void* userArg = nullptr);
void GxBufLock(CGxBuf* buffer);
void GxBufUnlock();
void GxBufRender(const CGxBatch* batches, unsigned count);
void GxBufRender(const CGxBatch* batch);
void GxBufDestroy(CGxBuf** buffer);
void GxBufReserve(EGxBufWriteFreq writeFreq, EGxVertexBufferFormat format, unsigned numVertices, unsigned numIndices);
CGxBuf* GxBufGetDynamic(EGxVertexBufferFormat format);
unsigned GxPerfCounter(EGxPerfCounter counter);
void GxMasterEnableSet(EGxMasterEnables enable, bool value);
bool GxMasterEnable(EGxMasterEnables enable);
const CGxCaps* GxCaps();
void GxCapsWindowSize(NTempest::CRect* rect);
void GxCapsScreenSize(NTempest::CRect* rect);
int GxCapsIsWindowVisible();

void GxPrimLockVertexPtrs(
    unsigned vertexCount,
    const NTempest::C3Vector* positions,
    unsigned positionStride,
    const NTempest::C3Vector* normals,
    unsigned normalStride,
    const NTempest::CImVector* colors,
    unsigned colorStride,
    const std::uint8_t* bones,
    unsigned boneStride,
    const NTempest::C2Vector* tex0,
    unsigned tex0Stride,
    const NTempest::C2Vector* tex1,
    unsigned tex1Stride);
void GxPrimLockIndexPtr(EGxPrim primitive, unsigned count, const std::uint16_t* indices);
void GxPrimDrawElements();
void GxPrimDrawElements(EGxPrim primitive, unsigned count, const std::uint16_t* indices);
void GxPrimUnlockIndexPtr();
void GxPrimUnlockVertexPtrs();
void GxPrimBegin(EGxPrim primitive);
void GxPrimEnd();
void GxPrimVertex(const NTempest::C3Vector& vertex);
void GxPrimTexCoord(unsigned unit, const NTempest::C2Vector& texCoord);
void GxPrimNormal(const NTempest::C3Vector& normal);
void GxPrimColor(const NTempest::CImVector& color);
void GxPrimPointSize(float size);
void GxPrimLineWidth(float width);

void GxSceneSetClearColor(NTempest::CImVector color);
NTempest::CImVector* GxSceneClearColor(NTempest::CImVector* color);
void GxScenePresent(unsigned mask);
void GxSceneClear(unsigned mask);

int GxTexCreate(unsigned width, unsigned height, EGxTexFormat format, CGxTexFlags flags, void* userArg, CGxTexUserCallback userFunc, CGxTex** texture);
int GxTexCreate(EGxTexTarget target, unsigned width, unsigned height, unsigned depth, EGxTexFormat format, EGxTexFormat dataFormat, CGxTexFlags flags, void* userArg, CGxTexUserCallback userFunc, CGxTex** texture);
void GxTexUpdate(CGxTex* texture, const NTempest::CiRect* rect, bool immediate);
bool GxTexNeedsUpdate(const CGxTex* texture);
void GxTexSetUserData(CGxTex* texture, CGxTexUserCallback userFunc, void* userArg);
void GxTexSetFlags(CGxTex* texture, CGxTexFlags flags);
void GxTexDestroy(CGxTex* texture);
void GxTexParameters(const CGxTex* texture, CGxTexParms* parameters);
void GxTexParametersEx(const CGxTex* texture, CGxTexParmsEx* parameters);
void GxTexSetDataFormat(CGxTex* texture, EGxTexFormat format);
void GxTexFlags(const CGxTex* texture, CGxTexFlags* flags);
void GxTexGetDimensions(const CGxTex* texture, unsigned* width, unsigned* height);

void GxXformSetViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
void GxXformSetProjection(const NTempest::C44Matrix& matrix);
void GxXformSetView(const NTempest::C44Matrix& matrix);
void GxXformSetBones(unsigned numBones, const NTempest::C34Matrix* matrices);
void GxXformViewport(float* minX, float* maxX, float* minY, float* maxY, float* minZ, float* maxZ);
void GxXformProjection(NTempest::C44Matrix* matrix);
void GxXformView(NTempest::C44Matrix* matrix);
void GxXformBone(unsigned index, NTempest::C34Matrix* matrix);
void GxXformViewProj(NTempest::C44Matrix* matrix);
void GxXformPush(EGxXform transform);
void GxXformPush(EGxXform transform, const NTempest::C44Matrix& matrix);
void GxXformPop(EGxXform transform);
void GxXformIdentity(EGxXform transform);
void GxXformSet(EGxXform transform, const NTempest::C44Matrix& matrix);
void GxXformTranslate(EGxXform transform, const NTempest::C3Vector& translation);
void GxXformScale(EGxXform transform, const NTempest::C3Vector& scale);
void GxXformMult(EGxXform transform, const NTempest::C44Matrix& matrix);
void GxXform(EGxXform transform, NTempest::C44Matrix* matrix);

void GxPixelShaderCreate(CGxPixelShader** shader, const char* filename);
void GxPixelShaderDestroy(CGxPixelShader** shader);

std::uint8_t* GxAllocVertexMem(unsigned bytes);
void GxFreeVertexMem();
std::uint8_t* GxAllocIndexMem(unsigned bytes);
void GxFreeIndexMem();
std::uint8_t* GxAllocPixelMem(unsigned bytes);
void GxFreePixelMem();
EGxTexFormat GxGetGxTexFormat(int blitFormat);
BlitFormat GxGetBlitFormat(EGxTexFormat format);
void GxLogOpen(const char* filename = nullptr);
void GxLogClose();
void GxLog(const char* format, ...);

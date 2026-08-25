#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "ENGINE/Source/Tempest/crect.h"
#include "ENGINE/Source/Tempest/cimvector.h"
#include "ENGINE/Source/Tempest/cirect.h"
#include "ENGINE/Source/Tempest/c44matrix.h"
#include "ENGINE/Source/Gx/gxapi/gxbase.h"
#include "ENGINE/Source/Gx/gxapi/gxshader.h"
#include "ENGINE/Source/Gx/gxapi/gxtexture.h"
#include "ENGINE/Source/Gx/gxapi/gxbuffer.h"
#include "ENGINE/Source/Gx/gxapi/gxformat.h"
#include "ENGINE/Source/Gx/gxapi/gxmonitor.h"

#include <vector>

class CGxDeviceD3d;
class CGxDeviceD3d11;
class CGxDeviceOpenGl;

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>
#include <GL/gl.h>

class CGxDevice {
public:
    CGxDevice();
    virtual ~CGxDevice();

    void ClampRectToWindow(NTempest::CiRect* rect) const;
    virtual bool DeviceCreate(void* windowProc, const CGxFormat& format);
    virtual bool DeviceCreate(HWND clientWindow, const CGxFormat& format);
    virtual void DeviceDestroy();
    virtual bool DeviceSetFormat(CGxFormat& format);
    void DeviceSetBaseMipLevel(unsigned baseMipLevel);
    void DeviceSetGamma(float gamma);
    void DeviceSetGamma(const CGxGammaRamp& ramp);
    void DeviceSetTextureQuality(int force32);
    const CGxFormat* DeviceFormat() const noexcept { return &m_format; }
    unsigned DeviceBaseMipLevel() const noexcept { return m_baseMipLevel; }
    void DeviceGamma(CGxGammaRamp* ramp) const;
    void DeviceSystemGamma(CGxGammaRamp* ramp) const;
    int DeviceTextureQuality() const noexcept { return m_force32BitTextures ? 1 : 0; }
    EGxApi DeviceApi() const noexcept { return m_api; }
    virtual void DeviceTakeScreenShot();
    void DeviceScreenShot();
    virtual void DeviceReadPixels(const NTempest::CiRect& rect, std::vector<NTempest::CImVector>& pixels);
    virtual void DeviceReadDepths(const NTempest::CiRect& rect, std::vector<float>& depths);
    void DeviceReadScreenShot(unsigned* width, unsigned* height, const NTempest::CImVector** pixels) const;
    void DeviceClearScreenShot();
    virtual HWND DeviceWindow() const { return nullptr; }
    virtual void DeviceWM(EGxWM, const NTempest::CRect*, int) {}
    bool IDevIsWindowed() const noexcept { return m_windowed; }
    virtual void DeviceSetRenderTarget(EGxBuffer buffer, CGxTex* texture, unsigned plane);
    const NTempest::CRect* DeviceCurWindow() const { return &m_currentWindow; }
    void DeviceSetDefWindow(const NTempest::CRect* rect);
    const NTempest::CRect* DeviceDefWindow() const { return &m_defWindow; }
    void DeviceSetCurWindow(const NTempest::CRect* rect);
    virtual void DeviceOverride(EGxOverride value, unsigned flags);
    virtual void CapsWindowSize(NTempest::CRect* destination) const;
    virtual void CapsWindowSizeInScreenCoords(NTempest::CRect* destination) const;
    virtual int CapsIsWindowVisible() const;

    const CGxCaps* Caps() const noexcept { return &m_caps; }
    CGxCaps* Caps() noexcept { return &m_caps; }
    void SceneSetClearColor(NTempest::CImVector clearColor) noexcept { m_clearColor = clearColor; }
    NTempest::CImVector* SceneClearColor(NTempest::CImVector* destination) const;
    virtual void SceneClear(unsigned) {}
    virtual void ScenePresent(unsigned mask);
    virtual void SceneClearColorSet(NTempest::CImVector color) { SceneSetClearColor(color); }
    virtual void XformViewport(float* minX, float* maxX, float* minY, float* maxY, float* minZ, float* maxZ) const
    {
        *minX = m_viewportMin.x; *maxX = m_viewportMax.x;
        *minY = m_viewportMin.y; *maxY = m_viewportMax.y;
        *minZ = m_viewportMin.z; *maxZ = m_viewportMax.z;
    }
    virtual void XformSetViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
    virtual void XformSetProjection(const NTempest::C44Matrix& matrix);
    virtual void XformSetView(const NTempest::C44Matrix& matrix);
    virtual void XformSetBones(unsigned count, const NTempest::C34Matrix* matrices);
    void XformProjection(NTempest::C44Matrix* matrix) const;
    void XformView(NTempest::C44Matrix* matrix) const;
    void XformBone(unsigned index, NTempest::C34Matrix* matrix) const;
    void XformPush(EGxXform transform);
    void XformPush(EGxXform transform, const NTempest::C44Matrix& matrix);
    void XformPop(EGxXform transform);
    void XformIdentity(EGxXform transform);
    void XformSet(EGxXform transform, const NTempest::C44Matrix& matrix);
    void XformTranslate(EGxXform transform, const NTempest::C3Vector& translation);
    void XformScale(EGxXform transform, const NTempest::C3Vector& scale);
    void XformMult(EGxXform transform, const NTempest::C44Matrix& matrix);
    void Xform(EGxXform transform, NTempest::C44Matrix* matrix) const;
    void VertexShaderSelect(EGxVertexShader shader) noexcept { m_vertexShader = shader; }
    bool IVbHasColor(EGxVertexBufferFormat format) const;
    EGxVertexBufferFormat IGiveVbColor(EGxVertexBufferFormat format) const;
    unsigned PrimCalcCount(EGxPrim primitive, unsigned indexCount) const;
    virtual void PrimBegin(EGxPrim primitive);
    virtual void PrimEnd();
    virtual void PrimVertex(const NTempest::C3Vector& vertex);
    virtual void PrimNormal(const NTempest::C3Vector& normal);
    virtual void PrimColor(const NTempest::CImVector& color);
    virtual void PrimTexCoord(unsigned unit, const NTempest::C2Vector& texCoord);
    virtual void PrimPointSize(float size) { m_pointSize = size; }
    virtual void PrimLineWidth(float width) { m_lineWidth = width; }
    virtual void PrimLockAndProcessVertexPtrs(
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
    virtual void PrimLockIndexPtr(EGxPrim primitive, unsigned count, const std::uint16_t* indices);
    virtual void PrimDrawElements();
    virtual void PrimUnlockIndexPtr();
    virtual void PrimUnlockVertexPtrs();
    void LightSet(unsigned which, const CGxLight* light, const NTempest::C3Vector& cameraPosition);
    void Light(unsigned which, CGxLight* light) const;
    void LightEnable(unsigned which, bool enable);
    bool EnableState(unsigned application, unsigned applicationDisables, unsigned flagPosition) const;
    bool NeedsUpdate(unsigned application, unsigned applicationDisables, unsigned hardware, unsigned hardwareDisables, unsigned flagPosition) const;
    void MasterEnableSet(EGxMasterEnables state, bool enable);
    bool MasterEnable(EGxMasterEnables state) const;
    void RsSet(EGxRenderState state, NTempest::CImVector value);
    void RsSet(EGxRenderState state, float value);
    void RsSet(EGxRenderState state, int value);
    void RsSet(EGxRenderState state, EGxBlend value);
    void RsSet(EGxRenderState state, CGxTex* texture);
    void RsGet(EGxRenderState state, NTempest::CImVector* value) const;
    void RsGet(EGxRenderState state, float* value) const;
    void RsGet(EGxRenderState state, int* value) const;
    void RsGet(EGxRenderState state, void** value) const;
    void RsPush();
    void RsPop();
    void RsInit();
    unsigned RsStackOffset() const noexcept { return static_cast<unsigned>(m_rsStack.size()); }
    void IRsSync(bool force = false);
    void IRsForceUpdate(unsigned state);
    void IRsForceUpdate();
    virtual void IRsSendToHw(unsigned) {}
    CGxBuf* BufCreate(EGxBufWriteFreq writeFreq, EGxVertexBufferFormat format, unsigned numVertices, unsigned numIndices, CGxBufUserCallback callback = nullptr, void* userArg = nullptr);
    void BufDestroy(CGxBuf** buffer);
    void BufLock(CGxBuf* buffer);
    void BufUnlock();
    virtual void BufRender(const CGxBatch* batches, unsigned count);
    void BufReserve(EGxBufWriteFreq frequency, EGxVertexBufferFormat format, unsigned vertexCount, unsigned indexCount);
    CGxBuf* BufGetDynamic(EGxVertexBufferFormat format) const;
    bool TexCreate(unsigned width, unsigned height, EGxTexFormat format, CGxTexFlags flags, void* userArg, CGxTexUserCallback callback, CGxTex** texture);
    bool TexCreate(EGxTexTarget target, unsigned width, unsigned height, unsigned depth, EGxTexFormat format, EGxTexFormat dataFormat, CGxTexFlags flags, void* userArg, CGxTexUserCallback callback, CGxTex** texture);
    void TexMarkForUpdate(CGxTex* texture, const NTempest::CiRect& updateRect, bool immediate);
    bool TexNeedsUpdate(const CGxTex* texture) const;
    virtual void ITexMarkAsUpdated(CGxTex* texture);
    unsigned ITexComputeByteSize(const CGxTex* texture, unsigned width, unsigned height) const;
    virtual void ITexBind(CGxTex* texture);
    void TexSetUserData(CGxTex* texture, CGxTexUserCallback callback, void* userArg);
    void TexSetFlags(CGxTex* texture, CGxTexFlags flags);
    void* TexUserArg(const CGxTex* texture) const;
    void TexGetDimensions(const CGxTex* texture, unsigned* width, unsigned* height) const;
    virtual void TexDestroy(CGxTex** texture);
    void TexSetDataFormat(CGxTex* texture, EGxTexFormat format);
    CGxTexParms TexParameters(const CGxTex* texture) const;
    CGxTexParmsEx TexParametersEx(const CGxTex* texture) const;
    CGxTexFlags TexFlags(const CGxTex* texture) const;
    void ISetShaderParameters(CGxShader* shader, bool force);
    unsigned PerfCounter(EGxPerfCounter counter) const;
    void PerfCountersLatch();
    static double CpuFrequency();
    static std::uint64_t CpuTicks();
    static void DbgPrintf(const char* format, ...);
    static void LogOpen(const char* filename = nullptr);
    static void LogClose();
    static void Log(const char* format, ...);

    static CGxDevice* NewD3d();
    static CGxDeviceD3d* NewD3d9();
    static CGxDeviceOpenGl* NewOpenGl();
    static int AdapterID(
        unsigned short* vendorID,
        unsigned short* deviceID,
        unsigned* driverVersionHi,
        unsigned* driverVersionLow);
    static int AdapterInfer(unsigned short* deviceClass);
    static bool AdapterMonitorModes(std::vector<CGxMonitorMode>& modes);
    static int AdapterDesktopMode(unsigned mode[4]);
    static bool D3dEnumFormats(std::vector<CGxFormat>& formats);
    static bool OpenGlEnumFormats(std::vector<CGxFormat>& formats);

    NTempest::CRect m_currentWindow{};
    NTempest::CRect m_defWindow{};
    bool m_windowed = true;
    CGxFormat m_format{};
    unsigned m_baseMipLevel = 0;
    CGxGammaRamp m_gammaRamp{};
    CGxGammaRamp m_systemGammaRamp{};
    CGxCaps m_caps{};
    EGxApi m_api = GxApi_OpenGl;
    bool m_force32BitTextures = false;
    NTempest::CImVector m_clearColor{};
    std::vector<NTempest::CImVector> m_scrShotPixels;
    unsigned m_scrShotWidth = 0;
    unsigned m_scrShotHeight = 0;
    int m_scrShotClick = 0;
    NTempest::C3Vector m_viewportMin{};
    NTempest::C3Vector m_viewportMax{1.0F, 1.0F, 1.0F};
    NTempest::C44Matrix m_projection{};
    std::array<CGxMatrixStack, GxXforms_Last> m_xforms{};
    const NTempest::C34Matrix* m_bones = nullptr;
    unsigned m_boneCount = 0;
    EGxVertexShader m_vertexShader = GxVS_PassThru;
    EGxPrim m_primType = GxPrim_Triangles;
    bool m_inBeginEnd = false;
    NTempest::C3Vector m_primVertex{};
    NTempest::C3Vector m_primNormal{0.0F, 0.0F, 1.0F};
    NTempest::CImVector m_primColor{0xFFFFFFFFU};
    NTempest::C2Vector m_primTexCoord[4]{};
    float m_pointSize = 1.0F;
    float m_lineWidth = 1.0F;
    CGxStateRegister m_appState{};
    CGxStateRegister m_hwState{};
    std::vector<std::array<std::uint32_t, GxRenderStates_Last>> m_rsStack;
    std::vector<std::array<CGxTex*, 4>> m_rsTextureStack;
    std::array<bool, GxRenderStates_Last> m_rsDirty{};
    std::vector<CGxBuf*> m_ownedBuffers;
    std::vector<CGxBuf*> m_dynamicBuffers;
    CGxBuf* m_bufLocked = nullptr;
    EGxVertexBufferFormat m_vertexBufferFormat = GxVBF_PN;
    std::vector<CGxTex*> m_textures;
    std::vector<CGxPixelShader*> m_pixelShaders;
    std::vector<CGxVertexShader*> m_vertexShaders;
    std::array<unsigned, 9 * GxVertexBufferFormats_Last> m_VBReserve{};
    std::array<unsigned, 9 * GxVertexBufferFormats_Last> m_IBReserve{};
    std::array<unsigned, GxPerfCounters_Last> m_perfCountersAcc{};
    std::array<unsigned, GxPerfCounters_Last> m_perfCountersLatched{};
    std::vector<NTempest::C3Vector> m_immediateVertices;
    std::vector<std::uint16_t> m_immediateIndices;
    std::array<std::vector<NTempest::C2Vector>, 4> m_immediateTexCoords;
    std::vector<NTempest::C3Vector> m_immediateNormals;
    std::vector<NTempest::CImVector> m_immediateColors;
    unsigned m_primMask = 0;
    unsigned m_primIndexCount = 0;
    bool m_vertexLocked = false;
    bool m_indexLocked = false;
};

class CGxDeviceD3d : public CGxDevice {
public:
    struct StateD3dLight {
        int which = -1;
        bool enabled = false;
        D3DLIGHT9 value{};
    };
    void ISceneBegin(unsigned mask);
    void ISceneEnd();
    void SceneClear(unsigned mask) override;
    void ScenePresent(unsigned mask) override;
    void XformSetViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) override;
    void XformSetProjection(const NTempest::C44Matrix& matrix) override;
    void XformSetView(const NTempest::C44Matrix& matrix) override;
    void IXformSetWorld();
    void IXformSetTex(unsigned textureUnit, unsigned transformMode);
    void IStateSync();
    void IStateSyncLights();
    void IStateSyncEnables();
    void IStateSyncMaterial();
    void IStateSyncTransforms();
    void IStateSetD3DDefaults();
    void ISetLight(unsigned which, const D3DLIGHT9& value, bool enabled);
    void IForceLights();
    void DsSet(unsigned state, unsigned value);
    void ISetTexture(unsigned textureUnit, CGxTex* texture);
    void ISetTexGen(unsigned textureUnit, unsigned texGen);
    void ISetTexLodBias(unsigned textureUnit, float bias);
    void ISetTexBlend(unsigned textureUnit, unsigned blend);
    void IRsSendToHw(unsigned renderState) override;
    void BufReserve(unsigned frequency, unsigned format, unsigned vertexCount, unsigned indexCount);
    CGxBufD3d* BufCreate(unsigned vertexBytes, unsigned indexBytes, unsigned vertexStride);
    bool BufLock(CGxBufD3d* buffer, bool indexBuffer, void** data, unsigned bytes, unsigned offset = 0);
    void BufUnlock(CGxBufD3d* buffer, bool indexBuffer);
    void BufRender(CGxBufD3d* buffer, unsigned primitiveCount);
    void BufDestroy(CGxBufD3d* buffer);
    void IPrimSetupPos(const float* positions, unsigned count, unsigned stride);
    void PrimLockAndProcessVertexPtrs(const void* vertices, unsigned count, unsigned stride);
    void IPrimProcessVertexPtrs(const void* vertices, unsigned count, unsigned stride);
    void IPrimProcessIndexPtrs(const unsigned short* indices, unsigned count);
    void PrimLockIndexPtr(EGxPrim primitive, unsigned count, const unsigned short* indices) override;
    void PrimDrawElements() override;
    void PrimUnlockIndexPtr() override;
    void PrimUnlockVertexPtrs() override;
    void ITexForceRecreation(bool freeTextures);
    bool TexCreate(unsigned width, unsigned height, CGxTex::Format format, unsigned flags, CGxTex** texture);
    void TexDestroy(CGxTex* texture);
    void ITexCreate(unsigned width, unsigned height, CGxTex::Format format, unsigned flags, CGxTex* texture);
    void ITexUpload(CGxTex* texture, unsigned width, unsigned height, unsigned startLevel, unsigned endLevel);
    void ITexMarkAsUpdated(CGxTex* texture);
    void IShaderForceRecreation(bool freeShaders);
    void IPixelShaderCreate(CGxPixelShader* shader);
    void PixelShaderCreate(CGxPixelShader** shader, const char* filename);
    void PixelShaderDestroy(CGxPixelShader** shader);
    void ISetShaderParamList(const std::vector<CGxShaderParam*>& params, bool forceForBind);
    void IBindPixelShader(CGxPixelShader* shader);
    void IBindVertexShader(CGxVertexShader* shader);

    void CapsWindowSize(NTempest::CRect* destination) const override;
    void CapsWindowSizeInScreenCoords(NTempest::CRect* destination) const override;
    int CapsIsWindowVisible() const override;

    HWND m_hwnd = nullptr;
    bool m_windowVisible = false;
    bool m_d3dNeedsReset = false;
    IDirect3DDevice9* m_d3dDevice = nullptr;
    bool m_inScene = false;
    unsigned m_clearColor = 0;
    unsigned m_masterEnables = 0;
    bool m_worldViewChange = false;
    NTempest::C44Matrix m_worldMatrix{};
    bool m_worldMatrixIdentity = true;
    bool m_worldMatrixDirty = true;
    NTempest::C44Matrix m_textureMatrices[4]{};
    NTempest::C44Matrix m_textureGenerationMatrices[4]{};
    bool m_textureGenerationEnabled[4]{};
    std::vector<CGxPixelShader*> m_pixelShaders;
    std::vector<CGxVertexShader*> m_vertexShaders;
    unsigned m_deviceState[43]{};
    bool m_texEnable[4]{};
    unsigned m_numTmus = 4;
    StateD3dLight m_d3dStatesLight[8]{};
    std::vector<CGxTex*> m_textures;
    bool m_force32BitTextures = false;
    std::vector<CGxBufD3d*> m_buffers;
    std::vector<float> m_primVertices;
    std::vector<std::uint16_t> m_primIndices;
    unsigned m_primVertexStride = 0;
};

class CGxDeviceOpenGl : public CGxDevice {
public:
    enum EDeviceState : unsigned {
        Ds_DepthMask = 0,
        Ds_ActiveTexture = 1,
        Ds_TexTarget0 = 2,
        Ds_TexTarget1 = 3,
        Ds_TexTarget2 = 4,
        Ds_TexTarget3 = 5,
        Ds_TexGenS0 = 6,
        Ds_TexGenS1 = 7,
        Ds_TexGenS2 = 8,
        Ds_TexGenS3 = 9,
        Ds_TexGenT0 = 10,
        Ds_TexGenT1 = 11,
        Ds_TexGenT2 = 12,
        Ds_TexGenT3 = 13,
        Ds_TexGenR0 = 14,
        Ds_TexGenR1 = 15,
        Ds_TexGenR2 = 16,
        Ds_TexGenR3 = 17,
        Ds_TexGenQ0 = 18,
        Ds_TexGenQ1 = 19,
        Ds_TexGenQ2 = 20,
        Ds_TexGenQ3 = 21,
        Ds_TexEnvMode0 = 22,
        Ds_TexEnvMode1 = 23,
        Ds_TexEnvMode2 = 24,
        Ds_TexEnvMode3 = 25,
        Ds_NormalArray = 26,
        Ds_ColorArray = 27,
        Ds_TextureArray0 = 28,
        Ds_TextureArray1 = 29,
        Ds_TextureArray2 = 30,
        Ds_TextureArray3 = 31,
        Ds_NVVAR = 32,
        Ds_PolygonOffsetEnable = 33,
        Ds_PolygonOffset = 34,
        Ds_BlendEnable = 35,
        Ds_AlphaTestEnable = 36,
        Ds_RegisterCombinersNV = 37,
        Ds_PerStageConstantsNV = 38,
        Ds_TextureShaderNV = 39,
        Ds_FragmentProgramARB = 40,
        Ds_MatrixMode = 41,
        Ds_BlendFunc = 42,
        DeviceStates_Last = 43,
    };
    enum WindowMessage : unsigned {
        GxWM_Size = 0,
        GxWM_DisplayChange = 1,
        GxWM_Destroy = 2,
        GxWM_SetFocus = 3,
        GxWM_KillFocus = 4,
    };
    using WindowProc = LRESULT (CALLBACK*)(HWND, UINT, WPARAM, LPARAM);
    enum ColorSource : unsigned {
        Cs_Material = 0,
        Cs_Constant = 1,
        Cs_Array = 2,
    };
    struct ColorSourceColor {
        ColorSourceColor() : color(), dirty(false) {}
        NTempest::CImVector color{};
        bool dirty = false;
    };
    void ISceneBegin(unsigned mask);
    void SceneClear(unsigned mask) override;
    void ScenePresent(unsigned mask) override;
    void IXformSetProjection(const NTempest::C44Matrix& matrix);
    void IXformGLModelView(const NTempest::C44Matrix& input, NTempest::C44Matrix& output) const;
    void IXformSetModelView(const NTempest::C44Matrix& matrix);
    void XformSetViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) override;
    void XformSetProjection(const NTempest::C44Matrix& matrix) override;
    void XformSetView(const NTempest::C44Matrix& matrix) override;
    void IPixelShaderBind(CGxPixelShader* shader);
    void PixelShaderCreate(CGxPixelShader** shader, const char* filename);
    void PixelShaderDestroy(CGxPixelShader** shader);
    void ISetShaderParamList(const std::vector<CGxShaderParam*>& params, bool forceForBind);
    void IShaderForceRecreation();
    void CapsWindowSize(NTempest::CRect* destination) const override;
    int CapsIsWindowVisible() const override;
    void DsInit();
    void DsSet(unsigned state, unsigned value, int force = 0);
    void IStateSync();
    void IStateSetColorSource(ColorSource source);
    void IStateSetColorSourceColor(ColorSource source, const NTempest::CImVector& color);
    void IStateSyncColorSource();
    void IStateSyncLights();
    void IStateSyncEnables();
    void IStateSyncTexTransforms();
    void IStateSyncTexTransform(unsigned textureUnit);
    void IStateSetContextDefaults();
    void ISetTexture(unsigned textureUnit, CGxTex* texture);
    void ISetTexBlend(unsigned textureUnit, unsigned blend);
    void ISetTexLodBias(unsigned textureUnit, float bias);
    void ISetTexGen(unsigned textureUnit, unsigned texGen);
    void IRsSendToHw(unsigned renderState) override;
    void BufReserve(unsigned frequency, unsigned format, unsigned vertexCount, unsigned indexCount);
    void GetError(const char* phase = nullptr) const;
    void DeviceReadPixels(const NTempest::CiRect& rect, std::vector<NTempest::CImVector>& pixels) override;
    void DeviceReadDepths(const NTempest::CiRect& rect, std::vector<float>& depths) override;
    void DeviceOverride(EGxOverride value, unsigned flags) override;
    void LockArrays(unsigned count);
    void UnlockArrays();
    void IAllocVAR();
    void IFreeVAR();
    void IAllocVertexBufferVAR(unsigned frequency, unsigned bytes);
    void AllocVertexBuffer(unsigned frequency, unsigned bytes);
    void AllocIndexBuffer(unsigned frequency, unsigned bytes);
    void IAllocBuffers();
    void AllocBuffers();
    void FreeVertexBuffer(unsigned frequency);
    void FreeIndexBuffer(unsigned frequency);
    void FreeBuffers();
    void ISetGlCaps();
    void BindTexture(CGxTex* texture, unsigned textureUnit);
    bool TexCreate(unsigned width, unsigned height, CGxTex::Format format, unsigned flags, CGxTex** texture);
    void TexDestroy(CGxTex* texture);
    void ITexSetFlags(CGxTex* texture);
    void ITexDownload(CGxTex* texture, int width, int height, unsigned level, unsigned stride, const unsigned char* texels);
    void ITexMarkAsUpdated(CGxTex* texture, unsigned textureUnit = 0);
    void ITexForceRecreation();
    CGxBufOgl* BufCreate(unsigned vertexBytes, unsigned indexBytes, unsigned vertexStride);
    bool BufLock(CGxBufOgl* buffer, bool indexBuffer, void** data, unsigned bytes, unsigned offset = 0);
    void BufUnlock(CGxBufOgl* buffer, bool indexBuffer);
    void BufRender(CGxBufOgl* buffer, unsigned primitiveCount);
    void BufDestroy(CGxBufOgl* buffer);
    void IPrimSetupNormal(unsigned stride, const void* normals);
    void IPrimSetupColor(unsigned stride, const NTempest::CImVector* colors, unsigned count, bool convert);
    void IPrimSetupTexCoord(unsigned textureUnit, unsigned stride, const void* coordinates);
    void PrimLockAndProcessVertexPtrs(
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
        unsigned tex1Stride) override;
    void PrimLockIndexPtr(EGxPrim primitive, unsigned count, const unsigned short* indices) override;
    void PrimDrawElements() override;
    void PrimUnlockIndexPtr() override;
    void PrimUnlockVertexPtrs() override;
    void PrimPointSize(float size);
    void PrimLineWidth(float width);
    void DeviceCreatePbuffer();
    void DeviceQueryPbuffer();
    void DeviceDestroyPbuffer();
    void IDevSetFocus(int focus, const CGxFormat& format);
    bool SetFormatMode(CGxFormat& format);
    bool IDevAttachGlContext(const CGxFormat& format);
    void IDevRemoveGlContext();
    static LRESULT CALLBACK WindowProcGl(HWND window, UINT message, WPARAM wParam, LPARAM lParam);
    void DeviceWM(WindowMessage message, const NTempest::CRect* rect, int param);
    bool DeviceCreate(WindowProc windowProc, const CGxFormat& format);
    bool DeviceCreate(HWND clientWindow, const CGxFormat& format);
    void DeviceDestroy();
    bool DeviceSetFormat(CGxFormat& format);
    void DeviceSetBaseMipLevel(unsigned baseMipLevel);
    void DeviceSetGamma(float gamma);
    void DeviceSetRenderTarget(unsigned buffer, CGxTex* texture, unsigned plane);
    void DeviceSetTextureQuality(int force32);
    HWND DeviceWindow() const;
    void CapsWindowSizeInScreenCoords(NTempest::CRect* destination) const override;

    unsigned m_deviceState[DeviceStates_Last]{};
    NTempest::CImVector m_clearColor{};
    unsigned m_masterEnables = 0;
    HDC m_hdc = nullptr;
    HWND m_hwnd = nullptr;
    HGLRC m_hglrc = nullptr;
    void* m_hPbuffer = nullptr;
    HDC m_hPbufferDC = nullptr;
    HGLRC m_hPbufferRC = nullptr;
    ATOM m_hwndClass = 0;
    bool m_context = false;
    bool m_ownhwnd = false;
    CGxFormat m_format{};
    unsigned m_numTmus = 1;
    unsigned m_maxTextureSize = 512;
    bool m_worldViewChange = false;
    int m_pixelShaderTarget = CGxPixelShader::Target_None;
    std::vector<CGxPixelShader*> m_pixelShaders;
    std::vector<CGxTex*> m_textures;
    ColorSource m_colorSource = Cs_Material;
    bool m_colorSourceDirty = false;
    ColorSourceColor m_colorSourceColor[3]{};
    NTempest::C44Matrix m_textureMatrices[4]{};
    std::array<CGxMatrixStack, 4> m_texGen{};
    std::vector<CGxBufOgl*> m_buffers;
    std::vector<float> m_primVertices;
    std::vector<NTempest::C3Vector> m_primNormals;
    std::vector<NTempest::CImVector> m_primColors;
    std::array<std::vector<NTempest::C2Vector>, 2> m_primTexCoords;
    std::vector<std::uint16_t> m_primIndices;
    const std::uint16_t* m_primIndicesPtr = nullptr;
    unsigned m_primVertexStride = 0;
    unsigned m_lockedArrays = 0;
    std::vector<std::uint8_t> m_nvvarMem;
    CGxBufOgl* m_vertexBuffers[4]{};
    CGxBufOgl* m_indexBuffers[4]{};
};

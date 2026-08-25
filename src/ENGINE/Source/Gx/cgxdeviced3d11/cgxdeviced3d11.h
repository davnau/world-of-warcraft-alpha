#pragma once

#include "ENGINE/Source/Gx/cgxdevice/cgxdevice.h"

#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>

#include <array>
#include <cstdint>
#include <unordered_map>
#include <vector>

class CGxDeviceD3d11 final : public CGxDevice {
public:
    using WindowProc = LRESULT (CALLBACK*)(HWND, UINT, WPARAM, LPARAM);

    CGxDeviceD3d11();
    ~CGxDeviceD3d11() override;

    bool DeviceCreate(void* windowProc, const CGxFormat& format) override;
    bool DeviceCreate(HWND clientWindow, const CGxFormat& format) override;
    void DeviceDestroy() override;
    bool DeviceSetFormat(CGxFormat& format) override;
    void DeviceWM(EGxWM message, const NTempest::CRect* rect, int param) override;
    HWND DeviceWindow() const override;
    void DeviceReadPixels(const NTempest::CiRect& rect,
        std::vector<NTempest::CImVector>& pixels) override;
    void DeviceReadDepths(const NTempest::CiRect& rect,
        std::vector<float>& depths) override;
    void DeviceSetRenderTarget(EGxBuffer buffer, CGxTex* texture, unsigned plane) override;
    void DeviceOverride(EGxOverride value, unsigned flags) override;

    void CapsWindowSize(NTempest::CRect* destination) const override;
    void CapsWindowSizeInScreenCoords(NTempest::CRect* destination) const override;
    int CapsIsWindowVisible() const override;

    void SceneClear(unsigned mask) override;
    void ScenePresent(unsigned mask) override;

    void XformSetViewport(float minX, float maxX, float minY, float maxY,
        float minZ, float maxZ) override;
    void XformSetProjection(const NTempest::C44Matrix& matrix) override;
    void XformSetView(const NTempest::C44Matrix& matrix) override;
    void XformSetBones(unsigned count, const NTempest::C34Matrix* matrices) override;

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
    void PrimLockIndexPtr(EGxPrim primitive, unsigned count,
        const std::uint16_t* indices) override;
    void PrimDrawElements() override;
    void PrimUnlockIndexPtr() override;
    void PrimUnlockVertexPtrs() override;
    void PrimPointSize(float size) override;
    void PrimLineWidth(float width) override;
    void BufRender(const CGxBatch* batches, unsigned count) override;

    void IRsSendToHw(unsigned renderState) override;

    void ITexMarkAsUpdated(CGxTex* texture) override;
    void ITexBind(CGxTex* texture) override;
    void TexDestroy(CGxTex** texture) override;

private:
    struct Vertex {
        float position[3]{};
        float normal[3]{0.0F, 1.0F, 0.0F};
        float color[4]{1.0F, 1.0F, 1.0F, 1.0F};
        float tex0[2]{};
        float tex1[2]{};
    };

    struct LightConstants {
        float vector[4]{};
        float diffuse[4]{};
        float ambient[4]{};
        float attenuation[4]{};
    };

    struct alignas(16) ShaderConstants {
        float world[16]{};
        float view[16]{};
        float projection[16]{};
        float textureMatrix[4][16]{};
        float materialDiffuse[4]{};
        float materialEmissive[4]{};
        float sceneAmbient[4]{};
        float fogColor[4]{};
        float fogParams[4]{};
        float stageBlend[4]{};
        float stageTexGen[4]{};
        float stageTexShader[4]{};
        float textureEnabled[4]{};
        float renderFlags[4]{};
        LightConstants lights[8]{};
    };

    struct TextureResource {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
        unsigned width = 0;
        unsigned height = 0;
        bool renderTarget = false;
    };

    bool CreateOwnedWindow(const CGxFormat& format);
    bool CreateDeviceResources(const CGxFormat& format);
    bool CreateSizeResources(unsigned width, unsigned height);
    bool CreateShaders();
    void ReleaseSizeResources();
    void UpdateViewport();
    void ApplyPipeline();
    void UpdateShaderConstants();
    void BindTextures();
    bool EnsureTexture(CGxTex* texture);
    bool UploadTexture(CGxTex* texture, TextureResource& resource);
    bool UploadVertices(const std::vector<Vertex>& vertices);
    bool UploadIndices(const std::vector<std::uint16_t>& indices);
    void Draw(const std::vector<Vertex>& vertices,
        const std::vector<std::uint16_t>& indices, EGxPrim primitive);
    std::vector<std::uint16_t> ConvertIndices(EGxPrim primitive,
        unsigned count, const std::uint16_t* indices) const;
    D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology(EGxPrim primitive) const;
    NTempest::C44Matrix D3dProjection(const NTempest::C44Matrix& source) const;
    static LRESULT CALLBACK WindowProcD3d11(HWND window, UINT message,
        WPARAM wParam, LPARAM lParam);

    WindowProc m_clientWindowProc = nullptr;
    HWND m_hwnd = nullptr;
    ATOM m_windowClass = 0;
    bool m_ownWindow = false;
    bool m_windowVisible = false;
    bool m_resizing = false;

    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_backBufferRtv;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthTexture;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthView;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader11;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader11;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_dynamicVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_dynamicIndexBuffer;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendState;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterState;
    std::array<Microsoft::WRL::ComPtr<ID3D11SamplerState>, 4> m_samplerStates;

    std::unordered_map<CGxTex*, TextureResource> m_textureResources;
    std::vector<Vertex> m_vertices;
    std::vector<std::uint16_t> m_indices;
    D3D11_PRIMITIVE_TOPOLOGY m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    unsigned m_vertexCapacity = 0;
    unsigned m_indexCapacity = 0;
    bool m_pipelineDirty = true;
    bool m_shaderOverride = false;
    CGxTex* m_renderTargetTexture = nullptr;
};

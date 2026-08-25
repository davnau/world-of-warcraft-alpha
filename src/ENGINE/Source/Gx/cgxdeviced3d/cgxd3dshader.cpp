#include "ENGINE/Source/Gx/cgxdeviced3d/cgxd3dshader.h"

#include <algorithm>

void CGxDeviceD3d::IShaderForceRecreation(bool freeShaders)
{
    if (!freeShaders) {
        return;
    }
    for (CGxPixelShader* shader : m_pixelShaders) {
        if (shader != nullptr && shader->apiSpecific != nullptr) {
            shader->apiSpecific->Release();
            shader->apiSpecific = nullptr;
            shader->valid = false;
        }
    }
    for (CGxVertexShader* shader : m_vertexShaders) {
        if (shader != nullptr && shader->apiSpecific != nullptr) {
            shader->apiSpecific->Release();
            shader->apiSpecific = nullptr;
        }
    }
}

void CGxDeviceD3d::IPixelShaderCreate(CGxPixelShader* shader)
{
    if (m_d3dDevice == nullptr || shader == nullptr || shader->code.empty()) {
        return;
    }

    if (shader->apiSpecific != nullptr) {
        shader->apiSpecific->Release();
        shader->apiSpecific = nullptr;
    }

    const HRESULT result = m_d3dDevice->CreatePixelShader(
        reinterpret_cast<const DWORD*>(shader->code.data()),
        &shader->apiSpecific);
    shader->valid = SUCCEEDED(result) && shader->apiSpecific != nullptr;
    if (!shader->valid) {
        shader->apiSpecific = nullptr;
    }
}

void CGxDeviceD3d::PixelShaderCreate(CGxPixelShader** shader, const char* filename)
{
    (void)filename;
    if (shader == nullptr) {
        return;
    }
    if (*shader == nullptr) {
        *shader = new CGxPixelShader();
        m_pixelShaders.push_back(*shader);
    }
    IPixelShaderCreate(*shader);
}

void CGxDeviceD3d::PixelShaderDestroy(CGxPixelShader** shader)
{
    if (shader == nullptr || *shader == nullptr) {
        return;
    }
    CGxPixelShader* value = *shader;
    if (value->apiSpecific != nullptr) {
        value->apiSpecific->Release();
        value->apiSpecific = nullptr;
    }
    value->valid = false;
    if (value->refCount > 1) {
        --value->refCount;
        return;
    }
    const auto it = std::find(m_pixelShaders.begin(), m_pixelShaders.end(), value);
    if (it != m_pixelShaders.end()) {
        m_pixelShaders.erase(it);
    }
    delete value;
    *shader = nullptr;
}

void CGxDeviceD3d::ISetShaderParamList(
    const std::vector<CGxShaderParam*>& params,
    bool forceForBind)
{
    if (m_d3dDevice == nullptr) {
        return;
    }
    for (CGxShaderParam* parameter : params) {
        if (parameter == nullptr || (!parameter->dirty && !forceForBind)) {
            continue;
        }
        m_d3dDevice->SetPixelShaderConstantF(
            parameter->index,
            parameter->values,
            parameter->vectorCount);
        parameter->dirty = false;
    }
}

void CGxDeviceD3d::IBindPixelShader(CGxPixelShader* shader)
{
    if (m_d3dDevice == nullptr) {
        return;
    }
    if (shader != nullptr) {
        if (shader->apiSpecific == nullptr) {
            IPixelShaderCreate(shader);
        }
        m_d3dDevice->SetPixelShader(shader->apiSpecific);
    } else {
        m_d3dDevice->SetPixelShader(nullptr);
    }
}

void CGxDeviceD3d::IBindVertexShader(CGxVertexShader* shader)
{
    if (m_d3dDevice != nullptr) {
        m_d3dDevice->SetVertexShader(shader != nullptr ? shader->apiSpecific : nullptr);
    }
}

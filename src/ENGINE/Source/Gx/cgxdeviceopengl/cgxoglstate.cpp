#include "ENGINE/Source/Gx/cgxdeviceopengl/cgxoglstate.h"

#include "ENGINE/Source/Gx/cgxdeviceopengl/glextsupport.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>

namespace {

constexpr unsigned kDepthMask = 0;
constexpr unsigned kActiveTexture = 1;
constexpr unsigned kTextureTarget0 = 2;
constexpr unsigned kTexGenS0 = 6;
constexpr unsigned kTexGenT0 = 10;
constexpr unsigned kTexGenR0 = 14;
constexpr unsigned kTexGenQ0 = 18;
constexpr unsigned kTexEnvMode0 = 22;
constexpr unsigned kNormalArray = 26;
constexpr unsigned kColorArray = 27;
constexpr unsigned kTextureArray0 = 28;
constexpr unsigned kNvVar = 32;
constexpr unsigned kPolygonOffsetEnable = 33;
constexpr unsigned kPolygonOffset = 34;
constexpr unsigned kBlendEnable = 35;
constexpr unsigned kAlphaTestEnable = 36;
constexpr unsigned kRegisterCombiners = 37;
constexpr unsigned kPerStageConstants = 38;
constexpr unsigned kTextureShader = 39;
constexpr unsigned kFragmentProgram = 40;
constexpr unsigned kMatrixMode = 41;
constexpr unsigned kBlendFunc = 42;
constexpr GLenum kTexture0 = 0x84C0;
constexpr GLenum kTextureFilterControl = 0x8500;
constexpr GLenum kTextureLodBias = 0x8501;

const NTempest::C44Matrix& StackTop(const CGxMatrixStack& stack)
{
    return *stack.Top();
}

} // namespace

void CGxDeviceOpenGl::DsInit()
{
    for (unsigned& value : m_deviceState) {
        value = 0;
    }
    m_deviceState[kDepthMask] = 1;
}

void CGxDeviceOpenGl::DsSet(unsigned state, unsigned value, int force)
{
    if (state >= DeviceStates_Last || (force == 0 && m_deviceState[state] == value)) {
        return;
    }

    const unsigned previous = m_deviceState[state];
    switch (state) {
    case kDepthMask:
        glDepthMask(value != 0 ? GL_TRUE : GL_FALSE);
        break;
    case kActiveTexture:
        if (glActiveTextureARB != nullptr && glClientActiveTextureARB != nullptr) {
            const GLenum texture = kTexture0 + value;
            glActiveTextureARB(texture);
            glClientActiveTextureARB(texture);
        }
        break;
    case kTextureTarget0:
    case kTextureTarget0 + 1:
    case kTextureTarget0 + 2:
    case kTextureTarget0 + 3:
        if (previous != 0) {
            glDisable(previous);
        }
        if (value != 0) {
            glEnable(value);
        }
        break;
    case kTexGenS0:
    case kTexGenS0 + 1:
    case kTexGenS0 + 2:
    case kTexGenS0 + 3:
        if (value != 0) {
            glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, value);
            glEnable(GL_TEXTURE_GEN_S);
        } else {
            glDisable(GL_TEXTURE_GEN_S);
        }
        break;
    case kTexGenT0:
    case kTexGenT0 + 1:
    case kTexGenT0 + 2:
    case kTexGenT0 + 3:
        if (value != 0) {
            glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, value);
            glEnable(GL_TEXTURE_GEN_T);
        } else {
            glDisable(GL_TEXTURE_GEN_T);
        }
        break;
    case kTexGenR0:
    case kTexGenR0 + 1:
    case kTexGenR0 + 2:
    case kTexGenR0 + 3:
        if (value != 0) {
            glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, value);
            glEnable(GL_TEXTURE_GEN_R);
        } else {
            glDisable(GL_TEXTURE_GEN_R);
        }
        break;
    case kTexGenQ0:
    case kTexGenQ0 + 1:
    case kTexGenQ0 + 2:
    case kTexGenQ0 + 3:
        if (value != 0) {
            glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, value);
            glEnable(GL_TEXTURE_GEN_Q);
        } else {
            glDisable(GL_TEXTURE_GEN_Q);
        }
        break;
    case kTexEnvMode0:
    case kTexEnvMode0 + 1:
    case kTexEnvMode0 + 2:
    case kTexEnvMode0 + 3:
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, value);
        break;
    case kNormalArray:
        value ? glEnableClientState(GL_NORMAL_ARRAY) : glDisableClientState(GL_NORMAL_ARRAY);
        break;
    case kColorArray:
        value ? glEnableClientState(GL_COLOR_ARRAY) : glDisableClientState(GL_COLOR_ARRAY);
        break;
    case kTextureArray0:
    case kTextureArray0 + 1:
    case kTextureArray0 + 2:
    case kTextureArray0 + 3:
        value ? glEnableClientState(GL_TEXTURE_COORD_ARRAY) : glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        break;
    case kNvVar:
        if (glVertexArrayRangeNV != nullptr)
            value ? glEnableClientState(0x8533) : glDisableClientState(0x8533);
        break;
    case kPolygonOffsetEnable:
        value != 0 ? glEnable(GL_POLYGON_OFFSET_FILL) : glDisable(GL_POLYGON_OFFSET_FILL);
        break;
    case kPolygonOffset: {
        float factor = 0.0F;
        std::memcpy(&factor, &value, sizeof(factor));
        glPolygonOffset(factor, -1.0F);
        break;
    }
    case kBlendEnable:
        value != 0 ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
        break;
    case kAlphaTestEnable:
        value != 0 ? glEnable(GL_ALPHA_TEST) : glDisable(GL_ALPHA_TEST);
        break;
    case kRegisterCombiners:
        if (glNVRegisterCombiners) {
            value != 0 ? glEnable(0x8522) : glDisable(0x8522);
        }
        break;
    case kPerStageConstants:
        if (glNVRegisterCombiners2) {
            value != 0 ? glEnable(0x8551) : glDisable(0x8551);
        }
        break;
    case kTextureShader:
        if (glNVTextureShader) {
            value != 0 ? glEnable(0x86A0) : glDisable(0x86A0);
        }
        break;
    case kFragmentProgram:
        if (glARBFragmentProgram) {
            value != 0 ? glEnable(0x8804) : glDisable(0x8804);
        }
        break;
    case kMatrixMode:
        glMatrixMode(value);
        break;
    case kBlendFunc:
        glBlendFunc(static_cast<GLenum>(value >> 16), static_cast<GLenum>(value & 0xffff));
        break;
    default:
        break;
    }
    m_deviceState[state] = value;
}

void CGxDeviceOpenGl::IStateSync()
{
    IStateSyncLights();
    GetError("StateSync-lights");
    IStateSyncEnables();
    GetError("StateSync-enables");
    CGxDevice::IRsSync(false);
    GetError("StateSync-renderstates");
    IStateSyncTexTransforms();
    GetError("StateSync-texforms");
    IStateSyncColorSource();
    GetError("StateSync-color");
}

void CGxDeviceOpenGl::IStateSetColorSource(ColorSource source)
{
    if (m_colorSource != source) {
        m_colorSource = source;
        m_colorSourceDirty = true;
    }
}

void CGxDeviceOpenGl::IStateSetColorSourceColor(
    ColorSource source,
    const NTempest::CImVector& color)
{
    if (source == Cs_Array) {
        return;
    }
    m_colorSourceColor[source].color = color;
    m_colorSourceColor[source].dirty = true;
}

void CGxDeviceOpenGl::IStateSyncColorSource()
{
    if (!m_colorSourceDirty && !m_colorSourceColor[m_colorSource].dirty) {
        return;
    }
    if (m_colorSource <= Cs_Constant) {
        const NTempest::CImVector& color = m_colorSourceColor[m_colorSource].color;
        const GLfloat color4f[] = {
            color.r / 255.0F, color.g / 255.0F,
            color.b / 255.0F, color.a / 255.0F};
        glColor4fv(color4f);
    }
    m_colorSourceDirty = false;
    m_colorSourceColor[m_colorSource].dirty = false;
}

void CGxDeviceOpenGl::IStateSyncLights()
{
    const bool transformChanged = m_worldViewChange
        || m_xforms[GxXform_World].m_dirty
        || m_xforms[GxXform_View].m_dirty;
    m_worldViewChange = transformChanged;
    bool viewLoadedForLights = false;

    for (unsigned index = 0; index < CGxDevice::m_appState.m_lights.size(); ++index) {
        const GLenum glIndex = GL_LIGHT0 + index;
        const auto& light = CGxDevice::m_appState.m_lights[index];
        const auto& hwLight = CGxDevice::m_hwState.m_lights[index];
        const bool lightDirty = CGxDevice::m_hwState.m_lightsDirty[index] != 0;

        const auto scaledColor = [](const NTempest::CImVector& color, float intensity) {
            return std::array<GLfloat, 4>{
                color.r * intensity / 255.0F, color.g * intensity / 255.0F,
                color.b * intensity / 255.0F, color.a * intensity / 255.0F};
        };

        const bool positionChanged = transformChanged || lightDirty
            || ((light.m_flags ^ hwLight.m_flags) & 2U) != 0
            || light.m_dir.x != hwLight.m_dir.x
            || light.m_dir.y != hwLight.m_dir.y
            || light.m_dir.z != hwLight.m_dir.z;
        if ((light.m_flags & 1U) != 0 && positionChanged) {
            if (!viewLoadedForLights) {
                IXformSetModelView(StackTop(m_xforms[GxXform_View]));
                viewLoadedForLights = true;
            }
            std::array<GLfloat, 4> position = (light.m_flags & 2U) != 0U
                ? std::array<GLfloat, 4>{light.m_dir.x, light.m_dir.y, light.m_dir.z, 1.0F}
                : std::array<GLfloat, 4>{-light.m_dir.x, -light.m_dir.y, -light.m_dir.z, 0.0F};
            if ((light.m_flags & 2U) == 0U) {
                const float length = std::sqrt(position[0] * position[0]
                    + position[1] * position[1] + position[2] * position[2]);
                if (std::fabs(length - 1.0F) >= 2.3841858e-7F && length > 0.0F) {
                    position[0] /= length;
                    position[1] /= length;
                    position[2] /= length;
                }
            }
            glLightfv(glIndex, GL_POSITION, position.data());
        }

        if (lightDirty) {
            if (((light.m_flags ^ hwLight.m_flags) & 1U) != 0)
                (light.m_flags & 1U) ? glEnable(glIndex) : glDisable(glIndex);
            if (light.m_ambColor.b != hwLight.m_ambColor.b
                || light.m_ambColor.g != hwLight.m_ambColor.g
                || light.m_ambColor.r != hwLight.m_ambColor.r
                || light.m_ambColor.a != hwLight.m_ambColor.a
                || light.m_ambIntensity != hwLight.m_ambIntensity) {
                const auto ambient = scaledColor(light.m_ambColor, light.m_ambIntensity);
                glLightfv(glIndex, GL_AMBIENT, ambient.data());
            }
            if (light.m_dirColor.b != hwLight.m_dirColor.b
                || light.m_dirColor.g != hwLight.m_dirColor.g
                || light.m_dirColor.r != hwLight.m_dirColor.r
                || light.m_dirColor.a != hwLight.m_dirColor.a
                || light.m_dirIntensity != hwLight.m_dirIntensity) {
                const auto diffuse = scaledColor(light.m_dirColor, light.m_dirIntensity);
                glLightfv(glIndex, GL_DIFFUSE, diffuse.data());
            }
            if (light.m_specColor.b != hwLight.m_specColor.b
                || light.m_specColor.g != hwLight.m_specColor.g
                || light.m_specColor.r != hwLight.m_specColor.r
                || light.m_specColor.a != hwLight.m_specColor.a
                || light.m_specIntensity != hwLight.m_specIntensity) {
                const auto specular = scaledColor(light.m_specColor, light.m_specIntensity);
                glLightfv(glIndex, GL_SPECULAR, specular.data());
            }
            if (light.m_constantAttenuation != hwLight.m_constantAttenuation)
                glLightf(glIndex, GL_CONSTANT_ATTENUATION, light.m_constantAttenuation);
            if (light.m_linearAttenuation != hwLight.m_linearAttenuation)
                glLightf(glIndex, GL_LINEAR_ATTENUATION, light.m_linearAttenuation);
            if (light.m_quadraticAttenuation != hwLight.m_quadraticAttenuation)
                glLightf(glIndex, GL_QUADRATIC_ATTENUATION, light.m_quadraticAttenuation);
            CGxDevice::m_hwState.m_lights[index] = light;
            CGxDevice::m_hwState.m_lightsDirty[index] = 0U;
        }
    }

    if (transformChanged || viewLoadedForLights) {
        const NTempest::C44Matrix worldView =
            StackTop(m_xforms[GxXform_World]) * StackTop(m_xforms[GxXform_View]);
        IXformSetModelView(worldView);
        m_worldViewChange = false;
        m_xforms[GxXform_World].m_dirty = false;
        m_xforms[GxXform_View].m_dirty = false;
    }
}

void CGxDeviceOpenGl::IStateSyncEnables()
{
    if (CGxDevice::m_appState.m_masterEnables == CGxDevice::m_hwState.m_masterEnables)
        return;
    const bool oldFill = (CGxDevice::m_hwState.m_masterEnables
        & (1U << GxMasterEnable_PolygonFill)) != 0;
    const bool newFill = (CGxDevice::m_appState.m_masterEnables
        & (1U << GxMasterEnable_PolygonFill)) != 0;
    if (oldFill != newFill)
        glPolygonMode(GL_FRONT_AND_BACK, newFill ? GL_FILL : GL_LINE);
    CGxDevice::m_hwState.m_masterEnables = CGxDevice::m_appState.m_masterEnables;
}

void CGxDeviceOpenGl::IStateSyncTexTransforms()
{
    for (unsigned textureUnit = 0; textureUnit < m_caps.m_numTmus; ++textureUnit) {
        if (CGxDevice::m_appState.m_textures[textureUnit] != nullptr
            && (m_xforms[GxXform_Tex0 + textureUnit].m_dirty
                || m_texGen[textureUnit].m_dirty))
            IStateSyncTexTransform(textureUnit);
    }
}

void CGxDeviceOpenGl::IStateSyncTexTransform(unsigned textureUnit)
{
    if (textureUnit >= m_caps.m_numTmus) return;
    DsSet(kActiveTexture, textureUnit);
    DsSet(kMatrixMode, GL_TEXTURE);
    const unsigned textureShader =
        CGxDevice::m_appState.m_values[GxRs_TextureShader0 + textureUnit];
    if (textureShader == GxTS_PassThru) {
        glLoadIdentity();
    } else if (textureShader == GxTS_Affine || textureShader == GxTS_Proj) {
        const NTempest::C44Matrix concat =
            StackTop(m_texGen[textureUnit])
            * StackTop(m_xforms[GxXform_Tex0 + textureUnit]);
        glLoadMatrixf(concat.Data());
    }
    m_texGen[textureUnit].m_dirty = false;
    m_xforms[GxXform_Tex0 + textureUnit].m_dirty = false;
    DsSet(kMatrixMode, GL_MODELVIEW);
}

void CGxDeviceOpenGl::IStateSetContextDefaults()
{
    DsInit();
    DsSet(kMatrixMode, GL_MODELVIEW, 1);
    glLoadIdentity();

    static constexpr GLfloat rPlane[4] = {0.0F, 0.0F, 1.0F, 0.0F};
    static constexpr GLfloat qPlane[4] = {0.0F, 0.0F, 0.0F, 1.0F};
    const unsigned textureUnitCount = m_caps.m_numTmus < 4U ? m_caps.m_numTmus : 4U;
    for (unsigned textureUnit = 0;
         textureUnit < textureUnitCount;
         ++textureUnit) {
        DsSet(kActiveTexture, textureUnit, 1);
        glTexGenfv(GL_R, GL_EYE_PLANE, rPlane);
        glTexGenfv(GL_Q, GL_EYE_PLANE, qPlane);
        glTexGenfv(GL_R, GL_OBJECT_PLANE, rPlane);
        glTexGenfv(GL_Q, GL_OBJECT_PLANE, qPlane);
    }

    static constexpr GLfloat opaqueBlack[4] = {0.0F, 0.0F, 0.0F, 0.0F};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, opaqueBlack);
    glLightModeli(0x81F8, 0x81FA);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    IXformSetModelView(StackTop(m_xforms[GxXform_View]));
    for (unsigned index = 0; index < 8; ++index) {
        const GLenum glIndex = GL_LIGHT0 + index;
        const CGxLight& light = m_hwState.m_lights[index];
        glLightfv(glIndex, GL_SPECULAR, opaqueBlack);

        GLfloat position[4]{};
        if ((light.m_flags & 2U) != 0) {
            position[0] = light.m_dir.x;
            position[1] = light.m_dir.y;
            position[2] = light.m_dir.z;
            position[3] = 1.0F;
        } else {
            NTempest::C3Vector direction{-light.m_dir.x, -light.m_dir.y, -light.m_dir.z};
            const float magnitude = std::sqrt(direction.SquaredMag());
            if (magnitude > 0.0F) direction /= magnitude;
            position[0] = direction.x;
            position[1] = direction.y;
            position[2] = direction.z;
            position[3] = 0.0F;
        }
        glLightfv(glIndex, GL_POSITION, position);
        (light.m_flags & 1U) != 0 ? glEnable(glIndex) : glDisable(glIndex);

        const float ambientScale = light.m_ambIntensity / 255.0F;
        const GLfloat ambient[4] = {
            light.m_ambColor.r * ambientScale,
            light.m_ambColor.g * ambientScale,
            light.m_ambColor.b * ambientScale,
            light.m_ambColor.a * ambientScale};
        glLightfv(glIndex, GL_AMBIENT, ambient);

        const float diffuseScale = light.m_dirIntensity / 255.0F;
        const GLfloat diffuse[4] = {
            light.m_dirColor.r * diffuseScale,
            light.m_dirColor.g * diffuseScale,
            light.m_dirColor.b * diffuseScale,
            light.m_dirColor.a * diffuseScale};
        glLightfv(glIndex, GL_DIFFUSE, diffuse);
        m_hwState.m_lightsDirty[index] = 1U;
        glLightf(glIndex, GL_LINEAR_ATTENUATION, m_hwState.m_lightLinearFalloff);
        glLightf(glIndex, GL_QUADRATIC_ATTENUATION, m_hwState.m_lightQuadraticFalloff);
    }

    const NTempest::C44Matrix worldView =
        StackTop(m_xforms[GxXform_World]) * StackTop(m_xforms[GxXform_View]);
    IXformSetModelView(worldView);
    m_worldViewChange = false;
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_SCISSOR_TEST);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void CGxDeviceOpenGl::ISetTexture(unsigned textureUnit, CGxTex* texture)
{
    if (textureUnit >= m_caps.m_numTmus) return;
    DsSet(kActiveTexture, textureUnit);
    if (texture != nullptr) {
        DsSet(kTextureTarget0 + textureUnit, GL_TEXTURE_2D);
        BindTexture(texture, textureUnit);
        ITexMarkAsUpdated(texture, textureUnit);
        CGxDevice::ITexBind(texture);
    } else {
        DsSet(kTextureTarget0 + textureUnit, 0);
    }
}

void CGxDeviceOpenGl::ISetTexBlend(unsigned textureUnit, unsigned blend)
{
    if (textureUnit >= m_caps.m_numTmus || blend >= GxTexBlends_Last) return;
    DsSet(kActiveTexture, textureUnit);
    static constexpr GLenum textureEnvironment[GxTexBlends_Last] = {
        GL_REPLACE, GL_MODULATE, GL_DECAL, GL_ADD, static_cast<GLenum>(~0U)};
    const GLenum mode = textureEnvironment[blend];
    DsSet(kTexEnvMode0 + textureUnit, mode);
}

void CGxDeviceOpenGl::ISetTexLodBias(unsigned textureUnit, float bias)
{
    if (textureUnit >= m_caps.m_numTmus || !glExtTextureLodBias) {
        return;
    }
    DsSet(kActiveTexture, textureUnit);
    glTexEnvf(kTextureFilterControl, kTextureLodBias, bias);
}

void CGxDeviceOpenGl::ISetTexGen(unsigned textureUnit, unsigned texGen)
{
    if (textureUnit >= m_caps.m_numTmus || texGen >= GxTexGens_Last) return;
    DsSet(kActiveTexture, textureUnit);
    if (texGen == GxTexGen_Disable) {
        DsSet(kTexGenS0 + textureUnit, 0);
        DsSet(kTexGenT0 + textureUnit, 0);
        DsSet(kTexGenR0 + textureUnit, 0);
        DsSet(kTexGenQ0 + textureUnit, 0);
    } else if (texGen == GxTexGen_Object) {
        DsSet(kTexGenS0 + textureUnit, GL_OBJECT_LINEAR);
        DsSet(kTexGenT0 + textureUnit, GL_OBJECT_LINEAR);
        DsSet(kTexGenR0 + textureUnit, GL_OBJECT_LINEAR);
        DsSet(kTexGenQ0 + textureUnit, 0);
    } else if (texGen == GxTexGen_World || texGen == GxTexGen_View) {
        DsSet(kTexGenS0 + textureUnit, GL_EYE_LINEAR);
        DsSet(kTexGenT0 + textureUnit, GL_EYE_LINEAR);
        DsSet(kTexGenR0 + textureUnit, GL_EYE_LINEAR);
        DsSet(kTexGenQ0 + textureUnit, 0);
    } else if (texGen == GxTexGen_ViewReflection) {
        DsSet(kTexGenS0 + textureUnit, 0x8512);
        DsSet(kTexGenT0 + textureUnit, 0x8512);
        DsSet(kTexGenR0 + textureUnit, 0x8512);
        DsSet(kTexGenQ0 + textureUnit, 0);
    } else if (texGen == GxTexGen_ViewNormal) {
        DsSet(kTexGenS0 + textureUnit, 0x8511);
        DsSet(kTexGenT0 + textureUnit, 0x8511);
        DsSet(kTexGenR0 + textureUnit, 0x8511);
        DsSet(kTexGenQ0 + textureUnit, 0);
    } else if (texGen == GxTexGen_SphereMap) {
        DsSet(kTexGenS0 + textureUnit, GL_SPHERE_MAP);
        DsSet(kTexGenT0 + textureUnit, GL_SPHERE_MAP);
        DsSet(kTexGenR0 + textureUnit, 0);
        DsSet(kTexGenQ0 + textureUnit, 0);
    }

    if (texGen == GxTexGen_World) {
        NTempest::C44Matrix& matrix = *m_texGen[textureUnit].Top();
        IXformGLModelView(StackTop(m_xforms[GxXform_View]), matrix);
        std::swap(matrix.a1, matrix.b0);
        std::swap(matrix.a2, matrix.c0);
        std::swap(matrix.b2, matrix.c1);
        matrix.d0 = -matrix.d0;
        matrix.d1 = -matrix.d1;
        matrix.d2 = -matrix.d2;
        m_texGen[textureUnit].m_dirty = true;
    } else {
        m_texGen[textureUnit].Identity();
    }
}

void CGxDeviceOpenGl::IRsSendToHw(unsigned renderState)
{
    if (renderState >= GxRenderStates_Last) return;
    const auto& values = CGxDevice::m_appState.m_values;
    const unsigned bits = values[renderState];
    const auto asFloat = [](unsigned value) {
        float result = 0.0F;
        std::memcpy(&result, &value, sizeof(result));
        return result;
    };
    const auto asColor = [](unsigned value) {
        const NTempest::CImVector color(value);
        return std::array<GLfloat, 4>{
            color.r / 255.0F, color.g / 255.0F,
            color.b / 255.0F, color.a / 255.0F};
    };

    static constexpr GLenum sourceBlend[GxBlends_Last] = {
        GL_ONE, GL_ONE, GL_SRC_ALPHA, GL_SRC_ALPHA,
        GL_DST_COLOR, GL_DST_COLOR, GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA};
    static constexpr GLenum destinationBlend[GxBlends_Last] = {
        GL_ZERO, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA, GL_ONE,
        GL_ZERO, GL_SRC_COLOR, GL_ONE, GL_ONE};
    static constexpr GLenum fogStyle[GxFogStyles_Last] = {
        GL_LINEAR, GL_EXP, GL_EXP2};
    static constexpr GLenum compare[GxCmps_Last] = {
        GL_LEQUAL, GL_EQUAL, GL_GEQUAL};

    switch (renderState) {
    case GxRs_PolygonOffset: {
        const float factor = asFloat(bits) * -16.0F;
        if (factor == 0.0F) {
            DsSet(kPolygonOffsetEnable, 0);
        } else {
            unsigned factorBits = 0;
            std::memcpy(&factorBits, &factor, sizeof(factor));
            DsSet(kPolygonOffset, factorBits);
            DsSet(kPolygonOffsetEnable, 1);
        }
        break;
    }
    case GxRs_MatDiffuse:
        IStateSetColorSource(Cs_Material);
        IStateSetColorSourceColor(Cs_Material, NTempest::CImVector(bits));
        break;
    case GxRs_MatEmissive: {
        const auto color = asColor(bits);
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, color.data());
        break;
    }
    case GxRs_MatSpecular: {
        const auto color = asColor(bits);
        glMaterialfv(GL_FRONT, GL_SPECULAR, color.data());
        break;
    }
    case GxRs_MatSpecularExp:
        glMaterialf(GL_FRONT, GL_SHININESS, asFloat(bits));
        break;
    case GxRs_NormalizeNormals:
        bits ? glEnable(GL_NORMALIZE) : glDisable(GL_NORMALIZE);
        break;
    case GxRs_SceneAmbient: {
        const auto color = asColor(bits);
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, color.data());
        break;
    }
    case GxRs_Blend:
        if (bits == GxBlend_Opaque || bits == GxBlend_AlphaKey) {
            DsSet(kBlendEnable, 0);
        } else if (bits < GxBlends_Last) {
            DsSet(kBlendEnable, 1);
            DsSet(kBlendFunc, (sourceBlend[bits] << 16) | destinationBlend[bits]);
        }
        break;
    case GxRs_AlphaRef:
        if (static_cast<int>(bits) <= 0) {
            DsSet(kAlphaTestEnable, 0);
        } else {
            glAlphaFunc(GL_GREATER, bits / 255.0F);
            DsSet(kAlphaTestEnable, 1);
        }
        break;
    case GxRs_FogStyle:
        if (bits < GxFogStyles_Last) glFogi(GL_FOG_MODE, fogStyle[bits]);
        break;
    case GxRs_FogStart: glFogf(GL_FOG_START, asFloat(bits)); break;
    case GxRs_FogEnd: glFogf(GL_FOG_END, asFloat(bits)); break;
    case GxRs_FogDensity: glFogf(GL_FOG_DENSITY, asFloat(bits)); break;
    case GxRs_FogColor: {
        const auto color = asColor(bits);
        glFogfv(GL_FOG_COLOR, color.data());
        break;
    }
    case GxRs_Lighting:
        ((CGxDevice::m_appState.m_masterEnables & (1U << GxMasterEnable_Lighting)) && bits)
            ? glEnable(GL_LIGHTING) : glDisable(GL_LIGHTING);
        break;
    case GxRs_Fog:
        ((CGxDevice::m_appState.m_masterEnables & (1U << GxMasterEnable_Fog)) && bits)
            ? glEnable(GL_FOG) : glDisable(GL_FOG);
        break;
    case GxRs_DepthTest:
        ((CGxDevice::m_appState.m_masterEnables & (1U << GxMasterEnable_DepthTest)) && bits)
            ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
        break;
    case GxRs_DepthFunc:
        if (bits < GxCmps_Last) glDepthFunc(compare[bits]);
        break;
    case GxRs_DepthWrite:
        DsSet(kDepthMask,
            (CGxDevice::m_appState.m_masterEnables & (1U << GxMasterEnable_DepthWrite))
                ? bits : 0);
        break;
    case GxRs_Culling:
        ((CGxDevice::m_appState.m_masterEnables & (1U << GxMasterEnable_Culling)) && bits)
            ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
        break;
    case GxRs_Texture0:
    case GxRs_Texture1:
    case GxRs_Texture2:
    case GxRs_Texture3:
        ISetTexture(renderState - GxRs_Texture0,
            CGxDevice::m_appState.m_textures[renderState - GxRs_Texture0]);
        break;
    case GxRs_TexBlend0:
    case GxRs_TexBlend1:
    case GxRs_TexBlend2:
    case GxRs_TexBlend3:
        ISetTexBlend(renderState - GxRs_TexBlend0, bits);
        break;
    case GxRs_TexLodBias0:
    case GxRs_TexLodBias1:
    case GxRs_TexLodBias2:
    case GxRs_TexLodBias3:
        ISetTexLodBias(renderState - GxRs_TexLodBias0, asFloat(bits));
        break;
    case GxRs_TexGen0:
    case GxRs_TexGen1:
    case GxRs_TexGen2:
    case GxRs_TexGen3:
        ISetTexGen(renderState - GxRs_TexGen0, bits);
        break;
    case GxRs_PixelShader:
        IPixelShaderBind(reinterpret_cast<CGxPixelShader*>(static_cast<std::uintptr_t>(bits)));
        break;
    case GxRs_TextureShader0:
    case GxRs_TextureShader1:
    case GxRs_TextureShader2:
    case GxRs_TextureShader3:
    case GxRs_VertexShader:
        break;
    default:
        break;
    }

    char phase[48]{};
    std::snprintf(phase, sizeof(phase), "RenderState[%u]", renderState);
    GetError(phase);
}

void CGxDeviceOpenGl::GetError(const char* phase) const
{
    for (unsigned count = 0; count <= 256; ++count) {
        const GLenum error = glGetError();
        if (error == GL_NO_ERROR) {
            return;
        }
        char buffer[96]{};
        std::snprintf(buffer, sizeof(buffer), "Gx OpenGL error: 0x%04X phase=%s\n",
            error, phase ? phase : "unspecified");
        OutputDebugStringA(buffer);
        std::fputs(buffer, stderr);
        std::fflush(stderr);
    }
}

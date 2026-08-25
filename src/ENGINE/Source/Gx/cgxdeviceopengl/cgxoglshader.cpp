#include "ENGINE/Source/Gx/cgxdeviceopengl/cgxoglshader.h"

#include <algorithm>

namespace {

constexpr GLenum kRegisterCombiners = 0x8522;
constexpr GLenum kFragmentProgram = 0x8804;
constexpr GLenum kProgramFormatAscii = 0x8875;
constexpr GLenum kConstantColor0 = 0x852A;
constexpr GLenum kConstantColor1 = 0x852B;

} // namespace

void RegisterCombiners::CombinerPortion::Realize(unsigned stage, unsigned portion) const
{
    if (glCombinerInputNV == nullptr || glCombinerOutputNV == nullptr) {
        return;
    }
    for (unsigned variableIndex = 0; variableIndex < 4; ++variableIndex) {
        const CombinerVariable& value = variable[variableIndex];
        glCombinerInputNV(
            stage,
            portion,
            34083U + variableIndex,
            value.input,
            value.mapping,
            value.component);
    }

    const CombinerOutput& value = output;
    glCombinerOutputNV(
        stage,
        portion,
        value.abOutput,
        value.cdOutput,
        value.sumOutput,
        value.scale,
        value.bias,
        static_cast<GLboolean>(value.abDotProduct != 0),
        static_cast<GLboolean>(value.cdDotProduct != 0),
        static_cast<GLboolean>(value.muxSum != 0));
}

void RegisterCombiners::GeneralCombiner::Realize(unsigned stage, bool perStageConstants) const
{
    portion[0].Realize(stage, 0x1907);
    portion[1].Realize(stage, 0x1906);
    if (perStageConstants && glCombinerStageParameterfvNV != nullptr) {
        glCombinerStageParameterfvNV(stage, kConstantColor0, constants[0]);
        glCombinerStageParameterfvNV(stage, kConstantColor1, constants[1]);
    }
}

void RegisterCombiners::FinalCombiner::Realize() const
{
    if (glFinalCombinerInputNV == nullptr) {
        return;
    }
    for (unsigned variableIndex = 0; variableIndex < 7; ++variableIndex) {
        const CombinerVariable& value = variable[variableIndex];
        glFinalCombinerInputNV(
            34083U + variableIndex,
            value.input,
            value.mapping,
            value.component);
    }
}

void CGxDeviceOpenGl::IPixelShaderBind(CGxPixelShader* shader)
{
    const CGxPixelShader::Target target =
        shader != nullptr && shader->target != CGxPixelShader::Target_None
        ? shader->target
        : static_cast<CGxPixelShader::Target>(m_pixelShaderTarget);

    if (shader == nullptr) {
        if (target == CGxPixelShader::Target_nvrc) {
            glDisable(kRegisterCombiners);
        } else if (target == CGxPixelShader::Target_arbfp1) {
            glDisable(kFragmentProgram);
        }
        return;
    }

    if (target == CGxPixelShader::Target_nvrc) {
        for (unsigned stage = 0; stage < shader->combiners.size(); ++stage) {
            shader->combiners[stage].Realize(stage + 34128U, false);
        }
        shader->finalCombiner.Realize();
        glEnable(kRegisterCombiners);
    } else if (target == CGxPixelShader::Target_arbfp1
        && glBindProgramARB != nullptr) {
        glBindProgramARB(kFragmentProgram, shader->glApiSpecific);
        glEnable(kFragmentProgram);
    }
}

void CGxDeviceOpenGl::PixelShaderCreate(CGxPixelShader** shader, const char* filename)
{
    (void)filename;
    if (shader == nullptr) {
        return;
    }
    if (*shader == nullptr) {
        *shader = new CGxPixelShader();
        m_pixelShaders.push_back(*shader);
    }
    CGxPixelShader* value = *shader;
    value->target = static_cast<CGxPixelShader::Target>(m_pixelShaderTarget);
    if (value->code.empty() || !glARBFragmentProgram || glGenProgramsARB == nullptr
        || glBindProgramARB == nullptr || glProgramStringARB == nullptr) {
        return;
    }
    if (value->glApiSpecific != 0 && glDeleteProgramsARB != nullptr) {
        glDeleteProgramsARB(1, &value->glApiSpecific);
        value->glApiSpecific = 0;
    }
    glGenProgramsARB(1, &value->glApiSpecific);
    glBindProgramARB(kFragmentProgram, value->glApiSpecific);
    glProgramStringARB(
        kFragmentProgram,
        kProgramFormatAscii,
        static_cast<GLsizei>(value->code.size()),
        value->code.data());
    value->valid = value->glApiSpecific != 0;
}

void CGxDeviceOpenGl::PixelShaderDestroy(CGxPixelShader** shader)
{
    if (shader == nullptr || *shader == nullptr) {
        return;
    }
    CGxPixelShader* value = *shader;
    if (value->glApiSpecific != 0 && glDeleteProgramsARB != nullptr) {
        glDeleteProgramsARB(1, &value->glApiSpecific);
        value->glApiSpecific = 0;
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

void CGxDeviceOpenGl::ISetShaderParamList(
    const std::vector<CGxShaderParam*>& params,
    bool forceForBind)
{
    if (glProgramLocalParameter4fvARB == nullptr) {
        return;
    }
    for (CGxShaderParam* parameter : params) {
        if (parameter == nullptr || (!parameter->dirty && !forceForBind)) {
            continue;
        }
        for (unsigned vectorIndex = 0; vectorIndex < parameter->vectorCount; ++vectorIndex) {
            glProgramLocalParameter4fvARB(
                kFragmentProgram,
                parameter->index + vectorIndex,
                parameter->values + vectorIndex * 4);
        }
        parameter->dirty = false;
    }
}

void CGxDeviceOpenGl::IShaderForceRecreation()
{
    for (CGxPixelShader* shader : m_pixelShaders) {
        if (shader != nullptr && shader->glApiSpecific != 0 && glDeleteProgramsARB != nullptr) {
            glDeleteProgramsARB(1, &shader->glApiSpecific);
            shader->glApiSpecific = 0;
            shader->valid = false;
        }
    }
}

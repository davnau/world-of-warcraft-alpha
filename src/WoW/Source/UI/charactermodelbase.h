#pragma once

#include "ENGINE/Source/Frame/frame.h"

#include <cstdint>

class CGCharacterModelBase : public CSimpleModel {
public:
    explicit CGCharacterModelBase(CSimpleFrame* parent = nullptr);
    static CSimpleFrame* Create(CSimpleFrame* parent);
    void ConfigureCamera();
    void GetUniquePaperDollModel();
    void InitializeModel();
    int LookupScriptMethod(lua_State* state, const char* name) override;
    static void RegisterScriptMethods();
    void SetUnit(std::uint64_t unitGuid);
    static void UnregisterScriptMethods();
    void UpdateModel();
    ~CGCharacterModelBase() override;

    std::uint64_t m_unit = 0;
    float m_rotationScale = 0.0f;
    static inline std::vector<FrameScript_Method> s_scriptMethods{};
};


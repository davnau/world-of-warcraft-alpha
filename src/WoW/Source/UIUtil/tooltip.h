#pragma once

#include "ENGINE/Source/Frame/frame.h"

class CGTooltip : public CSimpleFrame {
public:
    explicit CGTooltip(CSimpleFrame* parent = nullptr);
    static CSimpleFrame* Create(CSimpleFrame* parent);
    void AddLine();
    void AppendText();
    void CalculateSize();
    void ClearLines();
    void FadeOut();
    void GetAuraEffectString();
    void GetItemEnchantString();
    void GetItemQualityColorString();
    void GetSpellEffectString();
    void GetSpellTargetString();
    void GetSummonedByString();
    void HideThis();
    int LookupScriptMethod(lua_State* state, const char* name) override;
    void OnLayerUpdate();
    void PostLoadXML();
    static void RegisterScriptMethods();
    void SetBuff();
    void SetCorpse();
    void SetItem();
    void SetObject();
    void SetOwner();
    void SetPosition();
    void SetSpell();
    void SetTooltipPadding();
    void SetUnit();
    void ShowThis();
    static void UnregisterScriptMethods();
    ~CGTooltip() override;

    FrameScript_Object* m_owner = nullptr;
    float m_padding = 0.0f;
    unsigned m_numLines = 0;
    static inline std::vector<FrameScript_Method> s_scriptMethods{};
};


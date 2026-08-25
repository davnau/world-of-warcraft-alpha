#pragma once

#include "WoW/Source/loadingscreen.h"
#include "WoW/common/compdecorationcore.h"

#include <array>
#include <cstdint>

struct HOBJECT__;
struct HMODEL__;
struct HTEXTURE__;
struct HTEXCOMPONENT__;

struct CTexturePiece {
    TEXTUREINFO m_textureInfo{};
    std::uint32_t m_holds = 0;
    bool m_hidden = false;

    CTexturePiece();
    ~CTexturePiece();
    CTexturePiece& operator=(const CTexturePiece& rhs);
};

struct CTextureLayer {
    std::array<CTexturePiece, 4> m_priorities{};
    CTextureLayer();
    ~CTextureLayer();
    CTextureLayer& operator=(const CTextureLayer& rhs);
};

struct CSection {
    std::array<CTextureLayer, 4> m_layers{};
    CSection& operator=(const CSection& rhs);
};

struct CTexComponent {
    std::array<CSection, 10> m_sections{};
    std::array<unsigned, 10> m_underwearHideCounts{};
    unsigned m_flags = 0;
    unsigned m_dirtyFlags = 0;
    int m_emblemStyle = -1;
    int m_emblemColor = -1;
    int m_borderStyle = -1;
    int m_borderColor = -1;
    int m_background = -1;
    char m_upperFaceTexture[260]{};
    char m_lowerFaceTexture[260]{};
    HTEXTURE__* m_texture = nullptr;

    ~CTexComponent();
    CTexComponent& operator=(const CTexComponent& rhs);
    bool HasTabard() const;
};

extern TEXCOMPONENT_SECTIONS s_tabardSections[18];
extern const char* s_tabardSectionSuffix[18];
extern float INFINITY_90;
extern float TWO_PI_90;
extern float OO_TWO_PI_90;

int __fastcall GetObjComponentInfo(unsigned sex, unsigned race, int displayID,
                                   unsigned inventoryType, std::uint8_t isPlayer,
                                   std::uint8_t useAlternate, HMODEL__** models,
                                   int* attachmentPoints);
std::uint8_t __fastcall ComponentApplyTabardTexture(CTexComponent* component,
                                                    int emblemStyle, int emblemColor,
                                                    int borderStyle, int borderColor,
                                                    int background);
void __fastcall GetTabardBackgroundFileName(int section, int style, char* buffer, unsigned size);
void __fastcall GetTabardEmblemFileName(int section, int style, int color, char* buffer, unsigned size);
void __fastcall GetTabardBorderFileName(int section, int style, int color, char* buffer, unsigned size);
void __fastcall ComponentRemoveTabardTexture(HTEXCOMPONENT__* component, HTEXCOMPONENT__* destination,
                                             const void* displayInfo, int inventoryType);
void __fastcall ComponentForceTabardDraw(CTexComponent* component);
void __fastcall TexComponentCopy(CTexComponent* destination, const CTexComponent* source);

int _E2_90(); int _E1_90(); int _E140_4(); int _E139_3(); void _E138_4();
int _E145_2(); int _E5_90(); void _E4_90(); int _E8_90(); void _E7_90();
int _E20_29(); void _E19_16(); int _E23_31(); void _E22_22();
int _E26_27(); void _E25_20(); int _E29_26(); void _E28_14();
int _E32_24(); void _E31_11(); int _E35_24(); void _E34_12();
int _E38_24(); void _E37_11(); int _E41_22(); void _E40_12();
int _E44_24(); void _E43_11(); int _E47_26(); void _E46_9();
int _E50_23(); void _E49_11(); int _E53_21(); void _E52_10();
int _E56_1(); void _E55_6(); int _E59_12(); void _E58_5();
int _E62_11(); void _E61_6(); int _E65_7(); void _E64_3();
int _E68_6(); void _E67_2();


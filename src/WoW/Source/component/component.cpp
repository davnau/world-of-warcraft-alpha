#include "WoW/Source/component/component.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <limits>

TEXCOMPONENT_SECTIONS s_tabardSections[18] = {
    TCS_UPPERARM, TCS_LOWERARM, TCS_HAND, TCS_UPPERHEAD, TCS_LOWERHEAD, TCS_UPPERTORSO,
    TCS_LOWERTORSO, TCS_LEGUPPER, TCS_LEGLOWER, TCS_FEET, TCS_UPPERARM, TCS_LOWERARM,
    TCS_HAND, TCS_UPPERHEAD, TCS_LOWERHEAD, TCS_UPPERTORSO, TCS_INVALIDSECTION, TCS_INVALIDSECTION
};
const char* s_tabardSectionSuffix[18] = {
    "_B", "_E", "_C", "_F", "_A", "_L", "_H", "_U", "_D", "_T",
    "_LF", "_UF", "_T", "_P", "_L", "_F", "", ""
};
float INFINITY_90 = 0.0F;
float TWO_PI_90 = 0.0F;
float OO_TWO_PI_90 = 0.0F;

CTexturePiece::CTexturePiece() {
    m_textureInfo.name = nullptr;
    m_textureInfo.centerX = 1.0F;
    m_textureInfo.centerY = 0.0F;
    m_textureInfo.width = 1.0F;
    m_textureInfo.height = 0.0F;
    m_textureInfo.scaleWithProgress = 0;
}
CTexturePiece::~CTexturePiece() = default;
CTexturePiece& CTexturePiece::operator=(const CTexturePiece& rhs) { if(this!=&rhs){m_textureInfo=rhs.m_textureInfo;m_holds=rhs.m_holds;m_hidden=rhs.m_hidden;} return *this; }
CTextureLayer::CTextureLayer() = default;
CTextureLayer::~CTextureLayer() = default;
CTextureLayer& CTextureLayer::operator=(const CTextureLayer& rhs){if(this!=&rhs)m_priorities=rhs.m_priorities;return *this;}
CSection& CSection::operator=(const CSection& rhs){if(this!=&rhs)m_layers=rhs.m_layers;return *this;}
CTexComponent::~CTexComponent() = default;
CTexComponent& CTexComponent::operator=(const CTexComponent& rhs){
    if(this==&rhs)return *this; m_sections=rhs.m_sections;m_underwearHideCounts=rhs.m_underwearHideCounts;
    m_flags|=rhs.m_flags;m_dirtyFlags=rhs.m_dirtyFlags;m_emblemStyle=rhs.m_emblemStyle;m_emblemColor=rhs.m_emblemColor;
    m_borderStyle=rhs.m_borderStyle;m_borderColor=rhs.m_borderColor;m_background=rhs.m_background;m_texture=rhs.m_texture;
    std::strncpy(m_upperFaceTexture,rhs.m_upperFaceTexture,sizeof(m_upperFaceTexture)-1);
    std::strncpy(m_lowerFaceTexture,rhs.m_lowerFaceTexture,sizeof(m_lowerFaceTexture)-1); return *this;
}
bool CTexComponent::HasTabard() const { return m_emblemStyle!=-1&&m_emblemColor!=-1&&m_borderStyle!=-1&&m_borderColor!=-1&&m_background!=-1; }

int __fastcall GetObjComponentInfo(unsigned, unsigned, int, unsigned, std::uint8_t,
                                   std::uint8_t, HMODEL__** models, int* attachmentPoints) {
    if(models)*models=nullptr; if(attachmentPoints)*attachmentPoints=0; return 0;
}

std::uint8_t __fastcall ComponentApplyTabardTexture(CTexComponent* component,int emblemStyle,int emblemColor,int borderStyle,int borderColor,int background){
    if(!component)return 0;
    if(component->m_emblemStyle==emblemStyle&&component->m_emblemColor==emblemColor&&component->m_borderStyle==borderStyle&&component->m_borderColor==borderColor&&component->m_background==background)return 1;
    component->m_emblemStyle=emblemStyle;component->m_emblemColor=emblemColor;component->m_borderStyle=borderStyle;component->m_borderColor=borderColor;component->m_background=background;
    if(emblemStyle<0||emblemColor<0||borderStyle<0||borderColor<0||background<0)return 0;
    for(auto section:s_tabardSections)if(section!=TCS_INVALIDSECTION)component->m_flags|=1u<<static_cast<unsigned>(section); return 1;
}

void __fastcall GetTabardBackgroundFileName(int section,int style,char* buffer,unsigned size){if(!buffer||!size)return;std::snprintf(buffer,size,"Textures\\GuildEmblems\\Background_%02d%s_U",style,s_tabardSectionSuffix[std::clamp(section,0,17)]);}
void __fastcall GetTabardEmblemFileName(int section,int style,int color,char* buffer,unsigned size){if(!buffer||!size)return;std::snprintf(buffer,size,"Textures\\GuildEmblems\\Emblem_%02d_%02d%s_U",style,color,s_tabardSectionSuffix[std::clamp(section,0,17)]);}
void __fastcall GetTabardBorderFileName(int section,int style,int color,char* buffer,unsigned size){if(!buffer||!size)return;std::snprintf(buffer,size,"Textures\\GuildEmblems\\Border_%02d_%02d%s_U",style,color,s_tabardSectionSuffix[std::clamp(section,0,17)]);}
void __fastcall ComponentRemoveTabardTexture(HTEXCOMPONENT__*,HTEXCOMPONENT__*,const void*,int){ }
void __fastcall ComponentForceTabardDraw(CTexComponent* component){if(component){component->m_flags|=0x60;component->m_dirtyFlags|=2;}}
void __fastcall TexComponentCopy(CTexComponent* destination,const CTexComponent* source){if(destination&&source)*destination=*source;}

int _E1_90(){INFINITY_90=std::numeric_limits<float>::infinity();return 2139095040;} int _E2_90(){return _E1_90();}
void _E138_4(){} int _E139_3(){_E138_4();return 0;} int _E140_4(){return _E139_3();}
void _E4_90(){TWO_PI_90=6.28318530717958647692F;} int _E5_90(){_E4_90();return 0;}
void _E7_90(){OO_TWO_PI_90=1.0F/TWO_PI_90;} int _E8_90(){_E7_90();return 0;}
int _E145_2(){return 0;}
void _E19_16(){} int _E20_29(){_E19_16();return 0;} void _E22_22(){} int _E23_31(){_E22_22();return 0;}
void _E25_20(){} int _E26_27(){_E25_20();return 0;} void _E28_14(){} int _E29_26(){_E28_14();return 0;}
void _E31_11(){} int _E32_24(){_E31_11();return 0;} void _E34_12(){} int _E35_24(){_E34_12();return 0;}
void _E37_11(){} int _E38_24(){_E37_11();return 0;} void _E40_12(){} int _E41_22(){_E40_12();return 0;}
void _E43_11(){} int _E44_24(){_E43_11();return 0;} void _E46_9(){} int _E47_26(){_E46_9();return 0;}
void _E49_11(){} int _E50_23(){_E49_11();return 0;} void _E52_10(){} int _E53_21(){_E52_10();return 0;}
void _E55_6(){} int _E56_1(){_E55_6();return 0;} void _E58_5(){} int _E59_12(){_E58_5();return 0;}
void _E61_6(){} int _E62_11(){_E61_6();return 0;} void _E64_3(){} int _E65_7(){_E64_3();return 0;}
void _E67_2(){} int _E68_6(){_E67_2();return 0;}

#include "WoW/Source/Game/GameClient/playername.h"
#include <limits>
struct HPLAYERNAME__{PLAYERNAMEDESC* desc=nullptr;}; float INFINITY_368=0.0F; static PLAYERNAMEDESC* s_playerName=nullptr; static bool s_showNames=false;
int _E2_368(){return _E1_368();} int _E1_368(){INFINITY_368=std::numeric_limits<float>::infinity();return 2139095040;} int _E163_3(){return _E160_1(),_E162_2();} unsigned _E160_1(){return 0;} int _E162_2(){return 0;} void _E161_2(){}
PLAYERNAMEDESC::PLAYERNAMEDESC()=default; PLAYERNAMEDESC::~PLAYERNAMEDESC()=default; void PLAYERNAMEDESC::RenderWorldText(){} void PLAYERNAMEDESC::MoveGeoset(const NTempest::C3Vector*p){if(p)m_position=*p;} void PLAYERNAMEDESC::ShowWorldText(int v){m_visible=v!=0;} void PLAYERNAMEDESC::UpdateWorldPos(){} void PLAYERNAMEDESC::Render(const NTempest::C44Matrix*){} void PLAYERNAMEDESC::SetStringColor(const NTempest::CImVector*c){if(c)m_color=*c;} void PLAYERNAMEDESC::UpdateWorldText(){}
void PlayerNameInitialize(const char*){if(!s_playerName)s_playerName=new PLAYERNAMEDESC;} unsigned char UnitNameShowTypeCallback(const char*,unsigned){s_showNames=true;return 1;} PLAYERNAMEDESC* TriggerNameRegenerate(){return s_playerName;} void PlayerNameShutdown(){delete s_playerName;s_playerName=nullptr;} void PlayerNameShow(int v){s_showNames=v!=0;if(s_playerName)s_playerName->ShowWorldText(v);} HPLAYERNAME__* PlayerNameCreate(void*){auto*h=new HPLAYERNAME__;h->desc=new PLAYERNAMEDESC;return h;} void PlayerNameRenderCallback(const NTempest::C34Matrix*,float*,PLAYERNAMEDESC*p){if(p)p->Render(nullptr);} void PlayerNameTriggerColorUpdate(int){if(s_playerName)s_playerName->UpdateWorldText();} void PlayerNameUpdateWorldText(PLAYERNAMEDESC*p){if(p)p->UpdateWorldText();} void PlayerNameUpdateEarly(){if(s_playerName)s_playerName->UpdateWorldPos();} void PlayerNameUpdateLate(){if(s_playerName)s_playerName->UpdateWorldText();} void PlayerNameTriggerNameRegenerate(int){TriggerNameRegenerate();} void PlayerNameChangeLocation(PLAYERNAMEDESC*p,const NTempest::C3Vector*v){if(p)p->MoveGeoset(v);} int PlayerNameGetUnitNameMode(){return s_showNames?1:0;} void PlayerNameRenderWorldText(){if(s_playerName)s_playerName->RenderWorldText();}
void CalculateBillboardRotation(NTempest::C44Matrix* matrix, NTempest::C44Matrix* output){
    if(!matrix||!output)return;
    NTempest::C3Vector forward(matrix->a0,matrix->a1,matrix->a2);
    if(forward.SquaredMag()<=0.0F)forward=NTempest::C3Vector(0.0F,0.0F,1.0F);
    forward.Normalize();
    NTempest::C3Vector right(forward.y,-forward.x,0.0F);
    if(right.SquaredMag()<=0.0F)right=NTempest::C3Vector(1.0F,0.0F,0.0F);
    right.Normalize();
    NTempest::C3Vector up(right.y*forward.z,-right.x*forward.z,right.x*forward.y-right.y*forward.x);
    *output=NTempest::C44Matrix(right.x, right.y, right.z, 0.0F,
        up.x, up.y, up.z, 0.0F, forward.x, forward.y, forward.z, 0.0F,
        0.0F,0.0F,0.0F,1.0F);
}

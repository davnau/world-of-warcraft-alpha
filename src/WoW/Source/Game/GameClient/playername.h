#pragma once
#include "ENGINE/Source/Tempest/c34matrix.h"
#include "ENGINE/Source/Tempest/c44matrix.h"
#include "ENGINE/Source/Tempest/c3vector.h"
#include "ENGINE/Source/Tempest/cimvector.h"
#include <string>
struct HPLAYERNAME__; struct HMODEL__;
class PLAYERNAMEDESC { public: PLAYERNAMEDESC(); ~PLAYERNAMEDESC(); void RenderWorldText(); void MoveGeoset(const NTempest::C3Vector*); void ShowWorldText(int); void UpdateWorldPos(); void Render(const NTempest::C44Matrix*); void SetStringColor(const NTempest::CImVector*); void UpdateWorldText(); std::string m_name; NTempest::C3Vector m_position{}; NTempest::CImVector m_color{}; HMODEL__* m_model=nullptr; bool m_visible=false; };
void PlayerNameInitialize(const char*); unsigned char UnitNameShowTypeCallback(const char*,unsigned); PLAYERNAMEDESC* TriggerNameRegenerate(); void PlayerNameShutdown(); void PlayerNameShow(int); HPLAYERNAME__* PlayerNameCreate(void*); void PlayerNameRenderCallback(const NTempest::C34Matrix*,float*,PLAYERNAMEDESC*); void PlayerNameTriggerColorUpdate(int); void PlayerNameUpdateWorldText(PLAYERNAMEDESC*); void PlayerNameUpdateEarly(); void PlayerNameUpdateLate(); void PlayerNameTriggerNameRegenerate(int); void PlayerNameChangeLocation(PLAYERNAMEDESC*,const NTempest::C3Vector*); int PlayerNameGetUnitNameMode(); void PlayerNameRenderWorldText(); void CalculateBillboardRotation(NTempest::C44Matrix*,NTempest::C44Matrix*);
extern float INFINITY_368; int _E2_368(); int _E1_368(); int _E163_3(); unsigned _E160_1(); int _E162_2(); void _E161_2();

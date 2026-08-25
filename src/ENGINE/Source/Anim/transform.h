#pragma once
#include "ENGINE/Source/Anim/anim.h"
void RemoveRotation(NTempest::C34Matrix&);void RemoveRotationAndScaling(NTempest::C34Matrix&);void RemoveTranslation(NTempest::C34Matrix&);void RemoveScale(NTempest::C34Matrix&);void TranslateView(NTempest::C34Matrix&,const NTempest::C3Vector&);void RotateView(NTempest::C34Matrix&,const NTempest::C4Quaternion&);void ScaleView(NTempest::C34Matrix&,const NTempest::C3Vector&);

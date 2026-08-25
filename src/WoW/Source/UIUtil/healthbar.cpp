#include "WoW/Source/UIUtil/healthbar.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace {
float INFINITY_470 = 0.0F;
float TWO_PI_427 = 0.0F;
float OO_TWO_PI_428 = 0.0F;
}

CGSimpleHealthBar::CGSimpleHealthBar(CSimpleFrame* parent)
    : CSimpleStatusBar(parent), m_unitGUID(0), m_scaleColor(1) {
}

CGSimpleHealthBar::~CGSimpleHealthBar() {
    SetUnit(nullptr);
}

void CGSimpleHealthBar::SetUnit(CGUnit_C* unit) {
    RemoveMirrorHandlers();
    if (!unit) {
        m_unitGUID = 0;
        SetMinMaxValues(0.0F, 0.0F);
        SetValue(0.0F);
        return;
    }
    m_unitGUID = reinterpret_cast<std::uintptr_t>(unit);
    InstallMirrorHandlers();
    SetMinMaxValues(0.0F, 1.0F);
    SetValue(1.0F);
}

void CGSimpleHealthBar::SetValue(float value) {
    CSimpleStatusBar::SetValue(value);
    if (!m_scaleColor) return;
    const float normalized = static_cast<float>(GetAnimValue());
    const NTempest::CImVector color = normalized <= 0.2F
        ? NTempest::CImVector(0xFF0000FFu)
        : NTempest::CImVector(0x00FF00FFu);
    CSimpleStatusBar::SetStatusBarColor(color);
    m_scaleColor = 1;
}

void CGSimpleHealthBar::SetStatusBarColor(const NTempest::CImVector& color) {
    CSimpleStatusBar::SetStatusBarColor(color);
    m_scaleColor = 0;
}

void CGSimpleHealthBar::InstallMirrorHandlers() {
}

void CGSimpleHealthBar::RemoveMirrorHandlers() {
}

int __stdcall SimpleHealthUpdateHandler(std::uint64_t, const void*, CSimpleStatusBar*) {
    return 1;
}

int _E2_470() { return _E1_470(); }
int _E1_470() { INFINITY_470 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_440() { _E4_440(); return 0; }
void _E4_440() { TWO_PI_427 = 3.1415927F + 3.1415927F; }
int _E8_429() { _E7_440(); return 0; }
void _E7_440() { OO_TWO_PI_428 = 1.0F / TWO_PI_427; }


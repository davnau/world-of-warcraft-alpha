#include "WoW/Source/Game/GameClient/weapontrails.h"

#include <algorithm>
#include <cmath>
#include <limits>

float INFINITY_367 = 0.0F;
static std::vector<WTOBJECT*> s_trails;
static bool s_enabled = false;

int _E2_367(){return _E1_367();} int _E1_367(){INFINITY_367=std::numeric_limits<float>::infinity();return 2139095040;}
int _E159_3(){return _E156_2(),_E158_3();} int _E156_2(){return 0;} int _E158_3(){return 0;} void _E157_2(){}
int _E164_2(){return _E161_1(),_E163_2();} int _E161_1(){return 0;} int _E163_2(){return 0;} void _E162_1(){}
int _E169_2(){return _E166_0(),_E168_2();} int _E166_0(){return 0;} int _E168_2(){return 0;} void _E167_0(){}
int _E174_2(){return _E171_1(),_E173_1();} int _E171_1(){return 0;} int _E173_1(){return 0;} void _E172_0(){}

void SWING::Recycle() {
    vertices.clear();
    active = false;
}

void SWING::AddVerts(const NTempest::C44Matrix* basis,
                     const NTempest::C3Vector* bottom,
                     const NTempest::C3Vector* top,
                     const NTempest::CImVector* color,
                     unsigned char currentAlpha,
                     const NTempest::C3Vector* cameraPos) {
    if (!bottom || !top) return;
    NTempest::C3Vector low = *bottom;
    NTempest::C3Vector high = *top;
    if (basis) {
        low = *basis * low;
        high = *basis * high;
    }
    NTempest::C3Vector view(0.0F, 0.0F, 1.0F);
    if (cameraPos) {
        view = *cameraPos - low;
        if (view.SquaredMag() > 0.0F) view.Normalize();
    }
    NTempest::C3Vector edge = high - low;
    NTempest::C3Vector side = NTempest::C3Vector::Cross(view, edge);
    if (side.SquaredMag() > 0.0F) side.Normalize();
    side *= 0.015F;
    const NTempest::CImVector tint = color ? *color : NTempest::CImVector(0xFFFFFFFFU);
    NTempest::CImVector alpha = tint;
    alpha.a = currentAlpha;
    vertices.push_back({low - side, alpha, 1.0F});
    vertices.push_back({low + side, alpha, 1.0F});
    vertices.push_back({high - side, alpha, 1.0F});
    vertices.push_back({high + side, alpha, 1.0F});
    active = true;
}

void SWING::Render() {
    // The graphics backend consumes the generated strip in the renderer
    // bridge; this method intentionally retains the generated vertex state.
}

void WTOBJECT::Recycle() {
    for (auto& swing : swings) swing.Recycle();
    swings.clear();
    drawing = false;
}

void WTOBJECT::Render(const NTempest::C44Matrix* basis) {
    if (!drawing || swings.empty()) return;
    if (!swings.front().active) {
        const NTempest::C3Vector bottom(0.0F, 0.0F, 0.0F);
        const NTempest::C3Vector top(0.0F, 0.0F, 1.0F);
        swings.front().AddVerts(basis, &bottom, &top, &color, 255, nullptr);
    }
    RenderVerts(nullptr);
    FadeVerts();
}

void WTOBJECT::RenderVerts(const NTempest::C3Vector*) {
    for (auto& swing : swings) swing.Render();
}

void WTOBJECT::FadeVerts() {
    for (auto& swing : swings) {
        for (auto& vertex : swing.vertices) {
        vertex.time -= static_cast<float>(std::max(0, fadeOutRate)) * 0.016F;
            if (fadeOutRate > 0) {
                const int alpha = std::max(0, static_cast<int>(vertex.color.a) - fadeOutRate);
                vertex.color.a = static_cast<unsigned char>(alpha);
            }
        }
        swing.vertices.erase(std::remove_if(swing.vertices.begin(), swing.vertices.end(),
            [](const VERTEX& vertex) { return vertex.time <= 0.0F || vertex.color.a == 0; }),
            swing.vertices.end());
        if (swing.vertices.empty()) swing.active = false;
    }
}

void WTOBJECT::DisableDrawing() { drawing = false; }

void WTOBJECT::SetDrawTrail(const NTempest::CImVector* trailColor,
                            int rate, int (*)(const void*, void*)) {
    if (trailColor) color = *trailColor;
    fadeOutRate = rate;
    drawing = true;
    if (swings.empty()) swings.emplace_back();
}

void WTOBJECT::SetColor(NTempest::CImVector value) { color = value; }
void WTOBJECT::SetFadeOutRate(int rate) { fadeOutRate = rate; }
void WTOBJECT::SetDrawing(bool value) { drawing = value; }

void WeaponTrailsInitialize() { s_enabled = true; }
unsigned char ToggleCallback_0(const char*, void*) { s_enabled = !s_enabled; return 1; }
void WeaponTrailsShutdown() {
    for (auto* trail : s_trails) delete trail;
    s_trails.clear();
    s_enabled = false;
}

WTOBJECT* WeaponTrailCreate(int) {
    auto* trail = new WTOBJECT;
    trail->drawing = s_enabled;
    trail->swings.emplace_back();
    s_trails.push_back(trail);
    return trail;
}

void GeosetRenderFunction(const NTempest::C34Matrix*, float* transform, WTOBJECT* trail) {
    if (!trail) return;
    NTempest::C44Matrix basis;
    if (transform) {
        basis.a0 = transform[0]; basis.a1 = transform[1]; basis.a2 = transform[2];
        basis.b0 = transform[3]; basis.b1 = transform[4]; basis.b2 = transform[5];
        basis.c0 = transform[6]; basis.c1 = transform[7]; basis.c2 = transform[8];
        basis.d0 = transform[9]; basis.d1 = transform[10]; basis.d2 = transform[11];
    }
    trail->Render(&basis);
}

void WeaponTrailClose(void*, WTOBJECT* trail) {
    if (!trail) return;
    s_trails.erase(std::remove(s_trails.begin(), s_trails.end(), trail), s_trails.end());
    delete trail;
}
void WeaponTrailSetColor(WTOBJECT* trail, NTempest::CImVector color) { if (trail) trail->SetColor(color); }
void WeaponTrailSetFadeOutRate(WTOBJECT* trail, int rate) { if (trail) trail->SetFadeOutRate(rate); }
void WeaponTrailDisableDrawing(WTOBJECT* trail) { if (trail) trail->DisableDrawing(); }
void WeaponTrailSetDrawing(WTOBJECT* trail, const NTempest::CImVector* color,
                           int fadeOutRate, int (*timer)(const void*, void*)) {
    if (trail) trail->SetDrawTrail(color, fadeOutRate, timer);
}
int DiscontinueTimerHandler(void* userArg, int) {
    auto* trail = static_cast<WTOBJECT*>(userArg);
    if (trail) trail->DisableDrawing();
    return 1;
}

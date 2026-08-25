#pragma once

#include "ENGINE/Source/Frame/frame.h"

#include <cstdint>

struct CGUnit_C;

class CGSimpleHealthBar : public CSimpleStatusBar {
public:
    explicit CGSimpleHealthBar(CSimpleFrame* parent = nullptr);
    ~CGSimpleHealthBar() override;

    void SetUnit(CGUnit_C* unit);
    void SetValue(float value);
    void SetStatusBarColor(const NTempest::CImVector& color);
    void InstallMirrorHandlers();
    void RemoveMirrorHandlers();

    std::uint64_t m_unitGUID = 0;
    int m_scaleColor = 1;
};

int __stdcall SimpleHealthUpdateHandler(std::uint64_t guid, const void* data,
                                        CSimpleStatusBar* parameter);

int _E2_470();
int _E1_470();
int _E5_440();
void _E4_440();
int _E8_429();
void _E7_440();


#pragma once

#include "WoW/common/wowtime.h"

#include <cstdint>
#include <vector>

class CGameTime;

using GameTimeCallback = void (__stdcall *)(const WowTime*, void*);

struct HGAMETIMECALLBACK__ {
    CGameTime* owner{};
    WowTime time{};
    GameTimeCallback callback{};
    void* userData{};
    bool registered{};
};

class CGameTime {
public:
    // This is the first member in the recovered object.  Several original
    // call sites pass a CGameTime* where a WowTime* is expected.
    ::WowTime WowTime;
    int m_timeBias{};
    int m_dateBias{};
    int m_gameMinutesElapsed{};
    float m_gameMinutesPerRealSecond{};
    float m_gameMinutesThisTick{};
    int m_timeDifferential{};
    std::uint32_t m_lastTickMinute{};
    double m_dayProgression{};
    std::vector<HGAMETIMECALLBACK__*> m_callbacks;

    CGameTime();
    ~CGameTime();

    void Destroy();
    void SetTimeDateBias(int timeBias, int dateBias, unsigned char update);
    void GameTimeSetTime(const ::WowTime* time);
    void GameTimeUpdate(float elapsedSeconds);
    void GameTimeSync(unsigned char reset);
    void GameTimeSync(const ::WowTime* time, unsigned char reset);
    double GameTimeGetMinutesPerSecond() const;
    double GameTimeSetMinutesPerSecond(float minutesPerSecond);
    HGAMETIMECALLBACK__* GameTimeRegisterCallback(::WowTime* time,
                                                   GameTimeCallback callback,
                                                   void* user);
    void GameTimeUnregisterCallback(HGAMETIMECALLBACK__* callbackHandle);
    double GameTimeGetDayProgression() const;
    void TickMinute();
    void PerformCallbacks(int minutes);
};

int _E2_301();
int _E1_301();
void _E4_297();
void _E7_297();
void _E13_13();
void _E16_25();
void _E19_52();
void _E22_60();
void _E25_61();
void _E28_99();
void _E31_96();
void _E34_73();
void _E37_68();
void _E40_69();
void _E43_67();
void _E46_65();
int _E5_297();
int _E8_297();
int _E14_18();
int _E17_20();
int _E20_69();
int _E23_74();
int _E26_70();
int _E29_70();
int _E32_62();
int _E35_43();
int _E38_42();
int _E41_38();
int _E44_40();
int _E47_41();

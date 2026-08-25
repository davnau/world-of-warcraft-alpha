#pragma once

#include "ENGINE/Source/Event/event.h"
#include "WoW/Source/DB/DBClient/AutoCode/soundproviderpreferencesrec.h"

#include <cstdint>

struct SoundProviderDesc {
    int Environment = 0;
    float EnvSize = 7.5F;
    float EnvDiffusion = 1.0F;
    int Room = -10000;
    int RoomHF = -10000;
    int RoomLF = 0;
    float DecayTime = 1.0F;
    float DecayHFRatio = 1.0F;
    float DecayLFRatio = 1.0F;
    int Reflections = -2602;
    float ReflectionsDelay = 0.007F;
    float ReflectionsPan[3]{};
    int Reverb = 200;
    float ReverbDelay = 0.011F;
    float ReverbPan[3]{};
    float EchoTime = 0.25F;
    float EchoDepth = 0.0F;
    float ModulationTime = 0.25F;
    float ModulationDepth = 0.0F;
    float AirAbsorptionHF = -5.0F;
    float HFReference = 5000.0F;
    float LFReference = 250.0F;
    float RoomRolloffFactor = 0.0F;
    unsigned Flags = 831;
};

extern bool g_underWater;
extern SoundProviderDesc s_currentProviderDesc;

void SndInterfaceFadeProviderPrefs(unsigned now);
double InterpFloat(float progress, float start, float end);
std::int64_t InterpInt(int start, int end, float progress);
void StopWorldIdleHandler(EVENTID eventId = EVENT_ID_IDLE);
int __fastcall WorldIdleHandler(const void* eventData, void* parameter);
void SndInterfaceSetProviderPrefs(int normalId, int underwaterId, unsigned transitionDuration);
void SndInterfaceSetProviderPrefs(const SoundProviderDesc& normal, const SoundProviderDesc& underwater);
void StartProviderPrefFade(const SoundProviderDesc& target, unsigned duration);
void StartWorldIdleHandler();
void StopProviderPrefFade();
void SaveDesc(SoundProviderDesc& destination, const SoundProviderPreferencesRec& record);
void SndInterfaceClearProviderPrefs(int indoor);
void SndSetRoomType(EVENTID roomType, int room);
void ProviderPrefShutdown(const char* previous = nullptr);
void ProviderPrefInitialize(const char* previous = nullptr);
int __fastcall EnvironmentHandler(const char* arguments, const char* value);
int EnvironmentListHandler();
void SndInterfaceProviderPrefsUnderwaterChanged();

int _E2_72(); int _E1_72(); int _E5_72(); void _E4_72(); int _E8_72(); void _E7_72();
int _E21_7(); void _E20_11(); int _E24_10(); void _E23_14(); int _E27_7(); void _E26_10();
int _E30_5(); void _E29_9(); int _E33_3(); void _E32_7(); int _E36_2(); void _E35_8();
int _E39_4(); void _E38_8(); int _E42_3(); void _E41_6(); int _E45_2(); void _E44_8();
int _E48_7(); void _E47_10(); int _E51_1(); void _E50_11(); int _E54_6(); void _E53_9();

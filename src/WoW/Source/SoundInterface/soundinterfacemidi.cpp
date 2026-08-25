#include "WoW/Source/SoundInterface/soundinterfacemidi.h"

#include "ENGINE/Source/Os/W32/ossound.h"

#include <cstdlib>
#include <limits>

float INFINITY_80 = 0.0F;
float TWO_PI_80 = 0.0F;
float OO_TWO_PI_80 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_12 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_12 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_12 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_12 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_12 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_12 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_12 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_12 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_12 = 0.0F;
float SECONDS_PER_HEARTBEAT_12 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_12 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_12 = 0.0F;

float s_midiVolume = 1.0F;
bool s_midiPaused = false;
bool s_midiUnderwater = false;

namespace {
int s_normalArea = 0;
int s_underwaterArea = 0;
bool s_initialized = false;
}

void SoundInterfaceInitializeWorldMIDICVars(const char*)
{
    s_midiVolume = 1.0F;
    s_midiPaused = false;
}

unsigned char AmbienceVolumeHandler(const char* newValue, void*)
{
    s_midiVolume = newValue ? std::strtof(newValue, nullptr) : 0.0F;
    Sound::MIDI_SetVolume(s_midiVolume);
    if (s_midiVolume <= 0.0F)
        SndInterfaceMIDISetPaused(1);
    else if (!s_midiPaused)
        StartAmbience();
    return 1;
}

unsigned char EnableAmbienceHandler(const char* newValue, void*)
{
    SndInterfaceMIDISetPaused(newValue && std::strtol(newValue, nullptr, 0) == 0 ? 1 : 0);
    return 1;
}

void SoundInterfaceInitializeWorldMIDI(const char* previous)
{
    SoundInterfaceInitializeWorldMIDICVars(previous);
    s_initialized = Sound::MIDI_Initialize();
    s_normalArea = 0;
    s_underwaterArea = 0;
}

void SoundInterfaceShutdownWorldMIDI()
{
    if (s_initialized)
        Sound::MIDI_Shutdown();
    s_initialized = false;
}

void SndInterfaceSetMIDIArea(int normalArea, int underwaterArea)
{
    if (normalArea == s_normalArea && underwaterArea == s_underwaterArea)
        return;
    s_normalArea = normalArea;
    s_underwaterArea = underwaterArea;
    StartAmbience();
}

void StartAmbience()
{
    Sound::MIDI_Stop();
    if (!s_initialized || s_midiPaused || s_midiVolume <= 0.0F)
        return;
    // The DBC records carry the sequence/DLS names.  Until those records are
    // connected, the state transition and MIDI volume contract are retained.
    Sound::MIDI_SetVolume(s_midiVolume);
}

void SndInterfaceClearMIDI()
{
    s_normalArea = 0;
    s_underwaterArea = 0;
    Sound::MIDI_Stop();
}

void SndInterfaceMIDIAmbienceChanged() { StartAmbience(); }
void SndInterfaceMIDIUnderwaterChanged() { StartAmbience(); }

void SndInterfaceMIDISetPaused(unsigned char paused)
{
    s_midiPaused = paused != 0;
    if (s_midiPaused)
        Sound::MIDI_Stop();
    else
        StartAmbience();
}

int _E2_80() { return _E1_80(); }
int _E1_80() { INFINITY_80 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_80() { _E4_80(); return 0; }
void _E4_80() { TWO_PI_80 = 3.1415927F + 3.1415927F; }
int _E8_80() { _E7_80(); return 0; }
void _E7_80() { OO_TWO_PI_80 = 1.0F / TWO_PI_80; }

#define MIDI_CONST_PAIR(thunk, body, variable, value) \
    int thunk() { body(); return 0; } \
    void body() { variable = value; }
MIDI_CONST_PAIR(_E21_15, _E20_19, MAX_DUEL_DISTANCE_SQUARED_12, 100.0F)
MIDI_CONST_PAIR(_E24_18, _E23_22, MAX_TRADE_DISTANCE_SQUARED_12, 123.45679F)
MIDI_CONST_PAIR(_E27_15, _E26_18, MAX_SHOP_DISTANCE_SQUARED_12, 30.864195F)
MIDI_CONST_PAIR(_E30_13, _E29_17, MAX_INSPECT_DISTANCE_SQUARED_12, 30.864195F)
MIDI_CONST_PAIR(_E33_11, _E32_15, MAX_LOOT_DISTANCE_SQUARED_12, 25.0F)
MIDI_CONST_PAIR(_E36_10, _E35_16, MAX_BIND_DISTANCE_SQUARED_12, 100.0F)
MIDI_CONST_PAIR(_E39_12, _E38_16, MAX_SITCHAIR_DISTANCE_SQUARED_12, 0.25F)
MIDI_CONST_PAIR(_E42_11, _E41_14, MAX_SITCHAIRUSE_DISTANCE_SQUARED_12, 9.0F)
MIDI_CONST_PAIR(_E45_10, _E44_16, MAX_SHARE_DISTANCE_SQUARED_12, 10000.0F)
MIDI_CONST_PAIR(_E48_15, _E47_18, SECONDS_PER_HEARTBEAT_12, 5.0F)
MIDI_CONST_PAIR(_E51_9, _E50_19, SPELL_FOCUS_DISTANCE_SQUARED_12, 2500.0F)
MIDI_CONST_PAIR(_E54_14, _E53_17, MAX_OBJ_INTEREST_RADIUS_SQUARED_12, 10000.0F)
#undef MIDI_CONST_PAIR


#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SoundProviderPreferencesRec final : public ClientDBRecord {
public:
    int m_ID = 0;
    const char* m_Description = nullptr;
    int m_Flags = 0;
    int m_EAXEnvironmentSelection = 0;
    float m_EAXEffectVolume = 0.0F;
    float m_EAXDecayTime = 0.0F;
    float m_EAXDamping = 0.0F;
    float m_EAX2EnvironmentSize = 0.0F;
    float m_EAX2EnvironmentDiffusion = 0.0F;
    int m_EAX2Room = 0;
    int m_EAX2RoomHF = 0;
    float m_EAX2DecayHFRatio = 0.0F;
    int m_EAX2Reflections = 0;
    float m_EAX2ReflectionsDelay = 0.0F;
    int m_EAX2Reverb = 0;
    float m_EAX2ReverbDelay = 0.0F;
    float m_EAX2RoomRolloff = 0.0F;
    float m_EAX2AirAbsorption = 0.0F;
    int m_EAX3RoomLF = 0;
    float m_EAX3DecayLFRatio = 0.0F;
    float m_EAX3EchoTime = 0.0F;
    float m_EAX3EchoDepth = 0.0F;
    float m_EAX3ModulationTime = 0.0F;
    float m_EAX3ModulationDepth = 0.0F;
    float m_EAX3HFReference = 0.0F;
    float m_EAX3LFReference = 0.0F;
    static const char* GetFilename();
    SoundProviderPreferencesRec();
    ~SoundProviderPreferencesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_217();
int _E1_217();
int _E5_216();
int _E4_216();
int _E8_216();
int _E7_216();

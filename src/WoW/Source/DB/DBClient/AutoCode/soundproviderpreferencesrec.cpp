#include "WoW/Source/DB/DBClient/AutoCode/soundproviderpreferencesrec.h"

#include "Storm/h/storm.h"

#include <limits>

const char* SoundProviderPreferencesRec::GetFilename() {
    return "DBFilesClient\\SoundProviderPreferences.dbc";
}

SoundProviderPreferencesRec::SoundProviderPreferencesRec() = default;
SoundProviderPreferencesRec::~SoundProviderPreferencesRec() = default;

unsigned char SoundProviderPreferencesRec::Read(SFile* file, const char* stringBuffer) {
    unsigned stringIndex = 0;
    const bool ok = file != nullptr &&
        SFile::Read(file, &m_ID, sizeof(m_ID), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &stringIndex, sizeof(stringIndex), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_Flags, sizeof(m_Flags), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAXEnvironmentSelection, sizeof(m_EAXEnvironmentSelection), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAXEffectVolume, sizeof(m_EAXEffectVolume), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAXDecayTime, sizeof(m_EAXDecayTime), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAXDamping, sizeof(m_EAXDamping), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX2EnvironmentSize, sizeof(m_EAX2EnvironmentSize), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX2EnvironmentDiffusion, sizeof(m_EAX2EnvironmentDiffusion), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX2Room, sizeof(m_EAX2Room), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX2RoomHF, sizeof(m_EAX2RoomHF), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX2DecayHFRatio, sizeof(m_EAX2DecayHFRatio), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX2Reflections, sizeof(m_EAX2Reflections), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX2ReflectionsDelay, sizeof(m_EAX2ReflectionsDelay), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX2Reverb, sizeof(m_EAX2Reverb), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX2ReverbDelay, sizeof(m_EAX2ReverbDelay), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX2RoomRolloff, sizeof(m_EAX2RoomRolloff), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX2AirAbsorption, sizeof(m_EAX2AirAbsorption), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX3RoomLF, sizeof(m_EAX3RoomLF), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX3DecayLFRatio, sizeof(m_EAX3DecayLFRatio), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX3EchoTime, sizeof(m_EAX3EchoTime), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX3EchoDepth, sizeof(m_EAX3EchoDepth), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX3ModulationTime, sizeof(m_EAX3ModulationTime), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX3ModulationDepth, sizeof(m_EAX3ModulationDepth), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX3HFReference, sizeof(m_EAX3HFReference), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_EAX3LFReference, sizeof(m_EAX3LFReference), nullptr, nullptr, nullptr);
    if (!ok) return 0;
    m_Description = stringBuffer ? stringBuffer + stringIndex : nullptr;
    return ReadRecord(file, stringBuffer);
}

int _E2_217() { return 0; }
int _E1_217() { return 2139095040; }
int _E5_216() { return 0; }
int _E4_216() { return 0; }
int _E8_216() { return 0; }
int _E7_216() { return 0; }
unsigned int SFileReadTyped(SFile*, void*) { return 1; }

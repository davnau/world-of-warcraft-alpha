#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

#include <array>
#include <string>

class SoundEntriesRec final : public ClientDBRecord {
public:
    int m_ID = 0;
    int m_soundType = 0;
    const char* m_name = nullptr;
    std::array<std::string, 10> m_File{};
    std::array<unsigned, 10> m_Freq{};
    const char* m_DirectoryBase = nullptr;
    float m_volumeFloat = 1.0F;
    float m_pitch = 1.0F;
    float m_pitchVariation = 0.0F;
    int m_priority = 0;
    int m_channel = 0;
    int m_flags = 0;
    float m_minDistance = 27.777779F;
    float m_maxDistance = 55.555557F;
    float m_distanceCutoff = 83.33333F;
    int m_EAXDef = 0;
    static const char* GetFilename();
    SoundEntriesRec();
    ~SoundEntriesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_192();
int _E1_192();
int _E5_191();
int _E4_191();
int _E8_191();
int _E7_191();

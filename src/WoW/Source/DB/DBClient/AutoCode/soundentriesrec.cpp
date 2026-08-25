#include "WoW/Source/DB/DBClient/AutoCode/soundentriesrec.h"

#include "Storm/h/storm.h"

#include <limits>

const char* SoundEntriesRec::GetFilename() {
    return "DBFilesClient\\SoundEntries.dbc";
}

SoundEntriesRec::SoundEntriesRec() = default;
SoundEntriesRec::~SoundEntriesRec() = default;

unsigned char SoundEntriesRec::Read(SFile* file, const char* stringBuffer) {
    unsigned nameIndex = 0;
    std::array<unsigned, 10> fileIndices{};
    unsigned directoryIndex = 0;
    const bool ok = file != nullptr &&
        SFile::Read(file, &m_ID, sizeof(m_ID), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_soundType, sizeof(m_soundType), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &nameIndex, sizeof(nameIndex), nullptr, nullptr, nullptr);
    if (!ok || !SFile::Read(file, fileIndices.data(), sizeof(fileIndices), nullptr, nullptr, nullptr) ||
        !SFile::Read(file, m_Freq.data(), sizeof(m_Freq), nullptr, nullptr, nullptr) ||
        !SFile::Read(file, &directoryIndex, sizeof(directoryIndex), nullptr, nullptr, nullptr) ||
        !SFile::Read(file, &m_volumeFloat, sizeof(m_volumeFloat), nullptr, nullptr, nullptr) ||
        !SFile::Read(file, &m_pitch, sizeof(m_pitch), nullptr, nullptr, nullptr) ||
        !SFile::Read(file, &m_pitchVariation, sizeof(m_pitchVariation), nullptr, nullptr, nullptr) ||
        !SFile::Read(file, &m_priority, sizeof(m_priority), nullptr, nullptr, nullptr) ||
        !SFile::Read(file, &m_channel, sizeof(m_channel), nullptr, nullptr, nullptr) ||
        !SFile::Read(file, &m_flags, sizeof(m_flags), nullptr, nullptr, nullptr) ||
        !SFile::Read(file, &m_minDistance, sizeof(m_minDistance), nullptr, nullptr, nullptr) ||
        !SFile::Read(file, &m_maxDistance, sizeof(m_maxDistance), nullptr, nullptr, nullptr) ||
        !SFile::Read(file, &m_distanceCutoff, sizeof(m_distanceCutoff), nullptr, nullptr, nullptr) ||
        !SFile::Read(file, &m_EAXDef, sizeof(m_EAXDef), nullptr, nullptr, nullptr)) return 0;
    m_name = stringBuffer ? stringBuffer + nameIndex : nullptr;
    m_DirectoryBase = stringBuffer ? stringBuffer + directoryIndex : nullptr;
    for (std::size_t i = 0; i < m_File.size(); ++i)
        m_File[i] = stringBuffer ? stringBuffer + fileIndices[i] : "";
    return ReadRecord(file, stringBuffer);
}

int _E2_192() { return 0; }
int _E1_192() { return 2139095040; }
int _E5_191() { return 0; }
int _E4_191() { return 0; }
int _E8_191() { return 0; }
int _E7_191() { return 0; }

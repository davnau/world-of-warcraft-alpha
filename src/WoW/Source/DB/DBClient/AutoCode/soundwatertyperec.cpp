#include "WoW/Source/DB/DBClient/AutoCode/soundwatertyperec.h"

#include "Storm/h/storm.h"

#include <limits>

const char* SoundWaterTypeRec::GetFilename() {
    return "DBFilesClient\\SoundWaterType.dbc";
}

SoundWaterTypeRec::SoundWaterTypeRec() = default;
SoundWaterTypeRec::~SoundWaterTypeRec() = default;

unsigned char SoundWaterTypeRec::Read(SFile* file, const char* stringBuffer) {
    const bool ok = file != nullptr &&
        SFile::Read(file, &m_ID, sizeof(m_ID), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_soundType, sizeof(m_soundType), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_soundSubtype, sizeof(m_soundSubtype), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_SoundID, sizeof(m_SoundID), nullptr, nullptr, nullptr);
    return ok ? ReadRecord(file, stringBuffer) : 0;
}

int _E2_225() { return 0; }
int _E1_225() { return 2139095040; }
int _E5_224() { return 0; }
int _E4_224() { return 0; }
int _E8_224() { return 0; }
int _E7_224() { return 0; }

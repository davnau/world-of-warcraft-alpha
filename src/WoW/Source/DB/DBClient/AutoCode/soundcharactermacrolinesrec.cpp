#include "WoW/Source/DB/DBClient/AutoCode/soundcharactermacrolinesrec.h"

#include "Storm/h/storm.h"

#include <limits>

const char* SoundCharacterMacroLinesRec::GetFilename() {
    return "DBFilesClient\\SoundCharacterMacroLines.dbc";
}

SoundCharacterMacroLinesRec::SoundCharacterMacroLinesRec() = default;
SoundCharacterMacroLinesRec::~SoundCharacterMacroLinesRec() = default;

unsigned char SoundCharacterMacroLinesRec::Read(SFile* file, const char* stringBuffer) {
    const bool ok = file != nullptr &&
        SFile::Read(file, &m_ID, sizeof(m_ID), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_Category, sizeof(m_Category), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_Sex, sizeof(m_Sex), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_Race, sizeof(m_Race), nullptr, nullptr, nullptr) &&
        SFile::Read(file, &m_SoundID, sizeof(m_SoundID), nullptr, nullptr, nullptr);
    return ok ? ReadRecord(file, stringBuffer) : 0;
}

int _E2_242() { return 0; }
int _E1_242() { return 2139095040; }
int _E5_241() { return 0; }
int _E4_241() { return 0; }
int _E8_241() { return 0; }
int _E7_241() { return 0; }

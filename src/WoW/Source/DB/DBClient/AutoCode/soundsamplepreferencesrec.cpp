#include "WoW/Source/DB/DBClient/AutoCode/soundsamplepreferencesrec.h"

#include <limits>

const char* SoundSamplePreferencesRec::GetFilename() {
    return "DBFilesClient\\SoundSamplePreferences.dbc";
}

SoundSamplePreferencesRec::SoundSamplePreferencesRec() = default;
SoundSamplePreferencesRec::~SoundSamplePreferencesRec() = default;

unsigned char SoundSamplePreferencesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_218() { return 0; }
int _E1_218() { return 2139095040; }
int _E5_217() { return 0; }
int _E4_217() { return 0; }
int _E8_217() { return 0; }
int _E7_217() { return 0; }

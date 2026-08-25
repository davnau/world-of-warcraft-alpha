#include "WoW/Source/DB/DBClient/AutoCode/cinematicsequencesrec.h"

#include <limits>

const char* CinematicSequencesRec::GetFilename() {
    return "DBFilesClient\\CinematicSequences.dbc";
}

CinematicSequencesRec::CinematicSequencesRec() = default;
CinematicSequencesRec::~CinematicSequencesRec() = default;

unsigned char CinematicSequencesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_156() { return 0; }
int _E1_156() { return 2139095040; }
int _E5_155() { return 0; }
int _E4_155() { return 0; }
int _E8_155() { return 0; }
int _E7_155() { return 0; }

#include "WoW/Source/DB/DBClient/AutoCode/areamidiambiencesrec.h"
const char* AreaMIDIAmbiencesRec::GetFilename() { return "DBFilesClient\\AreaMIDIAmbiences.dbc"; }
AreaMIDIAmbiencesRec::AreaMIDIAmbiencesRec() = default;
AreaMIDIAmbiencesRec::~AreaMIDIAmbiencesRec() = default;
unsigned char AreaMIDIAmbiencesRec::Read(SFile* file, const char* strings) { return ReadRecord(file, strings); }
int _E2_222() { return 0; }
int _E1_222() { return 2139095040; }
int _E5_221() { return 0; }
int _E4_221() { return 0; }
int _E8_221() { return 0; }
int _E7_221() { return 0; }

#include "WoW/Source/DB/DBClient/AutoCode/pagetextmaterialrec.h"

#include <limits>

const char* PageTextMaterialRec::GetFilename() {
    return "DBFilesClient\\PageTextMaterial.dbc";
}

PageTextMaterialRec::PageTextMaterialRec() = default;
PageTextMaterialRec::~PageTextMaterialRec() = default;

unsigned char PageTextMaterialRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_209() { return 0; }
int _E1_209() { return 2139095040; }
int _E5_208() { return 0; }
int _E4_208() { return 0; }
int _E8_208() { return 0; }
int _E7_208() { return 0; }

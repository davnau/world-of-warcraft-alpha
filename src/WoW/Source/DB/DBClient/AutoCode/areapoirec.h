#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class AreaPOIRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    AreaPOIRec();
    ~AreaPOIRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);

    // The recovered client accesses these fields directly while constructing
    // minimap points of interest.  They are kept on the generated record so
    // the game-client code does not need an untyped byte-offset overlay.
    int m_ID = 0;
    int m_continentID = -1;
    int m_flags = 0;
    float m_x = 0.0F;
    float m_y = 0.0F;
    float m_z = 0.0F;
    int m_icon = 0;
    int m_importance = 0;
    char m_name_lang[4][256]{};
};

int _E2_188();
int _E1_188();
int _E5_187();
int _E4_187();
int _E8_187();
int _E7_187();

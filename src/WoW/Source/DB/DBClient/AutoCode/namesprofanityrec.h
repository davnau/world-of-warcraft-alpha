#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class NamesProfanityRec final : public ClientDBRecord {
public:
    std::string m_Name;

    static const char* GetFilename();
    NamesProfanityRec();
    ~NamesProfanityRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_246();
int _E1_246();
int _E5_245();
int _E4_245();
int _E8_245();
int _E7_245();

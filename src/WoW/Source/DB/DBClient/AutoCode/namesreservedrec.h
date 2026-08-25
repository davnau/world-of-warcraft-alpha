#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class NamesReservedRec final : public ClientDBRecord {
public:
    std::string m_Name;

    static const char* GetFilename();
    NamesReservedRec();
    ~NamesReservedRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_247();
int _E1_247();
int _E5_246();
int _E4_246();
int _E8_246();
int _E7_246();

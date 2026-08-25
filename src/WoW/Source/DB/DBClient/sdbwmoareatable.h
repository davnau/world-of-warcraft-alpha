#pragma once

#include "WoW/Source/DB/DBClient/AutoCode/wmoareatablerec.h"

char** __fastcall SDBWMOAreaTableLookup(int nameSetID, int languageID, int wmoGroupID);
bool __fastcall SDBWMOAreaTableLookup(int nameSetID, int languageID, int wmoGroupID,
                                      const WMOAreaTableRec** record);
int __cdecl bscompare(const WMOAreaTableRec* left, const WMOAreaTableRec* right);


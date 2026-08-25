#pragma once

#include "WoW/Source/DB/DBClient/AutoCode/itemsubclassrec.h"

#include <cstddef>

void SDBItemSubclassInitialize();
void SDBItemSubclassDestroy();
const ItemSubClassRec* __fastcall SDBItemSubclassGetSubClassRec(unsigned subClassID, unsigned classID);

int _E60_58();
unsigned* _E57_50();
int _E59_21();
void _E58_57();


#pragma once
char* __fastcall OsGetLastErrorStr(unsigned long error);
char* OsGetLastErrorStr();
void OsFreeLastErrorStr(void* text);

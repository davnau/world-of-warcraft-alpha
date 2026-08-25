#pragma once

#include "WoW/Source/SoundInterface/soundinterfaceproviderprefs.h"

#include <cstdio>
#include <memory>
#include <vector>

struct AREAHASHKEY {
    unsigned cont = 0;
    unsigned area = 0;
    unsigned subArea = 0;
    AREAHASHKEY& operator=(const AREAHASHKEY& rhs) = default;
    bool operator==(const AREAHASHKEY& rhs) const { return cont == rhs.cont && area == rhs.area && subArea == rhs.subArea; }
};

struct OUTDOORSCHUNKHASHOBJ {
    unsigned chunkNumber = 0;
    unsigned continentID = 0;
    unsigned areaID = 0;
    AREAHASHKEY key;
    SoundProviderDesc desc;
    void DumpInfo(int summary, const char* newlyCreated) const;
    void PrintInfo(FILE* outFile) const;
};

extern std::vector<std::unique_ptr<OUTDOORSCHUNKHASHOBJ>> s_chunkList;
extern int s_currentChunk;
extern unsigned s_currentContinent_1;

int DumpChunksOUTDOORS();
int __fastcall ShowCurrentChunkOUTDOORS(const char* command, const char* value = nullptr);
int __fastcall SetChunkPropertyOUTDOORS(const char* arguments, const char* value);
int __fastcall SetCurrentChunkOUTDOORS(const char* command, const char* value);
int CreateChunkOUTDOORS();
int SndDebugListChunksOUTDOORS();
void OutdoorsShutdown();
void SndDebugRegisterContinent(unsigned continent);

int _E2_81(); int _E1_81(); int _E60_11(); void _E59_9(); int _E63_4(); void _E62_8();
int _E66_2(); void _E65_4(); int _E69_4(); void _E68_3(); int _E72_4(); void _E71_3();
int _E140_1(); void _E137_1(); int _E139_0(); void _E138_1(); int _E145(); int _E142_0();
int _E144(); void _E143_1(); int _E5_81(); void _E4_81(); int _E8_81(); void _E7_81();
int _E21_16(); void _E20_20(); int _E24_19(); void _E23_23(); int _E27_16(); void _E26_19();
int _E30_14(); void _E29_18(); int _E33_12(); void _E32_16(); int _E36_11(); void _E35_17();
int _E39_13(); void _E38_17(); int _E42_12(); void _E41_15(); int _E45_11(); void _E44_17();
int _E48_16(); void _E47_19(); int _E51_10(); void _E50_20(); int _E54_15(); void _E53_18();

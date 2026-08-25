#pragma once

#include "WoW/Source/SoundInterface/soundinterfaceproviderprefs.h"

#include <cstdio>
#include <memory>
#include <string>
#include <vector>

struct CHUNKHASHOBJ {
    unsigned chunkNumber = 0;
    std::string zoneName;
    std::string subZoneName;
    SoundProviderDesc desc;

    void PrintInfo(FILE* outFile) const;
    void DumpInfo(int summary, const char* newlyCreated) const;
};

extern std::vector<std::unique_ptr<CHUNKHASHOBJ>> s_chunkList_0;
extern int s_currentChunk_0;

int SndDebugListChunksINDOORS();
int CreateChunkINDOORS();
int __fastcall SetCurrentChunkINDOORS(const char* command, const char* value);
int __fastcall ShowCurrentChunkINDOORS(const char* command, const char* value = nullptr);
int __fastcall SetChunkPropertyINDOORS(const char* arguments, const char* value);
int DumpChunksINDOORS();
void IndoorsShutdown();

int _E2_82(); int _E1_82(); int _E60_12(); void _E59_10(); int _E63_5(); void _E62_9();
int _E66_3(); void _E65_5(); int _E69_5(); void _E68_4(); int _E72_5(); void _E71_4();
int _E140_2(); void _E137_2(); int _E139_1(); void _E138_2(); int _E145_0(); int _E142_1();
int _E144_0(); void _E143_2(); int _E5_82(); void _E4_82(); int _E8_82(); void _E7_82();
int _E21_17(); void _E20_21(); int _E24_20(); void _E23_24(); int _E27_17(); void _E26_20();
int _E30_15(); void _E29_19(); int _E33_13(); void _E32_17(); int _E36_12(); void _E35_18();
int _E39_14(); void _E38_18(); int _E42_13(); void _E41_16(); int _E45_12(); void _E44_18();
int _E48_17(); void _E47_20(); int _E51_11(); void _E50_21(); int _E54_16(); void _E53_19();

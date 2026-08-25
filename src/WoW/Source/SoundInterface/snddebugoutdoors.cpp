#include "WoW/Source/SoundInterface/snddebugoutdoors.h"

#include "WoW/Source/clientruntime.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <limits>

float INFINITY_81 = 0.0F;
float TWO_PI_81 = 0.0F;
float OO_TWO_PI_81 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_16 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_16 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_16 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_16 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_16 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_16 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_16 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_16 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_16 = 0.0F;
float SECONDS_PER_HEARTBEAT_16 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_16 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_16 = 0.0F;

std::vector<std::unique_ptr<OUTDOORSCHUNKHASHOBJ>> s_chunkList;
int s_currentChunk = -1;
unsigned s_currentContinent_1 = 0;

namespace {
void SetProperty(SoundProviderDesc& desc, unsigned property, float value) {
    if (property == 1 || property == 22) desc.Environment = static_cast<int>(value);
    else if (property == 2) desc.DecayTime = value; else if (property == 3) desc.EnvSize = value;
    else if (property == 4) desc.EnvDiffusion = value; else if (property == 5) desc.Room = static_cast<int>(value);
    else if (property == 6) desc.RoomHF = static_cast<int>(value); else if (property == 7) desc.DecayHFRatio = value;
    else if (property == 8) desc.Reflections = static_cast<int>(value); else if (property == 9) desc.ReflectionsDelay = value;
    else if (property == 10) desc.Reverb = static_cast<int>(value); else if (property == 11) desc.ReverbDelay = value;
    else if (property == 12) desc.RoomRolloffFactor = value; else if (property == 13) desc.AirAbsorptionHF = value;
    else if (property == 14) desc.RoomLF = static_cast<int>(value); else if (property == 15) desc.DecayLFRatio = value;
    else if (property == 16) desc.EchoTime = value; else if (property == 17) desc.EchoDepth = value;
    else if (property == 18) desc.ModulationTime = value; else if (property == 19) desc.ModulationDepth = value;
    else if (property == 20) desc.HFReference = value; else if (property == 21) desc.LFReference = value;
}
}

void OUTDOORSCHUNKHASHOBJ::DumpInfo(int summary, const char* newlyCreated) const {
    if (newlyCreated) ConsolePrintf("%s", newlyCreated);
    ConsolePrintf("[%03u] continent=%u area=%u%s", chunkNumber, continentID, areaID, static_cast<int>(chunkNumber) == s_currentChunk ? " current" : "");
    if (!summary) ConsolePrintf("Environment=%d Room=%d DecayTime=%g EnvSize=%g", desc.Environment, desc.Room, desc.DecayTime, desc.EnvSize);
}
void OUTDOORSCHUNKHASHOBJ::PrintInfo(FILE* outFile) const { if (outFile) std::fprintf(outFile, "%u\n%u\n%u\n%d\n%.4f\n", continentID, areaID, chunkNumber, desc.Environment, desc.DecayTime); }
int DumpChunksOUTDOORS() {
    if (s_chunkList.empty()) return 1;
    for (unsigned index = 0; index < s_chunkList.size(); ++index) { std::ofstream file("SndEAXChunkInfo_OUTDOORS_" + std::to_string(index) + ".txt"); (void)file; }
    return 1;
}
int __fastcall ShowCurrentChunkOUTDOORS(const char* command, const char*) { if (s_currentChunk >= 0 && static_cast<std::size_t>(s_currentChunk) < s_chunkList.size()) s_chunkList[s_currentChunk]->DumpInfo(0, nullptr); else ConsolePrintf("%s", command ? command : "No chunks created"); return 1; }
int __fastcall SetChunkPropertyOUTDOORS(const char*, const char* value) {
    if (s_currentChunk < 0 || static_cast<std::size_t>(s_currentChunk) >= s_chunkList.size()) return 1;
    unsigned property = 0; float number = 0.0F; if (value) std::sscanf(value, "%u %f", &property, &number); SetProperty(s_chunkList[s_currentChunk]->desc, property, number); SndInterfaceSetProviderPrefs(s_chunkList[s_currentChunk]->desc, s_chunkList[s_currentChunk]->desc); return 1;
}
int __fastcall SetCurrentChunkOUTDOORS(const char* command, const char* value) { if (!value || !*value) { ConsolePrintf("%s", command ? command : "chunk"); return 1; } const int index = std::atoi(value); if (index >= 0 && static_cast<std::size_t>(index) < s_chunkList.size()) s_currentChunk = index; return 1; }
int CreateChunkOUTDOORS() { auto chunk = std::make_unique<OUTDOORSCHUNKHASHOBJ>(); chunk->chunkNumber = static_cast<unsigned>(s_chunkList.size()); chunk->continentID = s_currentContinent_1; chunk->areaID = chunk->chunkNumber; chunk->DumpInfo(1, "created"); s_currentChunk = static_cast<int>(chunk->chunkNumber); s_chunkList.push_back(std::move(chunk)); return 1; }
int SndDebugListChunksOUTDOORS() { for (const auto& chunk : s_chunkList) if (chunk) chunk->DumpInfo(1, nullptr); ConsolePrintf("listed %d entries", static_cast<int>(s_chunkList.size())); return 1; }
void OutdoorsShutdown() { s_chunkList.clear(); s_currentChunk = -1; }
void SndDebugRegisterContinent(unsigned continent) { s_currentContinent_1 = continent; OutdoorsShutdown(); }

int _E2_81() { return _E1_81(); } int _E1_81() { INFINITY_81 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E60_11() { _E59_9(); return _E63_4(); } void _E59_9() {} int _E63_4() { return 0; } void _E62_8() {}
int _E66_2() { _E65_4(); return 0; } void _E65_4() {} int _E69_4() { _E68_3(); return 0; } void _E68_3() {}
int _E72_4() { _E71_3(); return 0; } void _E71_3() {}
int _E140_1() { _E137_1(); return _E139_0(); } void _E137_1() {} int _E139_0() { return 0; } void _E138_1() {}
int _E145() { _E142_0(); return _E144(); } int _E142_0() { return 0; } int _E144() { return 0; } void _E143_1() { OutdoorsShutdown(); }
int _E5_81() { _E4_81(); return 0; } void _E4_81() { TWO_PI_81 = 6.2831854F; } int _E8_81() { _E7_81(); return 0; } void _E7_81() { OO_TWO_PI_81 = 1.0F / TWO_PI_81; }
#define OUTDOOR_CONST_PAIR(thunk, body, variable, value) int thunk() { body(); return 0; } void body() { variable = value; }
OUTDOOR_CONST_PAIR(_E21_16, _E20_20, MAX_DUEL_DISTANCE_SQUARED_16, 100.0F) OUTDOOR_CONST_PAIR(_E24_19, _E23_23, MAX_TRADE_DISTANCE_SQUARED_16, 123.45679F)
OUTDOOR_CONST_PAIR(_E27_16, _E26_19, MAX_SHOP_DISTANCE_SQUARED_16, 30.864195F) OUTDOOR_CONST_PAIR(_E30_14, _E29_18, MAX_INSPECT_DISTANCE_SQUARED_16, 30.864195F)
OUTDOOR_CONST_PAIR(_E33_12, _E32_16, MAX_LOOT_DISTANCE_SQUARED_16, 25.0F) OUTDOOR_CONST_PAIR(_E36_11, _E35_17, MAX_BIND_DISTANCE_SQUARED_16, 100.0F)
OUTDOOR_CONST_PAIR(_E39_13, _E38_17, MAX_SITCHAIR_DISTANCE_SQUARED_16, 0.25F) OUTDOOR_CONST_PAIR(_E42_12, _E41_15, MAX_SITCHAIRUSE_DISTANCE_SQUARED_16, 9.0F)
OUTDOOR_CONST_PAIR(_E45_11, _E44_17, MAX_SHARE_DISTANCE_SQUARED_16, 10000.0F) OUTDOOR_CONST_PAIR(_E48_16, _E47_19, SECONDS_PER_HEARTBEAT_16, 5.0F)
OUTDOOR_CONST_PAIR(_E51_10, _E50_20, SPELL_FOCUS_DISTANCE_SQUARED_16, 2500.0F) OUTDOOR_CONST_PAIR(_E54_15, _E53_18, MAX_OBJ_INTEREST_RADIUS_SQUARED_16, 10000.0F)
#undef OUTDOOR_CONST_PAIR

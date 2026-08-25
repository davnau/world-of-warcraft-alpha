#include "WoW/Source/SoundInterface/snddebugindoors.h"

#include "WoW/Source/clientruntime.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <limits>

float INFINITY_82 = 0.0F;
float TWO_PI_82 = 0.0F;
float OO_TWO_PI_82 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_17 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_17 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_17 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_17 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_17 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_17 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_17 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_17 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_17 = 0.0F;
float SECONDS_PER_HEARTBEAT_17 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_17 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_17 = 0.0F;

std::vector<std::unique_ptr<CHUNKHASHOBJ>> s_chunkList_0;
int s_currentChunk_0 = -1;

namespace {
void PrintDesc(const SoundProviderDesc& desc, FILE* file) {
    if (!file) return;
    std::fprintf(file, "%d\n%.4f\n%.4f\n%.4f\n%d\n%d\n%.4f\n%d\n%.4f\n%d\n%.4f\n%.4f\n%.4f\n%d\n%.4f\n%.4f\n%.4f\n%.4f\n%.4f\n%.4f\n%.4f\n%d\n",
        desc.Environment, desc.DecayTime, desc.EnvSize, desc.EnvDiffusion, desc.Room, desc.RoomHF,
        desc.DecayHFRatio, desc.Reflections, desc.ReflectionsDelay, desc.Reverb, desc.ReverbDelay,
        desc.RoomRolloffFactor, desc.AirAbsorptionHF, desc.RoomLF, desc.DecayLFRatio, desc.EchoTime,
        desc.EchoDepth, desc.ModulationTime, desc.ModulationDepth, desc.HFReference, desc.LFReference, desc.Environment);
}
void SetProperty(SoundProviderDesc& desc, unsigned property, float value) {
    switch (property) {
    case 1: case 22: desc.Environment = static_cast<int>(value); break;
    case 2: desc.DecayTime = value; break; case 3: desc.EnvSize = value; break; case 4: desc.EnvDiffusion = value; break;
    case 5: desc.Room = static_cast<int>(value); break; case 6: desc.RoomHF = static_cast<int>(value); break;
    case 7: desc.DecayHFRatio = value; break; case 8: desc.Reflections = static_cast<int>(value); break;
    case 9: desc.ReflectionsDelay = value; break; case 10: desc.Reverb = static_cast<int>(value); break;
    case 11: desc.ReverbDelay = value; break; case 12: desc.RoomRolloffFactor = value; break;
    case 13: desc.AirAbsorptionHF = value; break; case 14: desc.RoomLF = static_cast<int>(value); break;
    case 15: desc.DecayLFRatio = value; break; case 16: desc.EchoTime = value; break; case 17: desc.EchoDepth = value; break;
    case 18: desc.ModulationTime = value; break; case 19: desc.ModulationDepth = value; break;
    case 20: desc.HFReference = value; break; case 21: desc.LFReference = value; break; default: break;
    }
}
}

void CHUNKHASHOBJ::PrintInfo(FILE* outFile) const {
    if (!outFile) return;
    std::fprintf(outFile, "%s_%s\n", zoneName.c_str(), subZoneName.c_str()); PrintDesc(desc, outFile);
}
void CHUNKHASHOBJ::DumpInfo(int summary, const char* newlyCreated) const {
    if (newlyCreated) ConsolePrintf("%s", newlyCreated);
    ConsolePrintf("[%03u]   \"%s_%s\" %s", chunkNumber, zoneName.c_str(), subZoneName.c_str(),
                  static_cast<int>(chunkNumber) == s_currentChunk_0 ? "current" : "");
    if (!summary) ConsolePrintf("Environment=%d Room=%d DecayTime=%g EnvSize=%g", desc.Environment, desc.Room, desc.DecayTime, desc.EnvSize);
}
int SndDebugListChunksINDOORS() { for (const auto& chunk : s_chunkList_0) if (chunk) chunk->DumpInfo(1, nullptr); ConsolePrintf("listed %d entries", static_cast<int>(s_chunkList_0.size())); return 1; }
int CreateChunkINDOORS() {
    auto chunk = std::make_unique<CHUNKHASHOBJ>(); chunk->chunkNumber = static_cast<unsigned>(s_chunkList_0.size());
    chunk->zoneName = "unknown"; chunk->subZoneName = "unknown"; chunk->DumpInfo(1, "created");
    s_currentChunk_0 = static_cast<int>(chunk->chunkNumber); s_chunkList_0.push_back(std::move(chunk)); return 1;
}
int __fastcall SetCurrentChunkINDOORS(const char* command, const char* value) {
    if (!value || !*value) { ConsolePrintf("%s", command ? command : "chunk"); return 1; }
    const int index = std::atoi(value); if (index >= 0 && static_cast<std::size_t>(index) < s_chunkList_0.size()) { s_currentChunk_0 = index; ConsolePrintf("Current chunk set to %d", index); }
    else ConsolePrintf("Invalid chunk %d", index); return 1;
}
int __fastcall ShowCurrentChunkINDOORS(const char* command, const char*) {
    if (s_currentChunk_0 >= 0 && static_cast<std::size_t>(s_currentChunk_0) < s_chunkList_0.size()) s_chunkList_0[s_currentChunk_0]->DumpInfo(0, nullptr);
    else ConsolePrintf("%s", command ? command : "No chunks created"); return 1;
}
int __fastcall SetChunkPropertyINDOORS(const char*, const char* value) {
    if (s_currentChunk_0 < 0 || static_cast<std::size_t>(s_currentChunk_0) >= s_chunkList_0.size()) return 1;
    unsigned property = 0; float number = 0.0F; if (value) std::sscanf(value, "%u %f", &property, &number);
    SetProperty(s_chunkList_0[s_currentChunk_0]->desc, property, number);
    SndInterfaceSetProviderPrefs(s_chunkList_0[s_currentChunk_0]->desc, s_chunkList_0[s_currentChunk_0]->desc); return 1;
}
int DumpChunksINDOORS() {
    if (s_chunkList_0.empty()) return 1;
    for (unsigned index = 0; index < s_chunkList_0.size(); ++index) { std::ofstream file("SndEAXChunkInfo_INDOORS_" + std::to_string(index) + ".txt"); (void)file; }
    return 1;
}
void IndoorsShutdown() { s_chunkList_0.clear(); s_currentChunk_0 = -1; }

int _E2_82() { return _E1_82(); } int _E1_82() { INFINITY_82 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E60_12() { _E59_10(); return _E63_5(); } void _E59_10() {} int _E63_5() { return 0; } void _E62_9() {}
int _E66_3() { _E65_5(); return 0; } void _E65_5() {} int _E69_5() { _E68_4(); return 0; } void _E68_4() {}
int _E72_5() { _E71_4(); return 0; } void _E71_4() {}
int _E140_2() { _E137_2(); return _E139_1(); } void _E137_2() {} int _E139_1() { return 0; } void _E138_2() {}
int _E145_0() { _E142_1(); return _E144_0(); } int _E142_1() { return 0; } int _E144_0() { return 0; } void _E143_2() { IndoorsShutdown(); }
int _E5_82() { _E4_82(); return 0; } void _E4_82() { TWO_PI_82 = 6.2831854F; } int _E8_82() { _E7_82(); return 0; } void _E7_82() { OO_TWO_PI_82 = 1.0F / TWO_PI_82; }
#define INDOOR_CONST_PAIR(thunk, body, variable, value) int thunk() { body(); return 0; } void body() { variable = value; }
INDOOR_CONST_PAIR(_E21_17, _E20_21, MAX_DUEL_DISTANCE_SQUARED_17, 100.0F) INDOOR_CONST_PAIR(_E24_20, _E23_24, MAX_TRADE_DISTANCE_SQUARED_17, 123.45679F)
INDOOR_CONST_PAIR(_E27_17, _E26_20, MAX_SHOP_DISTANCE_SQUARED_17, 30.864195F) INDOOR_CONST_PAIR(_E30_15, _E29_19, MAX_INSPECT_DISTANCE_SQUARED_17, 30.864195F)
INDOOR_CONST_PAIR(_E33_13, _E32_17, MAX_LOOT_DISTANCE_SQUARED_17, 25.0F) INDOOR_CONST_PAIR(_E36_12, _E35_18, MAX_BIND_DISTANCE_SQUARED_17, 100.0F)
INDOOR_CONST_PAIR(_E39_14, _E38_18, MAX_SITCHAIR_DISTANCE_SQUARED_17, 0.25F) INDOOR_CONST_PAIR(_E42_13, _E41_16, MAX_SITCHAIRUSE_DISTANCE_SQUARED_17, 9.0F)
INDOOR_CONST_PAIR(_E45_12, _E44_18, MAX_SHARE_DISTANCE_SQUARED_17, 10000.0F) INDOOR_CONST_PAIR(_E48_17, _E47_20, SECONDS_PER_HEARTBEAT_17, 5.0F)
INDOOR_CONST_PAIR(_E51_11, _E50_21, SPELL_FOCUS_DISTANCE_SQUARED_17, 2500.0F) INDOOR_CONST_PAIR(_E54_16, _E53_19, MAX_OBJ_INTEREST_RADIUS_SQUARED_17, 10000.0F)
#undef INDOOR_CONST_PAIR

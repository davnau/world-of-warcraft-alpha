#pragma once

#include "ENGINE/Source/Base/cdatastore.h"
#include "ENGINE/Source/Tempest/c3vector.h"

#include <cstdint>

struct SpellCast {
    std::uint64_t caster = 0;
    std::uint64_t casterUnit = 0;
    int spellID = 0;
    std::uint16_t targets = 0;
    std::uint64_t unitTarget = 0;
    std::uint64_t itemTarget = 0;
    std::uint64_t selectedTarget = 0;
    NTempest::C3Vector sourceLocation{};
    NTempest::C3Vector destLocation{};
    float destFacing = 0.0F;
    unsigned destZoneID = 0;
    unsigned castTime = 0;
    unsigned castEndTime = 0;
    int spellIndex = 0;
    unsigned spellLevel = 0;
    std::uint64_t ammoItem = 0;
    std::uint64_t reflector = 0;
    char targetString[128]{};
    int overrideRank = 0;
    std::uint16_t flags = 0;
};

void SpellPutCastTargets(SpellCast* spell, CDataStore* message);
void SpellGetCastTargets(SpellCast* spell, CDataStore* message);

extern float INFINITY_471;
extern float TWO_PI_428;
extern float OO_TWO_PI_429;

int _E2_471(); int _E1_471(); int _E5_441(); void _E4_441();
int _E8_430(); void _E7_441();

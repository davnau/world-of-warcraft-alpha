#pragma once

#include "ENGINE/Source/Base/cdatastore.h"
#include "WoW/Source/Object/movement.h"

enum UNITSTANDSTATE : unsigned { UNIT_STAND = 0, UNIT_SIT = 1, UNIT_SLEEP = 2, UNIT_NUMSTANDSTATES = 9 };

class CGUnit {
public:
    void BuildMovementUpdate(CDataStore* message);
    int StandStateValid(UNITSTANDSTATE newState) const;
    CMovementData m_move;
    unsigned standState = UNIT_STAND;
};

// Recovered symbol declarations for unit.cpp

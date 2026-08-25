#include "WoW/Source/Object/unit.h"

#include <array>

float INFINITY_467 = 0.0F;
float TWO_PI_424 = 6.2831853071795864769F;
float OO_TWO_PI_425 = 0.15915494309189535F;

void CGUnit::BuildMovementUpdate(CDataStore* message) {
    if (!message) return;
    const float facingValue = m_move.m_facing;
    message->Put(m_move.m_transportGUID);
    message->Put(m_move.m_position.x); message->Put(m_move.m_position.y); message->Put(m_move.m_position.z);
    message->Put(m_move.m_facing);
    message->Put(m_move.m_position.x); message->Put(m_move.m_position.y); message->Put(m_move.m_position.z);
    message->Put(facingValue);
    message->Put(m_move.m_pitch);
    message->Put(m_move.m_moveFlags & 0xFAFF0BFFu);
}

int CGUnit::StandStateValid(UNITSTANDSTATE newState) const {
    if (standState >= UNIT_NUMSTANDSTATES || newState >= UNIT_NUMSTANDSTATES)
        return 0;
    static constexpr std::array<unsigned char, UNIT_NUMSTANDSTATES * UNIT_NUMSTANDSTATES> transitions = [] {
        std::array<unsigned char, UNIT_NUMSTANDSTATES * UNIT_NUMSTANDSTATES> result{};
        for (unsigned i = 0; i < UNIT_NUMSTANDSTATES; ++i)
            for (unsigned j = 0; j < UNIT_NUMSTANDSTATES; ++j)
                result[i * UNIT_NUMSTANDSTATES + j] = static_cast<unsigned char>(i == j || j == UNIT_SIT);
        return result;
    }();
    return transitions[standState * UNIT_NUMSTANDSTATES + newState];
}

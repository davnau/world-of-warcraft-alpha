#include "WoW/Source/Game/GameClient/violencelevels.h"

#include "WoW/Source/Console/consolevar.h"

#include <algorithm>

namespace {
CVar* s_violenceLevel = nullptr;
}

VIOLENCELEVELS s_maxViolenceLevels[8] = {
    VIOLENCELEVEL_HIGH, VIOLENCELEVEL_HIGH, VIOLENCELEVEL_HIGH, VIOLENCELEVEL_HIGH,
    VIOLENCELEVEL_HIGH, VIOLENCELEVEL_HIGH, VIOLENCELEVEL_HIGH, VIOLENCELEVEL_HIGH,
};

void ViolenceLevelsShutdown() {
    s_violenceLevel = nullptr;
}

void ViolenceLevelsInitialize() {
    s_violenceLevel = CVar::Register("violenceLevel", "Sets the violence level", 0,
                                     "2", nullptr, 0, 0, nullptr);
}

VIOLENCELEVELS ViolenceGetLevel() {
    const int current = s_violenceLevel != nullptr ? s_violenceLevel->IntValue() : 2;
    const VIOLENCELEVELS maximum = s_maxViolenceLevels[0];
    const int clamped = std::clamp(current, static_cast<int>(VIOLENCELEVEL_NONE),
                                   static_cast<int>(maximum));
    return static_cast<VIOLENCELEVELS>(clamped);
}

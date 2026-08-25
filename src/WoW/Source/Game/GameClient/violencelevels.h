#pragma once

enum VIOLENCELEVELS : int {
    VIOLENCELEVEL_NONE = 0,
    VIOLENCELEVEL_MEDIUM = 1,
    VIOLENCELEVEL_HIGH = 2,
    VIOLENCELEVEL_NUMVIOLENCELEVELS = 3,
};

void ViolenceLevelsShutdown();
void ViolenceLevelsInitialize();
VIOLENCELEVELS ViolenceGetLevel();

extern VIOLENCELEVELS s_maxViolenceLevels[8];

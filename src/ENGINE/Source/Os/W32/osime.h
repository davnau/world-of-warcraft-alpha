#pragma once
#include "Storm/h/stpl.h"
enum OS_IME_LANGUAGEMODE{OS_IME_MODE_ROMAN=0,OS_IME_MODE_JAPANESE=1,OS_IME_MODE_KOREAN=2,OS_IME_MODE_CHINESE=3};struct OsIMECandidate{char candidate[1024];};
OS_IME_LANGUAGEMODE OsIMEGetLanguageMode();int OsIMEGetCompositionString(char*,unsigned int);int OsIMEGetCompositionResult(char*,unsigned int);int OsIMEGetClauseInfo(unsigned int*,unsigned int*,unsigned int*);int OsIMEGetCandidates(unsigned int*,unsigned int*,unsigned int*,unsigned int*,TSGrowableArray<OsIMECandidate>*);void OsIMEEnable(int);void OsIMEInitialize();void OsIMEDestroy();

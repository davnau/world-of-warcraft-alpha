#include "WoW/Source/WorldClient/mapbaseobj.h"

CMapBaseObj::CMapBaseObj() : initialized(true), lightSelectionCount(0) { }
void CMapBaseObj::SelectLights() { ++lightSelectionCount; }
void CMapBaseObj::TestAABox() { initialized = true; }
CMapBaseObj::~CMapBaseObj() { initialized = false; }

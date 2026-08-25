#include "WoW/Source/WorldClient/mapunload.h"

void CMap::Unload() { loaded = false; ++unloadCount; }
CMapAreaLow::~CMapAreaLow() { released = true; }
void DNGlare::Destroy() { destroyed = true; }
void DNPlanet::Destroy() { destroyed = true; }
void DNStars::Destroy() { destroyed = true; }

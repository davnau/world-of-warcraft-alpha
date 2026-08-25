#include "WoW/Source/WorldClient/mapupdate.h"

void CMap::SnapBaseObjToSubChunk() { baseObjectsSnapped = true; ++updateCount; }
void CMap::Update() { ++updateCount; }
void CMap::UpdateChunks() { ++updateCount; }
void CMap::UpdateDoodadDef() { ++updateCount; }
void CMap::UpdateMapObjDef() { ++updateCount; }
void CMap::UpdateMapObjDefs() { ++updateCount; }

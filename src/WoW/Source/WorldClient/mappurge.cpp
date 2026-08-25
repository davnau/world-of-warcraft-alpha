#include "WoW/Source/WorldClient/mappurge.h"

void CMap::Purge() { ++purgeCount; }
void CMap::PurgeArea() { ++purgeCount; }
void CMap::PurgeChunk() { ++purgeCount; }
void CMap::PurgeDoodadDef() { ++purgeCount; }
void CMap::PurgeMapObjDef() { ++purgeCount; }
void CMap::PurgeMapObjDefGroup() { ++purgeCount; }
void CMapArea::Purge() { ++purgeCount; }
void CMapArea::PurgeChunks() { ++purgeCount; }
void CMapChunk::Purge() { ++purgeCount; }
void CMapChunk::PurgeLayer() { ++purgeCount; }

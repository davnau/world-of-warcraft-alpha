#include "WoW/Source/WorldClient/mapobjupdate.h"

void CMapObjGroup::CreateLightmaps() { lightmapsCreated = true; ++lightmapRevision; }
void CMapObjGroup::FreeLightmaps() { lightmapsCreated = false; ++lightmapRevision; }
void CMapObjGroup::UpdateLightmapTex() { if (lightmapsCreated) ++lightmapRevision; }

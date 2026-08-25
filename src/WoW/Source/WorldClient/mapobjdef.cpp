#include "WoW/Source/WorldClient/mapobjdef.h"

CMapObjDef::CMapObjDef() : initialized(true) { }
CMapObjDef::~CMapObjDef() { initialized = false; }
CMapObjDefGroup::CMapObjDefGroup() : lightsSelected(false), updateCount(0) { }
void CMapObjDefGroup::SelectLights() { lightsSelected = true; ++updateCount; }
void CMapObjDefGroup::Update() { ++updateCount; }
void CMapObjDefGroup::UpdateLights() { lightsSelected = true; ++updateCount; }
CMapObjDefGroup::~CMapObjDefGroup() { lightsSelected = false; }

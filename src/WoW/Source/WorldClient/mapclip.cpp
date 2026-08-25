#include "WoW/Source/WorldClient/mapclip.h"

void CMapChunk::UpdateClipBuffer() { clipBufferValid = true; ++clipRevision; }

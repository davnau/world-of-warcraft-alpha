#include "WoW/Source/DB/DBClient/sdbitemsubclass.h"

#include <array>
#include <vector>

namespace {
std::array<std::vector<const ItemSubClassRec*>, 16> g_itemSubClassLists;
}

void SDBItemSubclassInitialize() {
    for (auto& list : g_itemSubClassLists) list.clear();
}

void SDBItemSubclassDestroy() {
    for (auto& list : g_itemSubClassLists) list.clear();
}

const ItemSubClassRec* __fastcall SDBItemSubclassGetSubClassRec(unsigned subClassID, unsigned classID) {
    if (classID >= g_itemSubClassLists.size()) return nullptr;
    const auto& list = g_itemSubClassLists[classID];
    return subClassID < list.size() ? list[subClassID] : nullptr;
}

unsigned* _E57_50() { return nullptr; }
int _E59_21() { return 0; }
int _E60_58() { _E57_50(); return _E59_21(); }
void _E58_57() { SDBItemSubclassDestroy(); }


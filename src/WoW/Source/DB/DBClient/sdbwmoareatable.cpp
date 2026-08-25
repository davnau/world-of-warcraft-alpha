#include "WoW/Source/DB/DBClient/sdbwmoareatable.h"

#include <algorithm>
#include <string>
#include <vector>

namespace {
std::vector<WMOAreaTableRec> g_wmoRecords;
char g_emptyText[] = "";
char* g_emptyName = g_emptyText;
}

int __cdecl bscompare(const WMOAreaTableRec* left, const WMOAreaTableRec* right) {
    if (left->m_WMOID != right->m_WMOID) return left->m_WMOID - right->m_WMOID;
    if (left->m_NameSetID != right->m_NameSetID) return left->m_NameSetID - right->m_NameSetID;
    return left->m_WMOGroupID - right->m_WMOGroupID;
}

bool __fastcall SDBWMOAreaTableLookup(int nameSetID, int languageID, int wmoGroupID,
                                      const WMOAreaTableRec** record) {
    WMOAreaTableRec key;
    key.m_WMOID = nameSetID;
    key.m_NameSetID = languageID;
    key.m_WMOGroupID = wmoGroupID;
    const auto it = std::lower_bound(g_wmoRecords.begin(), g_wmoRecords.end(), key,
                                     [](const auto& left, const auto& right) { return bscompare(&left, &right) < 0; });
    const bool found = it != g_wmoRecords.end() && bscompare(&*it, &key) == 0;
    if (record) *record = found ? &*it : nullptr;
    return found;
}

char** __fastcall SDBWMOAreaTableLookup(int nameSetID, int languageID, int wmoGroupID) {
    const WMOAreaTableRec* record = nullptr;
    if (!SDBWMOAreaTableLookup(nameSetID, languageID, wmoGroupID, &record) || !record)
        return &g_emptyName;
    return const_cast<char**>(&record->m_Name);
}

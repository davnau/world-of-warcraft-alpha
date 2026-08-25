#include "WoW/Source/Object/objectclient/iuniteffects.h"

#include "WoW/Source/DB/DBClient/dbclient.h"

#include <algorithm>

std::array<int, 43> g_specialSpellIDs{};

void LoadUnitDefs() {
    g_specialSpellIDs.fill(-1);
    for (const auto& record : g_spellVisualEffectNameDB.m_records) {
        if (record.m_specialID >= 0 &&
            record.m_specialID < static_cast<int>(g_specialSpellIDs.size())) {
            g_specialSpellIDs[record.m_specialID] = record.m_ID;
        }
    }
}

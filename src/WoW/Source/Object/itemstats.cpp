#include "WoW/Source/Object/itemstats.h"

#include <algorithm>
#include <cstring>

namespace {

template <typename T>
void PutArray(CDataStore* msg, const T& values) {
    for (const auto& value : values) msg->Put(value);
}

template <typename T>
void GetArray(CDataStore* msg, T& values) {
    for (auto& value : values) msg->Get(&value);
}

template <typename T>
void PutStrings(CDataStore* msg, const T& values) {
    for (const auto& value : values) msg->PutString(value.c_str());
}

template <typename T>
void GetStrings(CDataStore* msg, T& values) {
    char buffer[256]{};
    for (std::size_t i = 0; i < values.size(); ++i) {
        msg->GetString(buffer, sizeof(buffer));
        if (buffer[0] || i == 0) values[i] = buffer;
        else values[i] = values[i - 1];
    }
}

template <typename T>
void PutCommon(const T& stats, CDataStore* msg) {
    msg->Put(stats.m_class);
    msg->Put(stats.m_subclass);
    PutStrings(msg, stats.m_displayName);
    msg->Put(stats.m_displayInfoID);
    msg->Put(stats.m_overallQualityID);
    msg->Put(stats.m_flags);
    msg->Put(stats.m_buyPrice);
    msg->Put(stats.m_sellPrice);
    msg->Put(stats.m_inventoryType);
    msg->Put(stats.m_allowableClass);
    msg->Put(stats.m_allowableRace);
    msg->Put(stats.m_itemLevel);
    msg->Put(stats.m_requiredLevel);
    msg->Put(stats.m_requiredSkill);
    msg->Put(stats.m_requiredSkillRank);
    msg->Put(stats.m_maxCount);
    msg->Put(stats.m_stackable);
    msg->Put(stats.m_containerSlots);
    PutArray(msg, stats.m_bonusAmount);
    PutArray(msg, stats.m_maxDamage);
    PutArray(msg, stats.m_resistances);
    msg->Put(stats.m_delay);
    msg->Put(stats.m_ammunitionType);
    msg->Put(stats.m_maxDurability);
    PutArray(msg, stats.m_spellTrigger);
    msg->Put(stats.m_bonding);
    msg->PutString(stats.m_description.empty() ? "" : stats.m_description.c_str());
    msg->Put(stats.m_pageText);
    msg->Put(stats.m_languageID);
    msg->Put(stats.m_pageMaterial);
    msg->Put(stats.m_startQuestID);
    msg->Put(stats.m_lockID);
    msg->Put(stats.m_material);
    msg->Put(stats.m_sheatheType);
}

template <typename T>
void GetCommon(T& stats, CDataStore* msg) {
    msg->Get(&stats.m_class);
    msg->Get(&stats.m_subclass);
    GetStrings(msg, stats.m_displayName);
    msg->Get(&stats.m_displayInfoID);
    msg->Get(&stats.m_overallQualityID);
    msg->Get(&stats.m_flags);
    msg->Get(&stats.m_buyPrice);
    msg->Get(&stats.m_sellPrice);
    msg->Get(&stats.m_inventoryType);
    msg->Get(&stats.m_allowableClass);
    msg->Get(&stats.m_allowableRace);
    msg->Get(&stats.m_itemLevel);
    msg->Get(&stats.m_requiredLevel);
    msg->Get(&stats.m_requiredSkill);
    msg->Get(&stats.m_requiredSkillRank);
    msg->Get(&stats.m_maxCount);
    msg->Get(&stats.m_stackable);
    msg->Get(&stats.m_containerSlots);
    GetArray(msg, stats.m_bonusAmount);
    GetArray(msg, stats.m_maxDamage);
    GetArray(msg, stats.m_resistances);
    msg->Get(&stats.m_delay);
    msg->Get(&stats.m_ammunitionType);
    msg->Get(&stats.m_maxDurability);
    GetArray(msg, stats.m_spellTrigger);
    msg->Get(&stats.m_bonding);
    char description[128]{};
    msg->GetString(description, sizeof(description));
    stats.m_description = description;
    msg->Get(&stats.m_pageText);
    msg->Get(&stats.m_languageID);
    msg->Get(&stats.m_pageMaterial);
    msg->Get(&stats.m_startQuestID);
    msg->Get(&stats.m_lockID);
    msg->Get(&stats.m_material);
    msg->Get(&stats.m_sheatheType);
}

} // namespace

void ItemStats::Pack(CDataStore* msg) const {
    if (msg) PutCommon(*this, msg);
}

void ItemStats_C::Unpack(CDataStore* msg) {
    if (!msg) return;
    ItemStats unpacked;
    GetCommon(unpacked, msg);
    m_class = unpacked.m_class;
    m_subclass = unpacked.m_subclass;
    for (std::size_t i = 0; i < 4; ++i)
        std::strncpy(m_displayName[i], unpacked.m_displayName[i].c_str(),
                     sizeof(m_displayName[i]) - 1);
    m_displayInfoID = unpacked.m_displayInfoID;
    m_overallQualityID = unpacked.m_overallQualityID;
    m_flags = unpacked.m_flags;
    m_buyPrice = unpacked.m_buyPrice;
    m_sellPrice = unpacked.m_sellPrice;
    m_inventoryType = unpacked.m_inventoryType;
    m_allowableClass = unpacked.m_allowableClass;
    m_allowableRace = unpacked.m_allowableRace;
    m_itemLevel = unpacked.m_itemLevel;
    m_requiredLevel = unpacked.m_requiredLevel;
    m_requiredSkill = unpacked.m_requiredSkill;
    m_requiredSkillRank = unpacked.m_requiredSkillRank;
    m_maxCount = unpacked.m_maxCount;
    m_stackable = unpacked.m_stackable;
    m_containerSlots = unpacked.m_containerSlots;
    std::copy(unpacked.m_bonusAmount.begin(), unpacked.m_bonusAmount.end(), m_bonusAmount);
    std::copy(unpacked.m_maxDamage.begin(), unpacked.m_maxDamage.end(), m_maxDamage);
    std::copy(unpacked.m_resistances.begin(), unpacked.m_resistances.end(), m_resistances);
    m_delay = unpacked.m_delay;
    m_ammunitionType = unpacked.m_ammunitionType;
    m_maxDurability = unpacked.m_maxDurability;
    std::copy(unpacked.m_spellTrigger.begin(), unpacked.m_spellTrigger.end(), m_spellTrigger);
    m_bonding = unpacked.m_bonding;
    std::strncpy(m_description, unpacked.m_description.c_str(), sizeof(m_description) - 1);
    m_pageText = unpacked.m_pageText;
    m_languageID = unpacked.m_languageID;
    m_pageMaterial = unpacked.m_pageMaterial;
    m_startQuestID = unpacked.m_startQuestID;
    m_lockID = unpacked.m_lockID;
    m_material = unpacked.m_material;
    m_sheatheType = unpacked.m_sheatheType;
}

#pragma once

#include "ENGINE/Source/Base/cdatastore.h"
#include "WoW/Source/DB/DBClient/dbcache.h"

#include <array>
#include <string>

class ItemStats {
public:
    void Pack(CDataStore* msg) const;

    int m_class = 0;
    int m_subclass = 0;
    std::array<std::string, 4> m_displayName;
    int m_displayInfoID = 0;
    int m_overallQualityID = 0;
    int m_flags = 0;
    int m_buyPrice = 0;
    int m_sellPrice = 0;
    int m_inventoryType = 0;
    int m_allowableClass = 0;
    int m_allowableRace = 0;
    int m_itemLevel = 0;
    int m_requiredLevel = 0;
    int m_requiredSkill = 0;
    int m_requiredSkillRank = 0;
    int m_maxCount = 0;
    int m_stackable = 0;
    int m_containerSlots = 0;
    std::array<int, 20> m_bonusAmount{};
    std::array<int, 15> m_maxDamage{};
    std::array<int, 6> m_resistances{};
    int m_delay = 0;
    int m_ammunitionType = 0;
    int m_maxDurability = 0;
    std::array<int, 30> m_spellTrigger{};
    int m_bonding = 0;
    std::string m_description;
    int m_pageText = 0;
    int m_languageID = 0;
    int m_pageMaterial = 0;
    int m_startQuestID = 0;
    int m_lockID = 0;
    int m_material = 0;
    int m_sheatheType = 0;
};


// Recovered symbol declarations for itemstats.cpp

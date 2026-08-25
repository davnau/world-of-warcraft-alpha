#pragma once

#include "ENGINE/Source/Base/cdatastore.h"

#include <cstdint>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

struct HASHKEY_INT {};
struct CHashKeyGUID {};

struct CreatureStats_C {
    int m_id = 0;
    char m_name[4][256]{};
    char m_title[256]{};
    int m_flags = 0;
    int m_creatureType = 0;
    int m_creatureFamily = 0;
    void Unpack(CDataStore* message);
};
struct GameObjectStats_C {
    int m_id = 0;
    int m_typeID = 0;
    int m_displayID = 0;
    char m_name[4][256]{};
    int m_propValue[10]{};
    void Unpack(CDataStore* message);
};
struct ItemStats_C {
    int m_id = 0;
    int m_class = 0;
    int m_subclass = 0;
    char m_displayName[4][256]{};
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
    int m_bonusAmount[20]{};
    int m_maxDamage[15]{};
    int m_resistances[6]{};
    int m_delay = 0;
    int m_ammunitionType = 0;
    int m_maxDurability = 0;
    int m_spellTrigger[30]{};
    int m_bonding = 0;
    char m_description[128]{};
    int m_pageText = 0;
    int m_languageID = 0;
    int m_pageMaterial = 0;
    int m_startQuestID = 0;
    int m_lockID = 0;
    int m_material = 0;
    int m_sheatheType = 0;
    void Unpack(CDataStore* message);
};
struct NPCText {
    int m_id = 0;
    char m_text[1024]{};
    int m_soundID = 0;
    void Pack(CDataStore* message) const;
    void Unpack(CDataStore* message);
};
struct GuildStats_C {
    int m_guildID = 0;
    char m_guildName[64]{};
    int m_emblemStyle = -1;
    int m_emblemColor = -1;
    int m_borderStyle = -1;
    int m_borderColor = -1;
    int m_backgroundColor = -1;
    void Unpack(CDataStore* message);
};
struct QuestCache {
    int m_questId = 0;
    int m_questType = 0;
    int m_questLevel = 0;
    int m_questSortID = 0;
    int m_questInfoID = 0;
    int m_rewardNextQuest = 0;
    int m_rewardMoney = 0;
    int m_startItem = 0;
    int m_rewardItems[4]{};
    int m_rewardAmount[4]{};
    int m_rewardChoiceItems[6]{};
    int m_rewardChoiceAmount[6]{};
    int m_POIContinent = 0;
    int m_POIx = 0;
    int m_POIy = 0;
    int m_POIPriority = 0;
    char m_logTitle[256]{};
    char m_logDescription[1024]{};
    char m_questDescription[2048]{};
    char m_areaDescription[256]{};
    int m_monsterToKill[4]{};
    int m_monsterToKillQuantity[4]{};
    int m_monsterToKillCurrency[4]{};
    int m_monsterToKillItem[4]{};
    char m_getDescription[4][64]{};
    void Pack(CDataStore* message);
    void Unpack(CDataStore* message);
};
struct PageTextCache_C {
    int m_id = 0;
    char m_text[500]{};
    int m_nextPage = 0;
    void Unpack(CDataStore* message);
};
struct PetNameCache {
    int m_ID = 0;
    char m_name[48]{};
    std::uint32_t m_timestamp = 0;
    void Pack(CDataStore* message);
    void Unpack(CDataStore* message);
};
struct CGPetition {
    int m_petitionID = 0;
    std::uint64_t m_petitioner = 0;
    char m_title[256]{};
    char m_bodyText[4096]{};
    int m_flags = 0;
    int m_minSignatures = 0;
    int m_maxSignatures = 0;
    int m_deadLine = 0;
    int m_issueDate = 0;
    int m_allowedGuildID = 0;
    int m_allowedClasses = 0;
    int m_allowedRaces = 0;
    std::int16_t m_allowedGender = 0;
    int m_allowedMinLevel = 0;
    int m_allowedMaxLevel = 0;
    char m_choicetext[10][64]{};
    int m_numChoices = 0;
    unsigned int m_muid = 0;
    void Pack(CDataStore* message) const;
    void Unpack(CDataStore* message);
};
struct DBCACHECALLBACK { virtual ~DBCACHECALLBACK() = default; };
struct NameCache {
    std::uint64_t m_guid = 0;
    char m_name[48]{};
    std::uint8_t m_race = 0;
    std::uint8_t m_sex = 0;
    std::uint8_t m_class = 0;
    std::uint32_t m_temp = 0;
    void Pack(CDataStore* message);
    void Unpack(CDataStore* message);
};

template<class Record, class Key = int>
class DBCacheStore {
public:
    Record* GetRecord(const Key& key) {
        const auto it = m_records.find(key);
        return it == m_records.end() ? nullptr : &it->second;
    }

    const Record* GetRecord(const Key& key) const {
        const auto it = m_records.find(key);
        return it == m_records.end() ? nullptr : &it->second;
    }

    void AddItem(const Record& record, const Key& key) {
        m_denied.erase(key);
        m_records[key] = record;
    }

    void AddItems(CDataStore* message, bool single) {
        if (!message) return;
        std::uint8_t count = 1U;
        if (!single) {
            message->Get(&count);
            if (message->Failed() || count == 0U) return;
        }

        for (std::uint8_t index = 0U; index < count; ++index) {
            Key key{};
            message->Get(&key);
            if (message->Failed()) return;

            bool invalid = false;
            if constexpr (std::is_integral_v<Key> && sizeof(Key) == 4U) {
                using UnsignedKey = std::make_unsigned_t<Key>;
                UnsignedKey raw = static_cast<UnsignedKey>(key);
                invalid = (raw & 0x80000000U) != 0U;
                raw &= 0x7FFFFFFFU;
                key = static_cast<Key>(raw);
            }
            if (invalid) {
                DenyItem(key);
                continue;
            }

            Record record{};
            if constexpr (requires { record.m_id; })
                record.m_id = static_cast<int>(key);
            if constexpr (requires(Record& value, CDataStore* data) {
                              value.Unpack(data);
                          }) {
                record.Unpack(message);
            }
            if (message->Failed()) return;
            AddItem(record, key);
        }
    }

    void DenyItem(const Key& key) {
        m_records.erase(key);
        m_denied.insert(key);
    }

    void CancelCallback(const Key&) {}
    void SetTemporary(const Key& key) { m_temporary.insert(key); }
    void Load() {}
    void Save() {}
    void Clear() { m_records.clear(); m_denied.clear(); m_temporary.clear(); }
    void Invalidate(const Key& key) { m_records.erase(key); }
    std::size_t Size() const { return m_records.size(); }

private:
    std::unordered_map<Key, Record> m_records;
    std::unordered_set<Key> m_denied;
    std::unordered_set<Key> m_temporary;
};

template<class Record, class Key, class HashKey>
class DBCache {
public:
    using DBCACHEHASH = Record;

    DBCache() = default;
    ~DBCache() = default;

    Record* GetRecord(const Key& key) {
        const auto it = m_records.find(key);
        return it == m_records.end() ? nullptr : &it->second;
    }

    const Record* GetRecord(const Key& key) const {
        const auto it = m_records.find(key);
        return it == m_records.end() ? nullptr : &it->second;
    }

    int VerifyPack(CDataStore*) const { return 1; }
    int VerifyCache() const { return 1; }

    void AddItem(const Record* record, const Key& key) {
        if (record) m_records[key] = *record;
        m_denied.erase(key);
    }

    void AddItems(CDataStore* message, int) {
        if (!message) return;
        Key key{};
        if (!message->Get(&key)) return;
        Record record{};
        if constexpr (requires { record.m_id; }) record.m_id = static_cast<int>(key);
        AddItem(&record, key);
    }

    void DenyItem(const Key& key) {
        m_records.erase(key);
        m_denied.insert(key);
    }

    void CancelCallback(const Key&) {}
    void Load(unsigned = 0) {}
    void Save() {}
    void Clear() { m_records.clear(); m_denied.clear(); m_temporary.clear(); }
    void Invalidate(const Key& key) { m_records.erase(key); }
    void SetTemporary(const Key& key) { m_temporary.insert(key); }

private:
    std::unordered_map<Key, Record> m_records;
    std::unordered_set<Key> m_denied;
    std::unordered_set<Key> m_temporary;
};

extern DBCacheStore<CreatureStats_C> g_creatureDBCache;
extern DBCacheStore<GameObjectStats_C> g_gameObjectDBCache;
extern DBCacheStore<ItemStats_C> g_itemDBCache;
extern DBCacheStore<NPCText> g_npcTextDBCache;
extern DBCacheStore<NameCache, std::uint64_t> g_nameDBCache;
extern DBCacheStore<GuildStats_C> g_guildInfoCache;
extern DBCacheStore<QuestCache> g_questDBCache;
extern DBCacheStore<PageTextCache_C> g_pageTextCache;
extern DBCacheStore<PetNameCache> g_petNameCache;
extern DBCacheStore<CGPetition> g_petitionCache;

#include "WoW/Source/DB/DBClient/dbcacheinstances.h"
#include "WoW/Source/UI/actionbarframe.h"
#include "WoW/Source/UI/containerframe.h"
#include "WoW/Source/WowSvcs/wowsvcsclient/clientservices.h"

#include <cstring>

namespace {
constexpr NETMESSAGE SMSG_NAME_QUERY_RESPONSE = 0x0051;
constexpr NETMESSAGE SMSG_PET_NAME_QUERY_RESPONSE = 0x0053;
constexpr NETMESSAGE SMSG_GUILD_QUERY_RESPONSE = 0x0055;
constexpr NETMESSAGE SMSG_ITEM_QUERY_SINGLE_RESPONSE = 0x0058;
constexpr NETMESSAGE SMSG_ITEM_QUERY_MULTIPLE_RESPONSE = 0x0059;
constexpr NETMESSAGE SMSG_PAGE_TEXT_QUERY_RESPONSE = 0x005B;
constexpr NETMESSAGE SMSG_QUEST_QUERY_RESPONSE = 0x005D;
constexpr NETMESSAGE SMSG_GAMEOBJECT_QUERY_RESPONSE = 0x005F;
constexpr NETMESSAGE SMSG_CREATURE_QUERY_RESPONSE = 0x0061;
constexpr NETMESSAGE SMSG_NPC_TEXT_UPDATE = 0x0179;
constexpr NETMESSAGE SMSG_PETITION_QUERY_RESPONSE = 0x01B8;

#define DB_CACHE_HANDLER(Name, Receiver)                                      \
    int __fastcall Name(void*, NETMESSAGE, unsigned flags, CDataStore* msg) { \
        return Receiver(flags, msg);                                          \
    }
DB_CACHE_HANDLER(ReceiveCreatureHandler, ReceiveCreature)
DB_CACHE_HANDLER(ReceiveGameObjectHandler, ReceiveGameObject)
DB_CACHE_HANDLER(ReceiveSingleItemHandler, ReceiveSingleItem)
DB_CACHE_HANDLER(ReceiveMultipleItemsHandler, ReceiveMultipleItems)
DB_CACHE_HANDLER(ReceiveNPCTextHandler, ReceiveNPCText)
DB_CACHE_HANDLER(ReceiveNameHandler, ReceiveName)
DB_CACHE_HANDLER(ReceiveGuildInfoHandler, ReceiveGuildInfo)
DB_CACHE_HANDLER(ReceiveQuestHandler, ReceiveQuest)
DB_CACHE_HANDLER(ReceivePageTextHandler, ReceivePageText)
DB_CACHE_HANDLER(ReceivePetNameHandler, ReceivePetName)
DB_CACHE_HANDLER(ReceivePetitionHandler, ReceivePetition)
#undef DB_CACHE_HANDLER
}

DBCacheStore<CreatureStats_C> g_creatureDBCache;
DBCacheStore<GameObjectStats_C> g_gameObjectDBCache;
DBCacheStore<ItemStats_C> g_itemDBCache;
DBCacheStore<NPCText> g_npcTextDBCache;
DBCacheStore<NameCache, std::uint64_t> g_nameDBCache;
DBCacheStore<GuildStats_C> g_guildInfoCache;
DBCacheStore<QuestCache> g_questDBCache;
DBCacheStore<PageTextCache_C> g_pageTextCache;
DBCacheStore<PetNameCache> g_petNameCache;
DBCacheStore<CGPetition> g_petitionCache;

void LoadDBCaches(unsigned) {
    g_creatureDBCache.Load();
    g_gameObjectDBCache.Load();
    g_itemDBCache.Load();
    g_npcTextDBCache.Load();
    g_nameDBCache.Load();
    g_guildInfoCache.Load();
    g_questDBCache.Load();
    g_pageTextCache.Load();
    g_petNameCache.Load();
    g_petitionCache.Load();
}

void DBCache_Initialize() { LoadDBCaches(0); }
void DBCache_Destroy() {}
void DBCache_RegisterHandlers(NETMESSAGE) {
    ClientServices_SetMessageHandler(SMSG_CREATURE_QUERY_RESPONSE,
                                     ReceiveCreatureHandler);
    ClientServices_SetMessageHandler(SMSG_GAMEOBJECT_QUERY_RESPONSE,
                                     ReceiveGameObjectHandler);
    ClientServices_SetMessageHandler(SMSG_ITEM_QUERY_SINGLE_RESPONSE,
                                     ReceiveSingleItemHandler);
    ClientServices_SetMessageHandler(SMSG_ITEM_QUERY_MULTIPLE_RESPONSE,
                                     ReceiveMultipleItemsHandler);
    ClientServices_SetMessageHandler(SMSG_NPC_TEXT_UPDATE, ReceiveNPCTextHandler);
    ClientServices_SetMessageHandler(SMSG_NAME_QUERY_RESPONSE, ReceiveNameHandler);
    ClientServices_SetMessageHandler(SMSG_GUILD_QUERY_RESPONSE,
                                     ReceiveGuildInfoHandler);
    ClientServices_SetMessageHandler(SMSG_QUEST_QUERY_RESPONSE, ReceiveQuestHandler);
    ClientServices_SetMessageHandler(SMSG_PAGE_TEXT_QUERY_RESPONSE,
                                     ReceivePageTextHandler);
    ClientServices_SetMessageHandler(SMSG_PET_NAME_QUERY_RESPONSE,
                                     ReceivePetNameHandler);
    ClientServices_SetMessageHandler(SMSG_PETITION_QUERY_RESPONSE,
                                     ReceivePetitionHandler);
}

int __stdcall ReceiveCreature(unsigned, CDataStore* message) { g_creatureDBCache.AddItems(message, true); return 1; }
int __stdcall ReceiveGameObject(unsigned, CDataStore* message) { g_gameObjectDBCache.AddItems(message, true); return 1; }
int __stdcall ReceiveSingleItem(unsigned, CDataStore* message) {
    if (!message) return 0;
    const unsigned savedRead = message->m_read;
    int itemID = 0;
    message->Get(&itemID);
    message->m_read = savedRead;
    g_itemDBCache.AddItems(message, true);
    if (!message->Failed() && itemID > 0) {
        CGActionBar::UpdateItem(itemID);
        CGContainerInfo::UpdateItem();
    }
    return message->Failed() ? 0 : 1;
}
int __stdcall ReceiveMultipleItems(unsigned, CDataStore* message) {
    g_itemDBCache.AddItems(message, false);
    if (!message || message->Failed()) return 0;
    CGActionBar::RefreshItemActions();
    CGContainerInfo::UpdateItem();
    return 1;
}
int __stdcall ReceiveNPCText(unsigned, CDataStore* message) { g_npcTextDBCache.AddItems(message, true); return 1; }

int __stdcall ReceiveName(unsigned, CDataStore* message) {
    NameCache record{};
    if (!message || !message->Get(&record.m_guid)) return 1;
    message->GetString(record.m_name, sizeof(record.m_name));
    message->Get(&record.m_race);
    message->Get(&record.m_sex);
    message->Get(&record.m_class);
    if (record.m_name[0]) {
        g_nameDBCache.AddItem(record, record.m_guid);
        if ((record.m_guid >> 32 & 0xF0000000u) == 0x90000000u) g_nameDBCache.SetTemporary(record.m_guid);
    } else {
        g_nameDBCache.DenyItem(record.m_guid);
    }
    return 1;
}

int __stdcall ReceiveGuildInfo(unsigned, CDataStore* message) {
    GuildStats_C record{};
    if (message) message->Get(&record.m_guildID);
    if (record.m_guildID) g_guildInfoCache.AddItem(record, record.m_guildID);
    else g_guildInfoCache.DenyItem(record.m_guildID);
    return 1;
}

int __stdcall ReceiveQuest(unsigned, CDataStore* message) {
    QuestCache record{};
    if (message) message->Get(&record.m_questId);
    if (record.m_questId) g_questDBCache.AddItem(record, record.m_questId);
    else g_questDBCache.DenyItem(record.m_questId);
    return 1;
}

int __stdcall ReceivePageText(unsigned, CDataStore* message) { g_pageTextCache.AddItems(message, true); return 1; }

int __stdcall ReceivePetName(unsigned, CDataStore* message) {
    PetNameCache record{};
    if (!message || !message->Get(&record.m_ID)) return 1;
    message->GetString(record.m_name, sizeof(record.m_name));
    message->Get(&record.m_timestamp);
    if (record.m_name[0]) g_petNameCache.AddItem(record, record.m_ID);
    else g_petNameCache.DenyItem(record.m_ID);
    return 1;
}

int __stdcall ReceivePetition(unsigned, CDataStore* message) {
    CGPetition record{};
    if (message) message->Get(&record.m_petitionID);
    if (record.m_petitionID > 0) g_petitionCache.AddItem(record, record.m_petitionID);
    else g_petitionCache.DenyItem(-record.m_petitionID);
    return 1;
}

void DBCache_ClearHandlers(NETMESSAGE) {
    constexpr NETMESSAGE messages[] = {
        SMSG_CREATURE_QUERY_RESPONSE, SMSG_GAMEOBJECT_QUERY_RESPONSE,
        SMSG_ITEM_QUERY_SINGLE_RESPONSE, SMSG_ITEM_QUERY_MULTIPLE_RESPONSE,
        SMSG_NPC_TEXT_UPDATE, SMSG_NAME_QUERY_RESPONSE,
        SMSG_GUILD_QUERY_RESPONSE, SMSG_QUEST_QUERY_RESPONSE,
        SMSG_PAGE_TEXT_QUERY_RESPONSE, SMSG_PET_NAME_QUERY_RESPONSE,
        SMSG_PETITION_QUERY_RESPONSE,
    };
    for (NETMESSAGE message : messages)
        ClientServices_ClearMessageHandler(message);
}
int _E60_57() { return 0; }
int _E59_20() { return 0; }
int _E65_15() { return 0; }
int _E64_45() { return 0; }
int _E70_45() { return 0; }
int _E69_41() { return 0; }
int _E75_37() { return 0; }
int _E74_13() { return 0; }
int _E80_15() { return 0; }
int _E79_11() { return 0; }
int _E85_2() { return 0; }
int _E84_2() { return 0; }
int _E90_5() { return 0; }
int _E89_5() { return 0; }
int _E95_1() { return 0; }
int _E94_3() { return 0; }
int _E100_3() { return 0; }
int _E99_2() { return 0; }
int _E105_3() { return 0; }
int _E104_3() { return 0; }

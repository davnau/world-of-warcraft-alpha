#include "WoW/Source/Game/GameClient/namecache.h"

void NameCache::Pack(CDataStore* message) {
    if (message == nullptr) return;
    message->Put(m_guid)->PutString(m_name)->Put(m_race)->Put(m_sex)->Put(m_class)->Put(m_temp);
}

void NameCache::Unpack(CDataStore* message) {
    if (message == nullptr) return;
    message->Get(&m_guid)->GetString(m_name, sizeof(m_name))->Get(&m_race)->Get(&m_sex)->Get(&m_class)->Get(&m_temp);
}

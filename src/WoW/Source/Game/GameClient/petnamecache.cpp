#include "WoW/Source/Game/GameClient/petnamecache.h"

void PetNameCache::Pack(CDataStore* message) {
    if (message == nullptr) return;
    message->Put(m_ID)->PutString(m_name)->Put(m_timestamp);
}

void PetNameCache::Unpack(CDataStore* message) {
    if (message == nullptr) return;
    message->Get(&m_ID)->GetString(m_name, sizeof(m_name))->Get(&m_timestamp);
}

#include "WoW/Source/Object/pagetextcache.h"

void PageTextCache::Pack(CDataStore* msg) const {
    if (!msg) return;
    msg->PutString(m_text.data());
    msg->Put(m_nextPage);
}

void PageTextCache_C::Unpack(CDataStore* msg) {
    if (!msg) return;
    msg->GetString(m_text, sizeof(m_text));
    msg->Get(&m_nextPage);
}

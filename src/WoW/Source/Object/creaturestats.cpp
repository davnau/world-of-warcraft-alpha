#include "WoW/Source/Object/creaturestats.h"

#include <cstring>

void CreatureStats::Pack(CDataStore* msg) const {
    if (!msg) return;
    for (const auto& name : m_name) msg->PutString(name.c_str());
    msg->PutString(m_title.c_str());
    msg->Put(m_flags);
    msg->Put(m_creatureType);
    msg->Put(m_creatureFamily);
}

void CreatureStats_C::Unpack(CDataStore* msg) {
    if (!msg) return;
    char buffer[256]{};
    for (std::size_t i = 0; i < 4; ++i) {
        msg->GetString(buffer, sizeof(buffer));
        if (buffer[0] || i == 0) std::strncpy(m_name[i], buffer, sizeof(m_name[i]) - 1);
        else std::strncpy(m_name[i], m_name[i - 1], sizeof(m_name[i]) - 1);
    }
    msg->GetString(buffer, sizeof(buffer));
    std::strncpy(m_title, buffer, sizeof(m_title) - 1);
    msg->Get(&m_flags);
    msg->Get(&m_creatureType);
    msg->Get(&m_creatureFamily);
}

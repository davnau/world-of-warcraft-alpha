#include "WoW/Source/Object/gameobjectstats.h"

#include <cstring>

void GameObjectStats::Pack(CDataStore* msg) const {
    if (!msg) return;
    msg->Put(m_typeID);
    msg->Put(m_displayID);
    for (const auto& name : m_name) msg->PutString(name.c_str());
    for (int value : m_propValue) msg->Put(value);
}

void GameObjectStats_C::Unpack(CDataStore* msg) {
    if (!msg) return;
    msg->Get(&m_typeID);
    msg->Get(&m_displayID);
    char buffer[256]{};
    for (std::size_t i = 0; i < 4; ++i) {
        msg->GetString(buffer, sizeof(buffer));
        if (buffer[0] || i == 0) std::strncpy(m_name[i], buffer, sizeof(m_name[i]) - 1);
        else std::strncpy(m_name[i], m_name[i - 1], sizeof(m_name[i]) - 1);
    }
    for (int& value : m_propValue) msg->Get(&value);
}

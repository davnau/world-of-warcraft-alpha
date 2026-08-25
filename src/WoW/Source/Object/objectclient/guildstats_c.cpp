#include "WoW/Source/Object/objectclient/guildstats_c.h"

void GuildStats_C::Unpack(CDataStore* msg) {
    if (!msg) return;
    msg->Get(&m_guildID);
    msg->GetString(m_guildName, sizeof(m_guildName));
    msg->Get(&m_emblemStyle);
    msg->Get(&m_emblemColor);
    msg->Get(&m_borderStyle);
    msg->Get(&m_borderColor);
    msg->Get(&m_backgroundColor);
}

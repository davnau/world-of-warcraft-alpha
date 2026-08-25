#include "WoW/Source/Object/guildstats.h"

void GuildStats::Pack(CDataStore* msg) const {
    if (!msg) return;
    msg->Put(m_guildID);
    msg->PutString(m_guildName[0] ? m_guildName.data() : "");
    msg->Put(m_emblemStyle);
    msg->Put(m_emblemColor);
    msg->Put(m_borderStyle);
    msg->Put(m_borderColor);
    msg->Put(m_backgroundColor);
}

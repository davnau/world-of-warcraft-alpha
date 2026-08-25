#include "WoW/Source/Object/npctext.h"

#include <cstring>

void NPCText::Pack(CDataStore* msg) const {
    if (!msg) return;
    msg->PutString(m_text);
    msg->Put(m_soundID);
}

void NPCText::Unpack(CDataStore* msg) {
    if (!msg) return;
    char text[1024]{};
    msg->GetString(text, sizeof(text));
    std::strncpy(m_text, text, sizeof(m_text) - 1);
    msg->Get(&m_soundID);
}

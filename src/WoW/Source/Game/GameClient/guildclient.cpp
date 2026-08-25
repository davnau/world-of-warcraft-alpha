#include "WoW/Source/Game/GameClient/guildclient.h"

namespace {
int s_connectionInitializerCount = 0;
}

void _E150_4() {
    ++s_connectionInitializerCount;
}

void _E151_3() {
    if (s_connectionInitializerCount > 0) --s_connectionInitializerCount;
}

int _E152_3() {
    return 0;
}

int _E153_4() {
    _E150_4();
    return _E152_3();
}

unsigned char GuildGetGuildTabard(unsigned guildID, GuildTabardCallback callback,
                                  int& emblemStyle, int& emblemColor,
                                  int& borderStyle, int& borderColor,
                                  int& backgroundColor) {
    emblemStyle = emblemColor = borderStyle = borderColor = backgroundColor = -1;
    const GuildStats_C* record = g_guildInfoCache.GetRecord(static_cast<int>(guildID));
    if (record == nullptr || record->m_emblemStyle < 0 || record->m_emblemColor < 0 ||
        record->m_borderStyle < 0 || record->m_borderColor < 0 ||
        record->m_backgroundColor < 0) {
        (void)callback;
        return 0;
    }

    emblemStyle = record->m_emblemStyle;
    emblemColor = record->m_emblemColor;
    borderStyle = record->m_borderStyle;
    borderColor = record->m_borderColor;
    backgroundColor = record->m_backgroundColor;
    return 1;
}

unsigned GuildGetTabardCost() {
    return 100000;
}

#pragma once

#include "ENGINE/Source/Base/cdatastore.h"

#include <array>

class GuildStats {
public:
    void Pack(CDataStore* msg) const;

    unsigned int m_guildID = 0;
    std::array<char, 24> m_guildName{};
    int m_emblemStyle = 0;
    int m_emblemColor = 0;
    int m_borderStyle = 0;
    int m_borderColor = 0;
    int m_backgroundColor = 0;
};

// Recovered symbol declarations for guildstats.cpp

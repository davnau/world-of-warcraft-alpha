#pragma once

#include "ENGINE/Source/Base/cdatastore.h"
#include "WoW/Source/DB/DBClient/dbcache.h"

#include <array>
#include <string>

class CreatureStats {
public:
    void Pack(CDataStore* msg) const;

    std::array<std::string, 4> m_name;
    std::string m_title;
    int m_flags = 0;
    int m_creatureType = 0;
    int m_creatureFamily = 0;
};


// Recovered symbol declarations for creaturestats.cpp

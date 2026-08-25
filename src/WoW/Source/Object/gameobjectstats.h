#pragma once

#include "ENGINE/Source/Base/cdatastore.h"
#include "WoW/Source/DB/DBClient/dbcache.h"

#include <array>
#include <string>

class GameObjectStats {
public:
    void Pack(CDataStore* msg) const;

    int m_typeID = 0;
    int m_displayID = 0;
    std::array<std::string, 4> m_name;
    std::array<int, 10> m_propValue{};
};


// Recovered symbol declarations for gameobjectstats.cpp

#pragma once

#include "ENGINE/Source/Base/cdatastore.h"
#include "WoW/Source/DB/DBClient/dbcache.h"

#include <array>

class PageTextCache {
public:
    void Pack(CDataStore* msg) const;

    std::array<char, 500> m_text{};
    int m_nextPage = 0;
};


// Recovered symbol declarations for pagetextcache.cpp

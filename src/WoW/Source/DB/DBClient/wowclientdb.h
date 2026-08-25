#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"
#include "WoW/Source/DB/DBClient/AutoCode/areamidiambiencesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/areapoirec.h"
#include "WoW/Source/DB/DBClient/AutoCode/areatablerec.h"
#include "WoW/Source/DB/DBClient/AutoCode/areatriggerrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/attackanimkitsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/attackanimtypesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/bankbagslotpricesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/camerashakesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/charactercreatecamerasrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/characterfacialhairstylesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/charbaseinforec.h"
#include "WoW/Source/DB/DBClient/AutoCode/charhairgeosetsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/charstartoutfitrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/chartexturevariationsv2rec.h"
#include "WoW/Source/DB/DBClient/AutoCode/charvariationsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/chrclassesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/chrproficiencyrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/chrracesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/cinematiccamerarec.h"
#include "WoW/Source/DB/DBClient/AutoCode/cinematicsequencesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/creaturedisplayinfoextrarec.h"
#include "WoW/Source/DB/DBClient/AutoCode/creaturedisplayinforec.h"
#include "WoW/Source/DB/DBClient/AutoCode/creaturefamilyrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/creaturemodeldatarec.h"
#include "WoW/Source/DB/DBClient/AutoCode/creaturesounddatarec.h"
#include "WoW/Source/DB/DBClient/AutoCode/creaturetyperec.h"
#include "WoW/Source/DB/DBClient/AutoCode/deaththudlookupsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/emoteanimsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/emotesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/emotestextdatarec.h"
#include "WoW/Source/DB/DBClient/AutoCode/emotestextrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/factiongrouprec.h"
#include "WoW/Source/DB/DBClient/AutoCode/factionrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/factiontemplaterec.h"
#include "WoW/Source/DB/DBClient/AutoCode/footprinttexturesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/footstepterrainlookuprec.h"
#include "WoW/Source/DB/DBClient/AutoCode/gameobjectdisplayinforec.h"
#include "WoW/Source/DB/DBClient/AutoCode/groundeffectdoodadrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/groundeffecttexturerec.h"
#include "WoW/Source/DB/DBClient/AutoCode/helmetgeosetvisdatarec.h"
#include "WoW/Source/DB/DBClient/AutoCode/itemclassrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/itemdisplayinforec.h"
#include "WoW/Source/DB/DBClient/AutoCode/itemgroupsoundsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/itemsubclassrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/itemvisualeffectsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/itemvisualsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/languagesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/languagewordsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/lockrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/locktyperec.h"
#include "WoW/Source/DB/DBClient/AutoCode/maprec.h"
#include "WoW/Source/DB/DBClient/AutoCode/materialrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/namesprofanityrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/namesreservedrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/npcsoundsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/pagetextmaterialrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/paperdollitemframerec.h"
#include "WoW/Source/DB/DBClient/AutoCode/questinforec.h"
#include "WoW/Source/DB/DBClient/AutoCode/questsortrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/resistancesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/sheathesoundlookupsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/skilllineabilityrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/skilllinerec.h"
#include "WoW/Source/DB/DBClient/AutoCode/soundcharactermacrolinesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/soundentriesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/soundproviderpreferencesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/soundsamplepreferencesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/soundwatertyperec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spellauranamesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spellcasttimesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spellchaineffectsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spelldispeltyperec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spelldurationrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spelleffectcamerashakesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spelleffectnamesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spellfocusobjectrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spelliconrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spellitemenchantmentrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spellradiusrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spellrangerec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spellrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spellshapeshiftformrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spellvisualanimnamerec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spellvisualeffectnamerec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spellvisualkitrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spellvisualprecasttransitionsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spellvisualrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/stringlookupsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/tabardbackgroundtexturesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/tabardemblemtexturesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/taxinodesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/taxipathnoderec.h"
#include "WoW/Source/DB/DBClient/AutoCode/taxipathrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/terraintyperec.h"
#include "WoW/Source/DB/DBClient/AutoCode/terraintypesoundsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/transportanimationrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/uisoundlookupsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/unitbloodlevelsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/unitbloodrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/videohardwarerec.h"
#include "WoW/Source/DB/DBClient/AutoCode/vocaluisoundsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/weaponimpactsoundsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/weaponswingsounds2rec.h"
#include "WoW/Source/DB/DBClient/AutoCode/wmoareatablerec.h"
#include "WoW/Source/DB/DBClient/AutoCode/worldmaparearec.h"
#include "WoW/Source/DB/DBClient/AutoCode/worldmapcontinentrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/worldsafelocsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/zonemusicrec.h"

#include <algorithm>
#include <vector>

template<class Record>
class WowClientDB {
public:
    bool Load() {
        m_loaded = true;
        m_numRecords = static_cast<int>(m_records.size());
        m_maxID = m_numRecords > 0 ? m_numRecords - 1 : -1;
        m_recordsById.clear();
        m_recordsById.reserve(m_records.size());
        for (auto& record : m_records) m_recordsById.push_back(&record);
        return true;
    }

    void Clear() {
        m_records.clear();
        m_recordsById.clear();
        m_numRecords = 0;
        m_maxID = -1;
        m_loaded = false;
    }

    Record* Lookup(int id) {
        return id >= 0 && id < static_cast<int>(m_recordsById.size()) ? m_recordsById[id] : nullptr;
    }

    std::vector<Record> m_records;
    std::vector<Record*> m_recordsById;
    int m_numRecords = 0;
    int m_maxID = -1;
    bool m_loaded = false;
};

#pragma once

#include <cstdint>
#include <vector>

enum UNIT_SEX : std::int32_t {
    UNITSEX_MALE = 0,
    UNITSEX_FEMALE = 1,
    UNITSEX_NONE = 2,
    UNITSEX_LAST = 3,
    UNITSEX_BOTH = 3,
};

enum TEXCOMPONENT_SECTIONS : std::int32_t {
    TCS_UPPERARM = 0,
    TCS_LOWERARM = 1,
    TCS_HAND = 2,
    TCS_UPPERHEAD = 3,
    TCS_LOWERHEAD = 4,
    TCS_UPPERTORSO = 5,
    TCS_LOWERTORSO = 6,
    TCS_LEGUPPER = 7,
    TCS_LEGLOWER = 8,
    TCS_FEET = 9,
    NUM_TEXCOMPONENT_SECTIONS = 10,
    TCS_INVALIDSECTION = 11,
};

struct ChrRacesRec {
    int m_ID = 0;
    int m_flags = 0;
    int m_factionID = 0;
    int m_MaleDisplayId = 0;
    int m_FemaleDisplayId = 0;
    const char* m_ClientPrefix = "";
    float m_MountScale = 0.0F;
    int m_BaseLanguage = 0;
    int m_creatureType = 0;
    int m_LoginEffectSpellID = 0;
    int m_CombatStunSpellID = 0;
    int m_ResSicknessSpellID = 0;
    int m_SplashSoundID = 0;
    int m_startingTaxiNodes = 0;
    const char* m_clientFileString = "";
    int m_cinematicSequenceID = 0;
    const char* m_name_lang[8]{};
    int m_name_flag = 0;
};

// Minimal typed view of the database members used by the recovered unit.
// The full WowClientDB implementation is reconstructed in the DB folders;
// keeping this view here lets the common utility remain independently usable.
struct ChrRacesDB {
    unsigned int m_maxID = 0;
    std::vector<ChrRacesRec*> m_recordsById;

    ChrRacesRec* Record(unsigned int id) const {
        return id < m_recordsById.size() ? m_recordsById[id] : nullptr;
    }
};

extern ChrRacesDB g_chrRacesDB;
extern const char* s_sexSuffixNames[3];
extern const char* s_sectionDirectorynames[NUM_TEXCOMPONENT_SECTIONS];
extern const char* s_texComponentBasePath;

void __fastcall CompDecorateObjName(const char* fileName, char* buffer, unsigned int size,
                         unsigned int race, unsigned int sex);
void __fastcall BuildObjComponentPath(const char* fileName, unsigned int race, unsigned int sex,
                           char* buffer, unsigned int size);
void __fastcall ConstructSuffixString(int race, int sex, int includeRace, int includeSex,
                           char* buffer, unsigned int size);
int __fastcall CompDecorateUndecorateObjName(const char* fileName, char* buffer, unsigned int size);

void __fastcall CompDecorateTexName(const char* section, int sectionNumber, char* buffer,
                         unsigned int size, UNIT_SEX sex, int includeSex);
void __fastcall BuildTexComponentPath(const char* section, int sectionNumber, char* buffer,
                           unsigned int size, UNIT_SEX sex, int includeSex);
int __fastcall CompDecorateUndecorateTexName(const char* fileName, char* buffer, unsigned int size);
int __fastcall ComponentUtilImageFileExists(const char* fileName);

int _E1_255();
int _E2_255();
void _E4_252();
int _E5_252();
void _E7_252();
int _E8_252();

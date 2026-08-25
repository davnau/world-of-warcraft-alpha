#include "WoW/Source/WorldClient/World.h"

#include "ENGINE/Source/Base/coordinate.h"
#include "ENGINE/Source/Gx/gxapi/gxapi.h"
#include "ENGINE/Source/Model/model.h"
#include "ENGINE/Source/Model/modelanim.h"
#include "ENGINE/Source/Model/modelcreate.h"
#include "ENGINE/Source/Model/modelmisc.h"
#include "ENGINE/Source/Model/modelrender.h"
#include "ENGINE/Source/Os/W32/time.h"
#include "ENGINE/Source/Services/textblock.h"
#include "ENGINE/Source/Services/texture.h"
#include "ENGINE/Source/Services/particlesystem2.h"
#include "ENGINE/Source/Tempest/c2vector.h"
#include "ENGINE/Source/Tempest/c34matrix.h"
#include "ENGINE/Source/Tempest/c44matrix.h"
#include "ENGINE/Source/Tempest/c4vector.h"
#include "ENGINE/Source/Tempest/cfacet.h"
#include "ENGINE/Source/Tempest/cimvector.h"
#include "ENGINE/Source/Tempest/tempest_intersect.h"
#include "Storm/h/storm.h"
#include "WoW/common/daynight.h"
#include "WoW/Source/DB/DBClient/dbcache.h"
#include "WoW/Source/Glue/cgluemgr.h"
#include "WoW/Source/Glue/charselectinfo.h"
#include "WoW/Source/ObjectMgrClient/objectmgrclient.h"
#include "WoW/Source/Object/objectclient/object_c.h"
#include "WoW/Source/UI/gameui.h"
#include "WoW/Source/UIUtil/inputcontrol.h"
#include "WoW/Source/WorldClient/mapobj.h"
#include "WoW/Source/WowSvcs/wowsvcsclient/clientservices.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <limits>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

void FrameScript_SignalEvent(unsigned eventId);
void FrameScript_SignalEvent(unsigned eventId, const char* format, ...);

namespace {

constexpr std::uint32_t MakeToken(char a, char b, char c, char d) {
    return static_cast<std::uint8_t>(a) |
        (static_cast<std::uint32_t>(static_cast<std::uint8_t>(b)) << 8U) |
        (static_cast<std::uint32_t>(static_cast<std::uint8_t>(c)) << 16U) |
        (static_cast<std::uint32_t>(static_cast<std::uint8_t>(d)) << 24U);
}

constexpr std::uint32_t MVER = MakeToken('R', 'E', 'V', 'M');
constexpr std::uint32_t MPHD = MakeToken('D', 'H', 'P', 'M');
constexpr std::uint32_t MAIN = MakeToken('N', 'I', 'A', 'M');
constexpr std::uint32_t MHDR = MakeToken('R', 'D', 'H', 'M');
constexpr std::uint32_t MCIN = MakeToken('N', 'I', 'C', 'M');
constexpr std::uint32_t MTEX = MakeToken('X', 'E', 'T', 'M');
constexpr std::uint32_t MCNK = MakeToken('K', 'N', 'C', 'M');
constexpr std::uint32_t MCLY = MakeToken('Y', 'L', 'C', 'M');
constexpr std::uint32_t MCRF = MakeToken('F', 'R', 'C', 'M');
constexpr std::uint32_t MNDM = MakeToken('M', 'N', 'D', 'M');
constexpr std::uint32_t MNOM = MakeToken('M', 'N', 'O', 'M');
constexpr std::uint32_t MDDF = MakeToken('F', 'D', 'D', 'M');
constexpr std::uint32_t MODF = MakeToken('F', 'D', 'O', 'M');

constexpr float kMapOrigin = 17066.666F;
constexpr float kChunkSize = 33.333332F;
constexpr float kChunkToIndex = 0.03F;
constexpr unsigned kMapAreaCount = 64U * 64U;
constexpr unsigned kChunksPerArea = 16U;
constexpr unsigned kChunkVertexCount = 145U;
constexpr unsigned kSmsgDeathNotify = 0x014CU;
constexpr unsigned kAnimStand = 0U;
constexpr unsigned kAnimDeath = 1U;
constexpr unsigned kAnimDead = 6U;

struct DiskEntry {
    std::uint32_t offset = 0;
    std::uint32_t size = 0;
    std::uint32_t flags = 0;
    std::uint32_t asyncId = 0;
};

struct TerrainLayer {
    std::uint32_t textureId = 0;
    std::uint32_t properties = 0;
    std::uint32_t alphaOffset = 0;
    std::uint32_t effectId = 0;
};

struct TerrainAlphaMask {
    std::array<std::uint32_t, 64U * 64U> pixels{};
    CGxTex* texture = nullptr;
};

struct TerrainChunk {
    NTempest::C3Vector corner{};
    std::array<NTempest::C3Vector, kChunkVertexCount> vertices{};
    std::array<NTempest::C3Vector, kChunkVertexCount> normals{};
    std::array<NTempest::C2Vector, kChunkVertexCount> textureCoordinates{};
    std::array<NTempest::C2Vector, kChunkVertexCount> alphaTextureCoordinates{};
    std::array<NTempest::CImVector, kChunkVertexCount> vertexColors{};
    std::array<TerrainLayer, 4> layers{};
    std::array<std::unique_ptr<TerrainAlphaMask>, 4> alphaMasks{};
    std::vector<std::uint8_t> alpha;
    std::vector<std::uint16_t> indices;
    std::vector<HTEXTURE__*>* textures = nullptr;
    unsigned layerCount = 0;
    unsigned globalX = 0;
    unsigned globalY = 0;
};

struct TerrainDoodadDef {
    std::uint32_t nameId = 0;
    std::uint32_t uniqueId = 0;
    NTempest::C3Vector position{};
    NTempest::C3Vector rotation{};
    float scale = 1.0F;
    std::uint16_t flags = 0;
};

struct TerrainMapObjDef {
    std::uint32_t nameId = 0;
    std::uint32_t uniqueId = 0;
    NTempest::C3Vector position{};
    NTempest::C3Vector rotation{};
    std::uint16_t flags = 0;
    std::uint16_t doodadSet = 0;
    std::uint16_t nameSet = 0;
};

struct TerrainArea {
    std::vector<HTEXTURE__*> textures;
    std::array<DiskEntry, kChunksPerArea * kChunksPerArea> chunks{};
    std::vector<TerrainDoodadDef> doodadDefs;
    std::vector<TerrainMapObjDef> mapObjDefs;
};

struct WorldDoodad {
    std::uint32_t uniqueId = 0;
    NTempest::C3Vector position{};
    NTempest::C3Vector rotation{};
    float scale = 1.0F;
    std::shared_ptr<CModel> model;
};

struct WorldMapObj {
    std::uint32_t uniqueId = 0;
    NTempest::C3Vector position{};
    NTempest::C3Vector rotation{};
    std::uint16_t flags = 0;
    std::uint16_t doodadSet = 0;
    std::shared_ptr<CMapObj> mapObj;
};

NTempest::C44Matrix BuildMapObjPlacement(
    const NTempest::C3Vector& position,
    const NTempest::C3Vector& rotation) {
    // CMap::CreateMapObjDef builds the authored matrix in this exact order.
    // C44Matrix's expression-order multiplication means using operator*= here
    // would rotate the translation and make rotated WMOs orbit the camera.
    NTempest::C44Matrix transform;
    transform.Translate(position);
    transform.Rotate({0.0F, 0.0F, 1.0F}, {rotation.z, 0.0F, 0.0F}, 1);
    transform.Rotate({0.0F, 1.0F, 0.0F}, {rotation.y, 0.0F, 0.0F}, 1);
    transform.Rotate({1.0F, 0.0F, 0.0F}, {rotation.x, 0.0F, 0.0F}, 1);
    return transform;
}

NTempest::C44Matrix BuildCameraRelativeMapObjPlacement(
    const NTempest::C3Vector& position,
    const NTempest::C3Vector& rotation,
    const NTempest::C3Vector& cameraPosition) {
    NTempest::C44Matrix cameraTranslation;
    cameraTranslation.Translate({-cameraPosition.x, -cameraPosition.y,
                                 -cameraPosition.z});
    // RenderMapObjDefGroups uses mapObjDef->mat * Translate(-camPos).
    return BuildMapObjPlacement(position, rotation) * cameraTranslation;
}

struct CreatureDisplayRecord {
    std::uint32_t id = 0;
    std::uint32_t modelId = 0;
    std::uint32_t extendedDisplayInfoId = 0;
    float modelScale = 1.0F;
    std::uint32_t alpha = 255U;
    std::array<std::string, 3> textureVariations;
};

struct CreatureModelRecord {
    std::uint32_t id = 0;
    std::uint32_t flags = 0;
    std::string modelName;
    float modelScale = 1.0F;
};

struct CreatureDisplayExtraRecord {
    std::uint32_t id = 0;
    std::uint32_t race = 0;
    std::uint32_t sex = 0;
    std::uint32_t skin = 0;
    std::uint32_t face = 0;
    std::uint32_t hairStyle = 0;
    std::uint32_t hairColor = 0;
    std::uint32_t facialHair = 0;
    std::string bakedTexture;
};

struct CreatureDatabase {
    bool loadAttempted = false;
    bool loaded = false;
    std::unordered_map<std::uint32_t, CreatureDisplayRecord> displays;
    std::unordered_map<std::uint32_t, CreatureModelRecord> models;
    std::unordered_map<std::uint32_t, CreatureDisplayExtraRecord> extras;
};

struct WorldCreature {
    std::uint64_t guid = 0;
    std::uint32_t entryId = 0;
    std::uint32_t displayId = 0;
    NTempest::C3Vector position{};
    float facing = 0.0F;
    float scale = 1.0F;
    bool moving = false;
    float movementSpeed = 0.0F;
    unsigned sequence = 0U;
    unsigned spellSequence = 0U;
    unsigned spellSequenceUntil = 0U;
    std::uint32_t health = 0U;
    std::uint32_t maxHealth = 0U;
    bool lifeStateKnown = false;
    bool dead = false;
    bool corpse = false;
    unsigned deathSequenceUntil = 0U;
    std::shared_ptr<CModel> model;
    std::array<HTEXTURE__*, 3> textures{};
};

struct WorldSpellEffect {
    std::shared_ptr<CModel> model;
    std::string impactModel;
    NTempest::C3Vector start{};
    NTempest::C3Vector end{};
    NTempest::C3Vector position{};
    std::uint64_t followGuid = 0U;
    unsigned startTime = 0U;
    unsigned duration = 0U;
    bool projectile = false;
};

struct UnitNameLabel {
    std::string text;
    HTEXTBLOCK block = nullptr;
};

struct WorldState {
    SArchive* archive = nullptr;
    SFile* wdt = nullptr;
    std::array<DiskEntry, kMapAreaCount> areas{};
    std::unordered_map<unsigned, TerrainArea> loadedAreas;
    std::vector<TerrainChunk> chunks;
    std::vector<std::string> doodadNames;
    std::vector<std::string> mapObjNames;
    std::vector<WorldDoodad> doodads;
    std::unordered_set<std::uint32_t> doodadIds;
    std::unordered_map<std::string, std::shared_ptr<CModel>> doodadTemplates;
    std::vector<WorldMapObj> mapObjects;
    std::unordered_set<std::uint32_t> mapObjIds;
    std::unordered_map<std::string, std::shared_ptr<CMapObj>> mapObjTemplates;
    std::vector<NTempest::CFacet> terrainCollision;
    std::vector<NTempest::CFacet> objectCollision;
    std::unordered_map<std::uint64_t, WorldCreature> creatures;
    std::unordered_map<std::string, std::shared_ptr<CModel>> creatureTemplates;
    NTempest::C3Vector playerPosition{};
    NTempest::C3Vector cameraPosition{};
    CHARINFO playerCharacter{};
    bool playerCharacterReady = false;
    bool playerJumping = false;
    float playerJumpVelocity = 0.0F;
    unsigned jumpStartTime = 0U;
    unsigned landAnimationUntil = 0U;
    unsigned playerSpellSequence = 0U;
    unsigned playerSpellSequenceUntil = 0U;
    std::uint32_t playerHealth = 0U;
    std::uint32_t playerMaxHealth = 0U;
    bool playerLifeStateKnown = false;
    bool playerDead = false;
    unsigned playerDeathSequenceUntil = 0U;
    std::uint64_t playerKillerGuid = 0U;
    std::vector<WorldSpellEffect> spellEffects;
    std::unordered_map<std::string, std::shared_ptr<CModel>> spellEffectTemplates;
    HTEXTFONT unitNameFont = nullptr;
    std::unordered_map<std::uint64_t, UnitNameLabel> unitNameLabels;
    std::string mapName;
    bool loaded = false;
};

WorldState s_world;
CreatureDatabase s_creatureDatabase;

struct SpellVisualDefinition {
    unsigned precastAnimation = 0U;
    unsigned castAnimation = 0U;
    float missileSpeed = 0.0F;
    std::string precastModel;
    std::string castModel;
    std::string missileModel;
    std::string impactModel;
};

struct SpellVisualDatabase {
    bool loadAttempted = false;
    bool loaded = false;
    std::unordered_map<unsigned, SpellVisualDefinition> spells;
};

SpellVisualDatabase s_spellVisualDatabase;

std::uint32_t ReadU32(const void* source) {
    std::uint32_t value = 0;
    std::memcpy(&value, source, sizeof(value));
    return value;
}

std::uint16_t ReadU16(const void* source) {
    std::uint16_t value = 0;
    std::memcpy(&value, source, sizeof(value));
    return value;
}

float ReadFloat(const void* source) {
    float value = 0.0F;
    std::memcpy(&value, source, sizeof(value));
    return value;
}

struct DbcFile {
    std::vector<std::uint8_t> bytes;
    std::uint32_t records = 0;
    std::uint32_t fields = 0;
    std::uint32_t recordSize = 0;
    std::uint32_t stringSize = 0;

    const std::uint8_t* Record(std::uint32_t row) const {
        if (row >= records) return nullptr;
        return bytes.data() + 20U + static_cast<std::size_t>(row) * recordSize;
    }

    const char* String(std::uint32_t offset) const {
        if (offset >= stringSize) return "";
        const std::size_t base = 20U + static_cast<std::size_t>(records) * recordSize;
        return reinterpret_cast<const char*>(bytes.data() + base + offset);
    }
};

bool LoadDbcFile(const char* filename, std::uint32_t minimumFields,
                 DbcFile& dbc) {
    void* data = nullptr;
    unsigned size = 0;
    if (!SFile::LoadFile(filename, &data, &size, 1, nullptr) || !data)
        return false;
    const auto* input = static_cast<const std::uint8_t*>(data);
    const bool headerValid = size >= 20U && std::memcmp(input, "WDBC", 4U) == 0;
    if (headerValid) {
        dbc.records = ReadU32(input + 4U);
        dbc.fields = ReadU32(input + 8U);
        dbc.recordSize = ReadU32(input + 12U);
        dbc.stringSize = ReadU32(input + 16U);
    }
    const std::size_t stringOffset = 20U +
        static_cast<std::size_t>(dbc.records) * dbc.recordSize;
    const bool valid = headerValid && dbc.fields >= minimumFields &&
        dbc.recordSize >= minimumFields * 4U && stringOffset <= size &&
        dbc.stringSize <= size - stringOffset;
    if (valid) dbc.bytes.assign(input, input + size);
    SFile::Unload(data);
    if (!valid) {
        std::fprintf(stderr,
                     "WORLD_CREATURE_DBC_FAILED file=%s size=%u fields=%u "
                     "recordSize=%u records=%u strings=%u\n",
                     filename, size, dbc.fields, dbc.recordSize, dbc.records,
                     dbc.stringSize);
    }
    return valid;
}

bool LoadCreatureDatabase() {
    if (s_creatureDatabase.loadAttempted) return s_creatureDatabase.loaded;
    s_creatureDatabase.loadAttempted = true;

    DbcFile displayDbc;
    DbcFile modelDbc;
    DbcFile extraDbc;
    if (!LoadDbcFile("DBFilesClient\\CreatureDisplayInfo.dbc", 10U,
                     displayDbc) ||
        !LoadDbcFile("DBFilesClient\\CreatureModelData.dbc", 14U,
                     modelDbc) ||
        !LoadDbcFile("DBFilesClient\\CreatureDisplayInfoExtra.dbc", 19U,
                     extraDbc)) {
        return false;
    }

    s_creatureDatabase.displays.reserve(displayDbc.records);
    for (std::uint32_t row = 0; row < displayDbc.records; ++row) {
        const auto* record = displayDbc.Record(row);
        CreatureDisplayRecord value;
        value.id = ReadU32(record);
        value.modelId = ReadU32(record + 4U);
        value.extendedDisplayInfoId = ReadU32(record + 12U);
        value.modelScale = ReadFloat(record + 16U);
        value.alpha = ReadU32(record + 20U);
        for (unsigned texture = 0; texture < value.textureVariations.size(); ++texture)
            value.textureVariations[texture] =
                displayDbc.String(ReadU32(record + 24U + texture * 4U));
        s_creatureDatabase.displays.emplace(value.id, std::move(value));
    }

    s_creatureDatabase.models.reserve(modelDbc.records);
    for (std::uint32_t row = 0; row < modelDbc.records; ++row) {
        const auto* record = modelDbc.Record(row);
        CreatureModelRecord value;
        value.id = ReadU32(record);
        value.flags = ReadU32(record + 4U);
        value.modelName = modelDbc.String(ReadU32(record + 8U));
        value.modelScale = ReadFloat(record + 16U);
        s_creatureDatabase.models.emplace(value.id, std::move(value));
    }

    s_creatureDatabase.extras.reserve(extraDbc.records);
    for (std::uint32_t row = 0; row < extraDbc.records; ++row) {
        const auto* record = extraDbc.Record(row);
        CreatureDisplayExtraRecord value;
        value.id = ReadU32(record);
        value.race = ReadU32(record + 4U);
        value.sex = ReadU32(record + 8U);
        value.skin = ReadU32(record + 12U);
        value.face = ReadU32(record + 16U);
        value.hairStyle = ReadU32(record + 20U);
        value.hairColor = ReadU32(record + 24U);
        value.facialHair = ReadU32(record + 28U);
        value.bakedTexture = extraDbc.String(ReadU32(record + 72U));
        s_creatureDatabase.extras.emplace(value.id, std::move(value));
    }

    s_creatureDatabase.loaded = true;
    std::fprintf(stderr,
                 "WORLD_CREATURE_DBC displays=%zu models=%zu extras=%zu\n",
                 s_creatureDatabase.displays.size(),
                 s_creatureDatabase.models.size(),
                 s_creatureDatabase.extras.size());
    return true;
}

const std::uint8_t* FindDbcRecord(const DbcFile& dbc, std::uint32_t id) {
    for (std::uint32_t row = 0; row < dbc.records; ++row) {
        const std::uint8_t* record = dbc.Record(row);
        if (record && ReadU32(record) == id) return record;
    }
    return nullptr;
}

std::string CompleteModelFileName(const char* fileName) {
    if (!fileName || !*fileName) return {};
    std::string result = fileName;
    const std::size_t separator = result.find_last_of("\\/");
    const std::size_t extension = result.find_last_of('.');
    if (extension == std::string::npos ||
        (separator != std::string::npos && extension < separator)) {
        result += ".mdx";
    }
    return result;
}

unsigned ResolveSpellAnimation(const DbcFile& animationDbc,
                               unsigned visualAnimation) {
    const std::uint8_t* animation = FindDbcRecord(animationDbc,
                                                   visualAnimation);
    const char* name = animation
        ? animationDbc.String(ReadU32(animation + 4U)) : nullptr;
    if (!name || !*name) return 0U;

    // SpellVisualAnimName calls the two hold poses "ReadySpell" while model
    // animation data uses the recovered SpellPrecast names.
    if (_stricmp(name, "ReadySpellDirected") == 0)
        name = "SpellPrecastDirected";
    else if (_stricmp(name, "ReadySpellOmni") == 0)
        name = "SpellPrecastOmni";

    for (std::size_t index = 0; index < g_animationNameCount; ++index) {
        if (_stricmp(name, g_animationNames[index]) == 0)
            return static_cast<unsigned>(index);
    }
    return 0U;
}

bool LoadSpellVisualDatabase() {
    if (s_spellVisualDatabase.loadAttempted)
        return s_spellVisualDatabase.loaded;
    s_spellVisualDatabase.loadAttempted = true;

    DbcFile spellDbc;
    DbcFile visualDbc;
    DbcFile kitDbc;
    DbcFile effectDbc;
    DbcFile animationDbc;
    if (!LoadDbcFile("DBFilesClient\\Spell.dbc", 102U, spellDbc) ||
        !LoadDbcFile("DBFilesClient\\SpellVisual.dbc", 15U, visualDbc) ||
        !LoadDbcFile("DBFilesClient\\SpellVisualKit.dbc", 19U, kitDbc) ||
        !LoadDbcFile("DBFilesClient\\SpellVisualEffectName.dbc", 6U,
                     effectDbc) ||
        !LoadDbcFile("DBFilesClient\\SpellVisualAnimName.dbc", 2U,
                     animationDbc)) {
        return false;
    }

    const auto effectName = [&effectDbc](unsigned effectID) {
        const std::uint8_t* effect = FindDbcRecord(effectDbc, effectID);
        return effect
            ? CompleteModelFileName(effectDbc.String(ReadU32(effect + 4U)))
            : std::string{};
    };
    const auto kitEffect = [&kitDbc, &effectName](unsigned kitID,
                                                  bool handsFirst) {
        const std::uint8_t* kit = FindDbcRecord(kitDbc, kitID);
        if (!kit) return std::string{};
        // Head, chest, base, left hand, right hand, breath, specials 0..2.
        static constexpr std::array<unsigned, 9> kBodyOrder{
            12U, 16U, 20U, 24U, 28U, 32U, 36U, 40U, 44U};
        static constexpr std::array<unsigned, 9> kHandOrder{
            24U, 28U, 16U, 12U, 20U, 32U, 36U, 40U, 44U};
        const auto& order = handsFirst ? kHandOrder : kBodyOrder;
        for (unsigned offset : order) {
            const unsigned effectID = ReadU32(kit + offset);
            if (effectID != 0U) {
                std::string fileName = effectName(effectID);
                if (!fileName.empty()) return fileName;
            }
        }
        return std::string{};
    };
    const auto kitAnimation = [&kitDbc, &animationDbc](unsigned kitID) {
        const std::uint8_t* kit = FindDbcRecord(kitDbc, kitID);
        return kit ? ResolveSpellAnimation(animationDbc,
                                            ReadU32(kit + 8U)) : 0U;
    };

    s_spellVisualDatabase.spells.reserve(spellDbc.records);
    for (std::uint32_t row = 0; row < spellDbc.records; ++row) {
        const std::uint8_t* spell = spellDbc.Record(row);
        const unsigned spellID = ReadU32(spell);
        const float missileSpeed = ReadFloat(spell + 31U * 4U);
        const unsigned visualID = ReadU32(spell + 101U * 4U);
        const std::uint8_t* visual = FindDbcRecord(visualDbc, visualID);
        if (!visual) continue;

        const unsigned precastKit = ReadU32(visual + 4U);
        const unsigned castKit = ReadU32(visual + 8U);
        const unsigned impactKit = ReadU32(visual + 12U);
        SpellVisualDefinition definition;
        definition.precastAnimation = kitAnimation(precastKit);
        definition.castAnimation = kitAnimation(castKit);
        definition.missileSpeed = missileSpeed;
        definition.precastModel = kitEffect(precastKit, true);
        definition.castModel = kitEffect(castKit, true);
        definition.impactModel = kitEffect(impactKit, false);
        if (ReadU32(visual + 24U) != 0U)
            definition.missileModel = effectName(ReadU32(visual + 28U));
        s_spellVisualDatabase.spells.emplace(spellID,
                                             std::move(definition));
    }

    s_spellVisualDatabase.loaded = true;
    std::fprintf(stderr, "WORLD_SPELL_VISUAL_DBC spells=%zu\n",
                 s_spellVisualDatabase.spells.size());
    return true;
}

std::string CreatureTextureFile(const std::string& modelName,
                                const std::string& variation) {
    if (variation.empty()) return {};
    const std::size_t separator = modelName.find_last_of("\\/");
    std::string result = separator == std::string::npos
        ? variation : modelName.substr(0U, separator + 1U) + variation;
    const std::size_t resultSeparator = result.find_last_of("\\/");
    const std::size_t extension = result.find_last_of('.');
    if (extension == std::string::npos ||
        (resultSeparator != std::string::npos && extension < resultSeparator)) {
        result += ".blp";
    }
    return result;
}

std::string BakedCreatureTextureFile(const std::string& bakedName) {
    if (bakedName.empty()) return {};
    std::string result = "Textures\\BakedNpcTextures\\" + bakedName;
    if (result.find_last_of('.') == std::string::npos) result += ".blp";
    return result;
}

void ReleaseCreature(WorldCreature& creature) {
    creature.model.reset();
    for (HTEXTURE__*& texture : creature.textures) {
        if (texture) TextureRelease(texture);
        texture = nullptr;
    }
}

unsigned DeathSequenceDeadline(const std::shared_ptr<CModel>& model,
                               unsigned now) {
    unsigned duration = 1200U;
    if (model) {
        const unsigned recoveredDuration =
            ModelGetSequenceDuration(*model, kAnimDeath);
        if (recoveredDuration != 0U)
            duration = (std::clamp)(recoveredDuration, 250U, 10000U);
    }
    return now + duration;
}

void CancelFollowingSpellEffects(std::uint64_t guid) {
    std::erase_if(s_world.spellEffects,
                  [guid](const WorldSpellEffect& effect) {
                      return !effect.projectile && effect.followGuid == guid;
                  });
}

void BeginCreatureDeath(WorldCreature& creature, bool animate) {
    const unsigned now = OsGetAsyncTimeMs();
    creature.dead = true;
    creature.moving = false;
    creature.movementSpeed = 0.0F;
    creature.spellSequence = 0U;
    creature.spellSequenceUntil = 0U;
    creature.deathSequenceUntil = animate
        ? DeathSequenceDeadline(creature.model, now) : 0U;
    CancelFollowingSpellEffects(creature.guid);

    const unsigned sequence = animate ? kAnimDeath : kAnimDead;
    if (creature.model && ModelSetSequence(*creature.model, sequence))
        creature.sequence = sequence;
    std::fprintf(stderr,
                 "WORLD_UNIT_DEATH guid=%llu animate=%u sequence=%u holdAt=%u\n",
                 static_cast<unsigned long long>(creature.guid),
                 animate ? 1U : 0U, sequence,
                 creature.deathSequenceUntil);
}

void RestoreCreature(WorldCreature& creature) {
    creature.dead = false;
    creature.deathSequenceUntil = 0U;
    creature.spellSequence = 0U;
    creature.spellSequenceUntil = 0U;
    if (creature.model && ModelSetSequence(*creature.model, kAnimStand))
        creature.sequence = kAnimStand;
    std::fprintf(stderr, "WORLD_UNIT_RESTORED guid=%llu health=%u/%u\n",
                 static_cast<unsigned long long>(creature.guid),
                 creature.health, creature.maxHealth);
}

void BeginPlayerDeath(bool animate) {
    if (s_world.playerDead) return;
    const unsigned now = OsGetAsyncTimeMs();
    s_world.playerDead = true;
    s_world.playerJumping = false;
    s_world.playerJumpVelocity = 0.0F;
    s_world.jumpStartTime = 0U;
    s_world.landAnimationUntil = 0U;
    s_world.playerSpellSequence = 0U;
    s_world.playerSpellSequenceUntil = 0U;
    CModel* player = s_world.playerCharacter.m_characterModelOwner.get();
    s_world.playerDeathSequenceUntil = animate
        ? DeathSequenceDeadline(
              s_world.playerCharacter.m_characterModelOwner, now)
        : 0U;
    CancelFollowingSpellEffects(ClntObjMgrGetActivePlayer());
    if (CGInputControl* input = CGInputControl::GetActive()) {
        input->m_controlFlags &= ~(INPUT_MOVE_PLAYER_MASK |
            INPUT_STRAFE_PLAYER_MASK | INPUT_TURN_PLAYER_MASK |
            INPUT_PITCH_PLAYER_MASK | INPUT_MOVE_PLAYER_AUTORUN |
            INPUT_PLAYER_MOVED);
    }
    CGGameUI::Target(0U);
    if (player)
        ModelSetSequence(*player, animate ? kAnimDeath : kAnimDead);
    FrameScript_SignalEvent(0x100U); // SCRIPT_PLAYER_DEAD
    std::fprintf(stderr,
                 "WORLD_PLAYER_DEATH killer=%llu animate=%u holdAt=%u "
                 "health=%u/%u\n",
                 static_cast<unsigned long long>(s_world.playerKillerGuid),
                 animate ? 1U : 0U, s_world.playerDeathSequenceUntil,
                 s_world.playerHealth, s_world.playerMaxHealth);
}

void RestorePlayer() {
    if (!s_world.playerDead) return;
    s_world.playerDead = false;
    s_world.playerDeathSequenceUntil = 0U;
    s_world.playerKillerGuid = 0U;
    CModel* player = s_world.playerCharacter.m_characterModelOwner.get();
    if (player) ModelSetSequence(*player, kAnimStand);
    FrameScript_SignalEvent(0x0FFU); // SCRIPT_PLAYER_ALIVE
    std::fprintf(stderr, "WORLD_PLAYER_RESTORED health=%u/%u\n",
                 s_world.playerHealth, s_world.playerMaxHealth);
}

void SyncPlayerLifeState() {
    const std::uint64_t guid = ClntObjMgrGetActivePlayer();
    const CLNT_VISIBLE_OBJECT* object = ClntObjMgrGetVisibleObject(guid);
    if (!object || object->maxHealth == 0U) return;

    const std::uint32_t oldHealth = s_world.playerHealth;
    const bool wasKnown = s_world.playerLifeStateKnown;
    s_world.playerHealth = object->health;
    s_world.playerMaxHealth = object->maxHealth;
    s_world.playerLifeStateKnown = true;

    if (!wasKnown) {
        if (object->health == 0U) BeginPlayerDeath(false);
    } else if (oldHealth > 0U && object->health == 0U) {
        BeginPlayerDeath(true);
    } else if (oldHealth == 0U && object->health > 0U) {
        RestorePlayer();
    }
}

int __fastcall WorldDeathNotifyHandler(void*, unsigned, unsigned,
                                       CDataStore* message) {
    if (!message) return 0;
    std::uint64_t killerGuid = 0U;
    message->Get(&killerGuid);
    if (message->Failed()) return 0;
    CWorld::NotifyPlayerDeath(killerGuid);
    return 1;
}

bool ReadFileRange(std::uint32_t offset, void* destination, std::uint32_t bytes) {
    if (!s_world.wdt || (!destination && bytes)) return false;
    if (SFile::SetFilePointer(s_world.wdt, static_cast<int>(offset), nullptr,
                              FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
        return false;
    }
    unsigned actual = 0;
    return SFile::Read(s_world.wdt, destination, bytes, &actual, nullptr, nullptr) &&
        actual == bytes;
}

void ReleaseArea(TerrainArea& area) {
    for (HTEXTURE__* texture : area.textures) {
        if (texture) TextureRelease(texture);
    }
    area.textures.clear();
}

void ResetWorld() {
    if (s_initialized_2)
        DayNightDestroy();
    s_world.playerCharacter.Destroy();
    s_world.playerCharacter = {};
    s_world.playerCharacterReady = false;
    s_world.playerJumping = false;
    s_world.playerJumpVelocity = 0.0F;
    s_world.jumpStartTime = 0U;
    s_world.landAnimationUntil = 0U;
    s_world.playerSpellSequence = 0U;
    s_world.playerSpellSequenceUntil = 0U;
    s_world.playerHealth = 0U;
    s_world.playerMaxHealth = 0U;
    s_world.playerLifeStateKnown = false;
    s_world.playerDead = false;
    s_world.playerDeathSequenceUntil = 0U;
    s_world.playerKillerGuid = 0U;
    s_world.spellEffects.clear();
    s_world.spellEffectTemplates.clear();
    for (auto& [guid, label] : s_world.unitNameLabels) {
        (void)guid;
        if (label.block) TextBlockDestroy(label.block);
    }
    s_world.unitNameLabels.clear();
    if (s_world.unitNameFont) {
        TextBlockReleaseFont(s_world.unitNameFont);
        s_world.unitNameFont = nullptr;
    }
    for (auto& [guid, creature] : s_world.creatures) {
        (void)guid;
        ReleaseCreature(creature);
    }
    s_world.creatures.clear();
    s_world.creatureTemplates.clear();
    for (TerrainChunk& chunk : s_world.chunks) {
        for (auto& mask : chunk.alphaMasks) {
            if (mask && mask->texture) {
                GxTexDestroy(mask->texture);
                mask->texture = nullptr;
            }
        }
    }
    s_world.chunks.clear();
    s_world.doodads.clear();
    s_world.doodadIds.clear();
    s_world.doodadTemplates.clear();
    s_world.mapObjects.clear();
    s_world.mapObjIds.clear();
    s_world.mapObjTemplates.clear();
    s_world.terrainCollision.clear();
    s_world.objectCollision.clear();
    s_world.doodadNames.clear();
    s_world.mapObjNames.clear();
    for (auto& [index, area] : s_world.loadedAreas) {
        (void)index;
        ReleaseArea(area);
    }
    s_world.loadedAreas.clear();
    if (s_world.wdt) {
        SFile::Close(s_world.wdt);
        s_world.wdt = nullptr;
    }
    if (s_world.archive) {
        SFile::CloseArchive(s_world.archive);
        s_world.archive = nullptr;
    }
    s_world.areas.fill({});
    s_world.playerPosition = {};
    s_world.cameraPosition = {};
    ParticleSystem2SetCameraWorldPosition({});
    s_world.mapName.clear();
    s_world.loaded = false;
}

void SendPlayerMovementUpdatePacket(unsigned opcode, unsigned moveFlags) {
    if (ClntObjMgrGetActivePlayer() == 0U) return;

    const NTempest::C3Vector& position = ClntObjMgrGetActivePosition();
    const float facing = ClntObjMgrGetActiveFacing();
    constexpr std::uint64_t noTransport = 0U;
    constexpr float zero = 0.0F;

    // CGUnit_C::BuildMovementUpdate writes the active mover in this exact
    // order.  Transport-relative values are zero while the reconstructed
    // player is not attached to a transport.
    CDataStore message;
    message.Put(opcode)
        ->Put(noTransport)
        ->Put(zero)->Put(zero)->Put(zero)->Put(zero)
        ->Put(position.x)->Put(position.y)->Put(position.z)
        ->Put(facing)->Put(zero)
        ->Put(moveFlags & 0xFAFF0BFFU);
    ClientServices_Send(&message);
}

const SpellVisualDefinition* FindSpellVisual(unsigned spellID) {
    if (!LoadSpellVisualDatabase()) return nullptr;
    const auto found = s_spellVisualDatabase.spells.find(spellID);
    return found == s_spellVisualDatabase.spells.end()
        ? nullptr : &found->second;
}

NTempest::C3Vector UnitWorldPosition(std::uint64_t guid) {
    if (guid == ClntObjMgrGetActivePlayer())
        return ClntObjMgrGetActivePosition();
    if (const CLNT_VISIBLE_OBJECT* object = ClntObjMgrGetVisibleObject(guid))
        return object->position;
    if (const auto creature = s_world.creatures.find(guid);
        creature != s_world.creatures.end()) {
        return creature->second.position;
    }
    return s_world.playerPosition;
}

std::shared_ptr<CModel> CreateSpellEffectModel(const std::string& fileName) {
    if (fileName.empty()) return {};
    std::shared_ptr<CModel> source;
    if (const auto found = s_world.spellEffectTemplates.find(fileName);
        found != s_world.spellEffectTemplates.end()) {
        source = found->second;
    } else {
        source = ModelCreate(fileName.c_str());
        if (source)
            s_world.spellEffectTemplates.emplace(fileName, source);
    }
    std::shared_ptr<CModel> model = source ? ModelDuplicate(*source) : nullptr;
    if (!model || !model->data) {
        std::fprintf(stderr, "WORLD_SPELL_MODEL_FAILED file=%s\n",
                     fileName.c_str());
        return {};
    }
    if (ModelGetNumSequences(*model) != 0U) ModelSetSequence(*model, 0U);
    ModelSetLightSelectCallback(*model, &CWorld::SelectLight, nullptr);
    return model;
}

void SpawnStationarySpellEffect(const std::string& fileName,
                                std::uint64_t followGuid,
                                const NTempest::C3Vector& position,
                                unsigned duration) {
    std::shared_ptr<CModel> model = CreateSpellEffectModel(fileName);
    if (!model) return;
    WorldSpellEffect effect;
    effect.model = std::move(model);
    effect.position = position;
    effect.followGuid = followGuid;
    effect.startTime = OsGetAsyncTimeMs();
    effect.duration = (std::max)(duration, 250U);
    s_world.spellEffects.emplace_back(std::move(effect));
}

void SpawnProjectileSpellEffect(const SpellVisualDefinition& definition,
                                const NTempest::C3Vector& start,
                                const NTempest::C3Vector& end) {
    std::shared_ptr<CModel> model =
        CreateSpellEffectModel(definition.missileModel);
    if (!model) {
        SpawnStationarySpellEffect(definition.impactModel, 0U, end, 1100U);
        return;
    }
    const NTempest::C3Vector delta = end - start;
    const float distance = std::sqrt(delta.SquaredMag());
    const float speed = definition.missileSpeed > 0.0F
        ? definition.missileSpeed : 20.0F;
    WorldSpellEffect effect;
    effect.model = std::move(model);
    effect.impactModel = definition.impactModel;
    effect.start = start;
    effect.end = end;
    effect.position = start;
    effect.startTime = OsGetAsyncTimeMs();
    effect.duration = (std::clamp)(
        static_cast<unsigned>(std::lround(distance / speed * 1000.0F)),
        100U, 5000U);
    effect.projectile = true;
    s_world.spellEffects.emplace_back(std::move(effect));
}

void SetUnitTransientSequence(std::uint64_t guid, unsigned sequence,
                              unsigned duration) {
    if (sequence == 0U) return;
    const unsigned until = OsGetAsyncTimeMs() + (std::max)(duration, 250U);
    if (guid == ClntObjMgrGetActivePlayer()) {
        if (s_world.playerDead) return;
        s_world.playerSpellSequence = sequence;
        s_world.playerSpellSequenceUntil = until;
        return;
    }
    if (auto found = s_world.creatures.find(guid);
        found != s_world.creatures.end()) {
        if (found->second.dead) return;
        found->second.spellSequence = sequence;
        found->second.spellSequenceUntil = until;
    }
}

void UpdateSpellEffects(unsigned elapsedMs) {
    const unsigned now = OsGetAsyncTimeMs();
    struct PendingImpact {
        std::string model;
        NTempest::C3Vector position{};
    };
    std::vector<PendingImpact> impacts;
    for (WorldSpellEffect& effect : s_world.spellEffects) {
        if (!effect.model || !effect.model->data) continue;
        const unsigned age = now - effect.startTime;
        if (effect.projectile) {
            const float progress = effect.duration != 0U
                ? (std::min)(1.0F,
                    static_cast<float>(age) / static_cast<float>(effect.duration))
                : 1.0F;
            effect.position = effect.start + (effect.end - effect.start) * progress;
        } else if (effect.followGuid != 0U) {
            effect.position = UnitWorldPosition(effect.followGuid);
            effect.position.z += 1.15F;
        }

        const NTempest::C3Vector direction = effect.end - effect.start;
        const float yaw = effect.projectile
            ? std::atan2(direction.y, direction.x) : 0.0F;
        NTempest::C34Matrix transform = NTempest::C34Matrix::Rotation(
            {0.0F, 0.0F, 1.0F}, yaw);
        const NTempest::C3Vector relative =
            effect.position - s_world.cameraPosition;
        transform.d0 = relative.x;
        transform.d1 = relative.y;
        transform.d2 = relative.z;
        effect.model->data->m_modelToWorld = transform;
        ModelAnimateAttached(*effect.model, elapsedMs);

        if (effect.projectile && age >= effect.duration &&
            !effect.impactModel.empty()) {
            impacts.push_back({effect.impactModel, effect.end});
            effect.impactModel.clear();
        }
    }
    std::erase_if(s_world.spellEffects, [now](const WorldSpellEffect& effect) {
        return !effect.model || now - effect.startTime >= effect.duration;
    });
    for (const PendingImpact& impact : impacts)
        SpawnStationarySpellEffect(impact.model, 0U, impact.position, 1100U);
}

const char* ResolveVisibleObjectName(const CLNT_VISIBLE_OBJECT& object) {
    if (object.guid == ClntObjMgrGetActivePlayer() &&
        CGlueMgr::m_characterInfo && CGlueMgr::m_characterInfo->name[0]) {
        return CGlueMgr::m_characterInfo->name;
    }
    if (object.objectType == 4U) {
        if (const NameCache* name = g_nameDBCache.GetRecord(object.guid))
            return name->m_name[0] ? name->m_name : nullptr;
    } else if (object.objectType == 3U && object.entryId != 0U) {
        if (const CreatureStats_C* creature =
                g_creatureDBCache.GetRecord(static_cast<int>(object.entryId))) {
            return creature->m_name[0][0] ? creature->m_name[0] : nullptr;
        }
    }
    return nullptr;
}

bool ProjectUnitName(const CLNT_VISIBLE_OBJECT& object,
                     const NTempest::C44Matrix& view,
                     const NTempest::C44Matrix& projection,
                     float& ddcX, float& ddcY) {
    const NTempest::C3Vector relative = object.position - s_world.cameraPosition;
    if (relative.SquaredMag() > 10000.0F) return false;

    NTempest::C44Matrix world;
    world.Translate(relative);
    NTempest::C44Matrix modelView = world * view;
    modelView.a2 = -modelView.a2;
    modelView.b2 = -modelView.b2;
    modelView.c2 = -modelView.c2;
    modelView.d2 = -modelView.d2;

    const NTempest::C4Vector eye = modelView * NTempest::C4Vector(
        0.0F, 0.0F, 2.25F, 1.0F);
    const NTempest::C4Vector clip = projection * eye;
    if (clip.w <= 0.00001F) return false;

    const float ndcX = (clip.x / clip.w + 1.0F) * 0.5F;
    const float ndcY = (clip.y / clip.w + 1.0F) * 0.5F;
    if (ndcX < 0.0F || ndcX > 1.0F || ndcY < 0.0F || ndcY > 1.0F)
        return false;
    NDCToDDC(&ddcX, &ddcY, ndcX, ndcY);
    return true;
}

void RenderUnitNames() {
    const std::uint64_t activeGuid = ClntObjMgrGetActivePlayer();
    if (activeGuid == 0U) return;

    if (!s_world.unitNameFont) {
        s_world.unitNameFont = TextBlockGenerateFont(
            "Fonts\\FRIZQT__.TTF", 1, 0.017F);
        if (!s_world.unitNameFont) return;
    }

    NTempest::C44Matrix view;
    NTempest::C44Matrix projection;
    GxXformView(&view);
    GxXformProjection(&projection);
    projection.c0 = -projection.c0;
    projection.c1 = -projection.c1;
    projection.c2 = -projection.c2;
    projection.c3 = -projection.c3;
    if (!GxMasterEnable(GxMasterEnable_NormalProjection) && projection.d3 != 1.0F) {
        projection.a0 *= 0.2F; projection.a1 *= 0.2F; projection.a2 *= 0.2F;
        projection.b0 *= 0.2F; projection.b1 *= 0.2F; projection.b2 *= 0.2F;
        projection.c0 *= 0.2F; projection.c1 *= 0.2F; projection.c2 *= 0.2F;
        projection.d0 *= 0.2F; projection.d1 *= 0.2F; projection.d2 *= 0.2F;
    }

    std::vector<CLNT_VISIBLE_OBJECT> objects = ClntObjMgrGetVisibleObjects();
    auto active = std::find_if(objects.begin(), objects.end(),
        [activeGuid](const CLNT_VISIBLE_OBJECT& object) {
            return object.guid == activeGuid;
        });
    if (active == objects.end()) {
        CLNT_VISIBLE_OBJECT local{};
        local.guid = activeGuid;
        local.objectType = 4U;
        local.position = ClntObjMgrGetActivePosition();
        local.facing = ClntObjMgrGetActiveFacing();
        objects.push_back(local);
    } else {
        active->position = ClntObjMgrGetActivePosition();
        active->facing = ClntObjMgrGetActiveFacing();
    }

    struct VisibleLabel {
        std::uint64_t guid;
        float x;
        float y;
        NTempest::CImVector color;
    };
    std::vector<VisibleLabel> visible;
    std::unordered_set<std::uint64_t> known;
    for (const CLNT_VISIBLE_OBJECT& object : objects) {
        if (object.objectType != 3U && object.objectType != 4U) continue;
        known.insert(object.guid);
        const char* name = ResolveVisibleObjectName(object);
        if (!name || !*name) continue;

        UnitNameLabel& label = s_world.unitNameLabels[object.guid];
        if (!label.block || label.text != name) {
            if (label.block) TextBlockDestroy(label.block);
            label.text = name;
            const NTempest::CImVector color(object.guid == activeGuid
                ? 0xFF00FF00U
                : object.objectType == 3U ? 0xFFFFFF00U : 0xFF54A7FFU);
            const NTempest::C3Vector origin{};
            label.block = TextBlockCreate(s_world.unitNameFont, label.text.c_str(),
                &color, &origin, 0.017F, 0.32F, 0.025F, 2U, 0.0F, 0.0F);
            if (label.block) {
                const NTempest::C2Vector shadowOffset{0.0015F, -0.0015F};
                TextBlockAddShadow(label.block, &shadowOffset,
                                   NTempest::CImVector(0xFF000000U));
            }
        }

        float x = 0.0F;
        float y = 0.0F;
        if (label.block && ProjectUnitName(object, view, projection, x, y)) {
            visible.push_back({object.guid, x, y,
                NTempest::CImVector(object.guid == activeGuid
                    ? 0xFF00FF00U
                    : object.objectType == 3U ? 0xFFFFFF00U : 0xFF54A7FFU)});
        }
    }

    for (auto it = s_world.unitNameLabels.begin();
         it != s_world.unitNameLabels.end();) {
        if (!known.contains(it->first)) {
            if (it->second.block) TextBlockDestroy(it->second.block);
            it = s_world.unitNameLabels.erase(it);
        } else {
            ++it;
        }
    }

    if (visible.empty()) return;

    float minX = 0.0F, maxX = 1.0F;
    float minY = 0.0F, maxY = 1.0F;
    float minZ = 0.0F, maxZ = 1.0F;
    GxXformViewport(&minX, &maxX, &minY, &maxY, &minZ, &maxZ);
    const float width = static_cast<float>(NDCToDDCWidth(1.0F));
    const float height = static_cast<float>(NDCToDDCHeight(1.0F));
    const NTempest::C44Matrix identity;
    const NTempest::C44Matrix ortho(
        2.0F / width, 0.0F, 0.0F, 0.0F,
        0.0F, 2.0F / height, 0.0F, 0.0F,
        0.0F, 0.0F, 0.004F, 0.0F,
        -1.0F, -1.0F, -1.0F, 1.0F);
    GxXformSetViewport(0.0F, 1.0F, 0.0F, 1.0F, 0.0F, 1.0F);
    GxXformSetView(identity);
    GxXformSetProjection(ortho);
    for (const VisibleLabel& projected : visible) {
        auto found = s_world.unitNameLabels.find(projected.guid);
        if (found == s_world.unitNameLabels.end() || !found->second.block) continue;
        const NTempest::C3Vector position{
            projected.x - 0.16F, projected.y + 0.004F, 0.0F};
        TextBlockUpdateColor(found->second.block, &projected.color);
        TextBlockAnimate(found->second.block, &position);
        TextBlockRender(found->second.block);
    }
    GxXformSetProjection(projection);
    GxXformSetView(view);
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

bool LoadNameChunk(std::uint32_t& cursor, std::uint32_t expectedToken,
                   std::uint32_t expectedNames,
                   std::vector<std::string>& names) {
    std::array<std::uint32_t, 2> chunk{};
    if (!ReadFileRange(cursor, chunk.data(), sizeof(chunk))) {
        std::fprintf(stderr,
                     "WORLD_WDT_NAME_CHUNK_READ_FAILED offset=%u expected=%08X\n",
                     cursor, expectedToken);
        return false;
    }
    if (chunk[0] != expectedToken || chunk[1] > 16U * 1024U * 1024U) {
        std::fprintf(stderr,
                     "WORLD_WDT_NAME_CHUNK_INVALID offset=%u expected=%08X actual=%08X bytes=%u names=%u\n",
                     cursor, expectedToken, chunk[0], chunk[1], expectedNames);
        return false;
    }
    cursor += static_cast<std::uint32_t>(sizeof(chunk));
    std::vector<char> bytes(chunk[1]);
    if (!bytes.empty() &&
        !ReadFileRange(cursor, bytes.data(), static_cast<std::uint32_t>(bytes.size()))) {
        return false;
    }
    cursor += static_cast<std::uint32_t>(bytes.size());
    names.clear();
    names.reserve(expectedNames);
    std::size_t nameOffset = 0;
    while (nameOffset < bytes.size() && names.size() < expectedNames) {
        const std::size_t remaining = bytes.size() - nameOffset;
        const std::size_t length = strnlen(bytes.data() + nameOffset, remaining);
        if (length == remaining) return false;
        names.emplace_back(bytes.data() + nameOffset, length);
        nameOffset += length + 1U;
    }
    if (names.size() > expectedNames) {
        std::fprintf(stderr,
                     "WORLD_WDT_NAME_COUNT_INVALID capacity=%u actual=%zu bytes=%zu\n",
                     expectedNames, names.size(), bytes.size());
        return false;
    }
    if (names.size() != expectedNames)
        std::fprintf(stderr,
                     "WORLD_WDT_NAME_COUNT capacity=%u populated=%zu bytes=%zu\n",
                     expectedNames, names.size(), bytes.size());
    return true;
}

bool LoadWdtHeader() {
    constexpr std::uint32_t kMainHeaderOffset = 0x94U;
    constexpr std::uint32_t kMainBodyOffset = 0x9cU;
    std::vector<std::uint8_t> header(kMainBodyOffset +
        kMapAreaCount * sizeof(DiskEntry));
    if (!ReadFileRange(0, header.data(), static_cast<std::uint32_t>(header.size()))) {
        std::fprintf(stderr, "WORLD_WDT_HEADER_READ_FAILED bytes=%zu\n",
                     header.size());
        return false;
    }
    if (ReadU32(header.data()) != MVER || ReadU32(header.data() + 8) != 18U ||
        ReadU32(header.data() + 0x0cU) != MPHD ||
        ReadU32(header.data() + kMainHeaderOffset) != MAIN ||
        ReadU32(header.data() + kMainHeaderOffset + 4U) !=
            kMapAreaCount * sizeof(DiskEntry)) {
        std::fprintf(stderr,
                     "WORLD_WDT_HEADER_INVALID t0=%08X version=%u mphd=%08X main=%08X mainBytes=%u\n",
                     ReadU32(header.data()), ReadU32(header.data() + 8U),
                     ReadU32(header.data() + 0x0cU),
                     ReadU32(header.data() + kMainHeaderOffset),
                     ReadU32(header.data() + kMainHeaderOffset + 4U));
        return false;
    }
    std::memcpy(s_world.areas.data(), header.data() + kMainBodyOffset,
                s_world.areas.size() * sizeof(DiskEntry));
    const std::uint32_t doodadNameCount = ReadU32(header.data() + 0x14U);
    const std::uint32_t mapObjNameCount = ReadU32(header.data() + 0x1cU);
    std::uint32_t cursor = static_cast<std::uint32_t>(header.size());
    if (!LoadNameChunk(cursor, MNDM, doodadNameCount, s_world.doodadNames) ||
        !LoadNameChunk(cursor, MNOM, mapObjNameCount, s_world.mapObjNames)) {
        return false;
    }
    std::fprintf(stderr, "WORLD_WDT_NAMES doodads=%zu mapObjects=%zu\n",
                 s_world.doodadNames.size(), s_world.mapObjNames.size());
    return true;
}

bool ParseDoodadDefs(TerrainArea& area, unsigned areaX, unsigned areaY,
                     const std::uint8_t* bytes, std::size_t size) {
    (void)areaX;
    (void)areaY;
    if (!bytes || size < 0x50U || ReadU32(bytes) != MHDR) return false;
    const std::uint32_t relativeOffset = ReadU32(bytes + 20U);
    if (relativeOffset == 0U) return true;
    const std::uint32_t chunkOffset = 8U + relativeOffset;
    if (chunkOffset + 8U > size || ReadU32(bytes + chunkOffset) != MDDF)
        return false;
    const std::uint32_t chunkBytes = ReadU32(bytes + chunkOffset + 4U);
    if (chunkBytes > size - chunkOffset - 8U || chunkBytes % 36U != 0U)
        return false;

    // CMapChunk::CreateRefs passes the fixed (17066.666, 17066.666, 0)
    // vector to CMap::CreateDoodadDef.  SMDoodadDef positions are already
    // map-global; applying the ADT corner here translates them a second time.
    const NTempest::C3Vector mapOrigin{kMapOrigin, kMapOrigin, 0.0F};
    constexpr float kDegreesToRadians = 0.017453292F;
    area.doodadDefs.reserve(chunkBytes / 36U);
    for (std::uint32_t offset = 0; offset < chunkBytes; offset += 36U) {
        const std::uint8_t* disk = bytes + chunkOffset + 8U + offset;
        TerrainDoodadDef definition;
        definition.nameId = ReadU32(disk);
        definition.uniqueId = ReadU32(disk + 4U);
        definition.position = {
            mapOrigin.x - ReadFloat(disk + 16U),
            mapOrigin.y - ReadFloat(disk + 8U),
            mapOrigin.z + ReadFloat(disk + 12U),
        };
        definition.rotation = {
            ReadFloat(disk + 28U) * kDegreesToRadians,
            ReadFloat(disk + 20U) * kDegreesToRadians,
            ReadFloat(disk + 24U) * kDegreesToRadians + 3.1415927F,
        };
        definition.scale = static_cast<float>(ReadU16(disk + 32U)) /
            1024.0F;
        definition.flags = ReadU16(disk + 34U);
        area.doodadDefs.push_back(definition);
    }
    return true;
}

bool ParseMapObjDefs(TerrainArea& area, unsigned areaX, unsigned areaY,
                     const std::uint8_t* bytes, std::size_t size) {
    (void)areaX;
    (void)areaY;
    if (!bytes || size < 0x50U || ReadU32(bytes) != MHDR) return false;
    const std::uint32_t relativeOffset = ReadU32(bytes + 28U);
    if (relativeOffset == 0U) return true;
    const std::uint32_t chunkOffset = 8U + relativeOffset;
    if (chunkOffset + 8U > size || ReadU32(bytes + chunkOffset) != MODF)
        return false;
    const std::uint32_t chunkBytes = ReadU32(bytes + chunkOffset + 4U);
    if (chunkBytes > size - chunkOffset - 8U || chunkBytes % 64U != 0U)
        return false;

    const NTempest::C3Vector mapOrigin{kMapOrigin, kMapOrigin, 0.0F};
    constexpr float kDegreesToRadians = 0.017453292F;
    area.mapObjDefs.reserve(chunkBytes / 64U);
    for (std::uint32_t offset = 0; offset < chunkBytes; offset += 64U) {
        const std::uint8_t* disk = bytes + chunkOffset + 8U + offset;
        TerrainMapObjDef definition;
        definition.nameId = ReadU32(disk);
        definition.uniqueId = ReadU32(disk + 4U);
        definition.position = {
            mapOrigin.x - ReadFloat(disk + 16U),
            mapOrigin.y - ReadFloat(disk + 8U),
            mapOrigin.z + ReadFloat(disk + 12U),
        };
        definition.rotation = {
            ReadFloat(disk + 28U) * kDegreesToRadians,
            ReadFloat(disk + 20U) * kDegreesToRadians,
            ReadFloat(disk + 24U) * kDegreesToRadians + 3.1415927F,
        };
        definition.flags = ReadU16(disk + 56U);
        definition.doodadSet = ReadU16(disk + 58U);
        definition.nameSet = ReadU16(disk + 60U);
        area.mapObjDefs.push_back(definition);
    }
    return true;
}

bool ParseTextureNames(TerrainArea& area, const std::uint8_t* bytes,
                       std::size_t size) {
    if (!bytes || size < 0x50U || ReadU32(bytes) != MHDR ||
        ReadU32(bytes + 4U) < 8U) {
        return false;
    }
    const std::uint32_t mtexOffset = 8U + ReadU32(bytes + 12U);
    if (mtexOffset + 8U > size || ReadU32(bytes + mtexOffset) != MTEX)
        return false;
    const std::uint32_t namesBytes = ReadU32(bytes + mtexOffset + 4U);
    if (namesBytes > size - mtexOffset - 8U) return false;

    const CGxTexFlags textureFlags(GxTex_LinearMipNearest, true, true,
                                   false, false, false, 1U);
    std::size_t cursor = 0;
    while (cursor < namesBytes) {
        const char* name = reinterpret_cast<const char*>(bytes + mtexOffset + 8U + cursor);
        const std::size_t remaining = namesBytes - cursor;
        const std::size_t length = strnlen(name, remaining);
        if (length == remaining) return false;
        HTEXTURE__* texture = TextureCreate(name, nullptr, textureFlags.bits, 0);
        if (!texture) return false;
        area.textures.push_back(texture);
        cursor += length + 1U;
    }
    return !area.textures.empty();
}

TerrainArea* LoadArea(unsigned areaX, unsigned areaY) {
    if (areaX >= 64U || areaY >= 64U) return nullptr;
    const unsigned areaIndex = areaY * 64U + areaX;
    if (auto found = s_world.loadedAreas.find(areaIndex);
        found != s_world.loadedAreas.end()) {
        return &found->second;
    }
    const DiskEntry& entry = s_world.areas[areaIndex];
    if (!entry.offset || !entry.size || entry.size >= 0x28000U) return nullptr;
    std::vector<std::uint8_t> bytes(entry.size);
    if (!ReadFileRange(entry.offset, bytes.data(), entry.size) ||
        bytes.size() < 0x1050U || ReadU32(bytes.data()) != MHDR) {
        return nullptr;
    }
    const std::uint32_t mhdrSize = ReadU32(bytes.data() + 4U);
    const std::uint32_t mcinOffset = 8U + mhdrSize;
    if (mcinOffset + 8U + sizeof(DiskEntry) * 256U > bytes.size() ||
        ReadU32(bytes.data() + mcinOffset) != MCIN ||
        ReadU32(bytes.data() + mcinOffset + 4U) != sizeof(DiskEntry) * 256U) {
        return nullptr;
    }
    TerrainArea area;
    std::memcpy(area.chunks.data(), bytes.data() + mcinOffset + 8U,
                area.chunks.size() * sizeof(DiskEntry));
    if (!ParseTextureNames(area, bytes.data(), bytes.size())) {
        ReleaseArea(area);
        return nullptr;
    }
    if (!ParseDoodadDefs(area, areaX, areaY, bytes.data(), bytes.size())) {
        ReleaseArea(area);
        return nullptr;
    }
    if (!ParseMapObjDefs(area, areaX, areaY, bytes.data(), bytes.size())) {
        ReleaseArea(area);
        return nullptr;
    }
    return &s_world.loadedAreas.emplace(areaIndex, std::move(area)).first->second;
}

unsigned OuterVertex(unsigned x, unsigned y) {
    return x < 8U ? x * 17U + y : 136U + y;
}

unsigned InnerVertex(unsigned x, unsigned y) {
    return x * 17U + 9U + y;
}

void BuildIndices(TerrainChunk& chunk) {
    chunk.indices.reserve(8U * 8U * 12U);
    for (unsigned x = 0; x < 8U; ++x) {
        for (unsigned y = 0; y < 8U; ++y) {
            const std::uint16_t topLeft = static_cast<std::uint16_t>(OuterVertex(x, y));
            const std::uint16_t topRight = static_cast<std::uint16_t>(OuterVertex(x + 1U, y));
            const std::uint16_t bottomRight = static_cast<std::uint16_t>(OuterVertex(x + 1U, y + 1U));
            const std::uint16_t bottomLeft = static_cast<std::uint16_t>(OuterVertex(x, y + 1U));
            const std::uint16_t center = static_cast<std::uint16_t>(InnerVertex(x, y));
            const std::uint16_t faces[] = {
                topLeft, topRight, center,
                topRight, bottomRight, center,
                bottomRight, bottomLeft, center,
                bottomLeft, topLeft, center,
            };
            chunk.indices.insert(chunk.indices.end(), std::begin(faces), std::end(faces));
        }
    }
}

void BuildVertices(TerrainChunk& chunk, const std::uint8_t* bytes,
                   std::size_t size) {
    constexpr std::size_t kHeightOffset = 0x88U;
    if (!bytes || kHeightOffset + kChunkVertexCount * sizeof(float) > size)
        return;
    const float worldX = kMapOrigin - static_cast<float>(chunk.globalY) * kChunkSize;
    const float worldY = kMapOrigin - static_cast<float>(chunk.globalX) * kChunkSize;
    const float step = -kChunkSize / 8.0F;
    chunk.corner = {
        worldX,
        worldY,
        ReadFloat(bytes + kHeightOffset),
    };
    unsigned vertex = 0;
    for (unsigned x = 0; x < 9U; ++x) {
        for (unsigned y = 0; y < 9U; ++y) {
            chunk.vertices[vertex] = {
                static_cast<float>(x) * step,
                static_cast<float>(y) * step,
                ReadFloat(bytes + kHeightOffset + (x * 9U + y) * 4U)
                    - chunk.corner.z,
            };
            chunk.normals[vertex] = {0.0F, 0.0F, 1.0F};
            // CMapChunk::CreateStaticData stores terrain coordinates in the
            // texture-axis order: the inner (ADT Y) index is S and the outer
            // (ADT X) index is T.  The world conversion swaps those ADT axes,
            // so feeding {x, y} here rotates every authored MCAL layer.
            chunk.textureCoordinates[vertex] = {static_cast<float>(y),
                                                 static_cast<float>(x)};
            // Recovered RenderLayers maps the 64x64 alpha texture over
            // 0..0.9765625 of a chunk to retain the replicated edge texels.
            chunk.alphaTextureCoordinates[vertex] = {
                static_cast<float>(y) * 0.1220703125F,
                static_cast<float>(x) * 0.1220703125F,
            };
            chunk.vertexColors[vertex] = NTempest::CImVector(0xffffffffU);
            ++vertex;
        }
        if (x < 8U) {
            for (unsigned y = 0; y < 8U; ++y) {
                chunk.vertices[vertex] = {
                    (static_cast<float>(x) + 0.5F) * step,
                    (static_cast<float>(y) + 0.5F) * step,
                    ReadFloat(bytes + kHeightOffset + (81U + x * 8U + y) * 4U)
                        - chunk.corner.z,
                };
                chunk.normals[vertex] = {0.0F, 0.0F, 1.0F};
                chunk.textureCoordinates[vertex] = {static_cast<float>(y) + 0.5F,
                                                     static_cast<float>(x) + 0.5F};
                chunk.alphaTextureCoordinates[vertex] = {
                    0.0625F + static_cast<float>(y) * 0.1220703125F,
                    0.0625F + static_cast<float>(x) * 0.1220703125F,
                };
                chunk.vertexColors[vertex] = NTempest::CImVector(0xffffffffU);
                ++vertex;
            }
        }
    }
    BuildIndices(chunk);
}

void TerrainAlphaTextureCallback(EGxTexCommand command, unsigned width,
                                 unsigned, unsigned, unsigned, void* userArg,
                                 unsigned* stride, const void** texels) {
    auto* mask = static_cast<TerrainAlphaMask*>(userArg);
    if (!mask || command != GxTex_Lock) return;
    if (stride) *stride = width * sizeof(std::uint32_t);
    if (texels) *texels = mask->pixels.data();
}

std::uint8_t LayerAlphaPixel(const TerrainChunk& chunk,
                             const TerrainLayer& layer, unsigned pixel) {
    if (!(layer.properties & 0x100U) || chunk.alpha.empty()) return 255U;
    const std::size_t byteOffset = static_cast<std::size_t>(layer.alphaOffset)
        + pixel / 2U;
    if (byteOffset >= chunk.alpha.size()) return 0U;
    const std::uint8_t packed = chunk.alpha[byteOffset];
    const std::uint8_t nibble = (pixel & 1U) ? packed >> 4U : packed & 0x0fU;
    return static_cast<std::uint8_t>(nibble << 4U);
}

bool CreateLayerAlphaMask(TerrainChunk& chunk, unsigned layerIndex) {
    if (layerIndex == 0U || layerIndex >= chunk.layerCount ||
        !(chunk.layers[layerIndex].properties & 0x100U)) {
        return true;
    }
    auto mask = std::make_unique<TerrainAlphaMask>();
    for (unsigned pixel = 0; pixel < mask->pixels.size(); ++pixel) {
        const std::uint8_t alpha = LayerAlphaPixel(
            chunk, chunk.layers[layerIndex], pixel);
        mask->pixels[pixel] = 0x00ffffffU |
            (static_cast<std::uint32_t>(alpha) << 24U);
    }
    const CGxTexFlags flags(GxTex_Linear, false, false, true,
                            false, false, 1U);
    if (!GxTexCreate(64U, 64U, GxTex_Argb8888, flags, mask.get(),
                     TerrainAlphaTextureCallback, &mask->texture)) {
        return false;
    }
    chunk.alphaMasks[layerIndex] = std::move(mask);
    return true;
}

bool ParseLayers(TerrainChunk& chunk, const std::uint8_t* bytes,
                 std::size_t size) {
    constexpr std::size_t kDataBase = 0x88U;
    if (!bytes || size < kDataBase || ReadU32(bytes) != MCNK) return false;
    const std::uint32_t layerOffset = static_cast<std::uint32_t>(kDataBase) +
        ReadU32(bytes + 0x28U);
    if (layerOffset + 8U > size || ReadU32(bytes + layerOffset) != MCLY)
        return false;
    const std::uint32_t layerBytes = ReadU32(bytes + layerOffset + 4U);
    if (layerBytes > size - layerOffset - 8U || layerBytes % 16U != 0U)
        return false;
    chunk.layerCount = (std::min)(4U, layerBytes / 16U);
    for (unsigned index = 0; index < chunk.layerCount; ++index) {
        const std::uint8_t* layer = bytes + layerOffset + 8U + index * 16U;
        chunk.layers[index] = {ReadU32(layer), ReadU32(layer + 4U),
                               ReadU32(layer + 8U), ReadU32(layer + 12U)};
    }

    const std::uint32_t alphaOffset = static_cast<std::uint32_t>(kDataBase) +
        ReadU32(bytes + 0x30U);
    const std::uint32_t alphaBytes = ReadU32(bytes + 0x34U);
    if (alphaBytes && alphaOffset <= size && alphaBytes <= size - alphaOffset)
        chunk.alpha.assign(bytes + alphaOffset, bytes + alphaOffset + alphaBytes);
    for (unsigned index = 1U; index < chunk.layerCount; ++index) {
        if (!CreateLayerAlphaMask(chunk, index)) return false;
    }
    return chunk.layerCount != 0;
}

bool InstantiateDoodad(const TerrainArea& area, std::uint32_t definitionIndex) {
    if (definitionIndex >= area.doodadDefs.size()) return false;
    const TerrainDoodadDef& definition = area.doodadDefs[definitionIndex];
    if (s_world.doodadIds.contains(definition.uniqueId)) return true;
    if (definition.nameId >= s_world.doodadNames.size()) return false;

    static const char* sequenceNames[] = {"Stand"};
    CModelCreate createData;
    createData.flags = 10242U;
    createData.sequenceNames = sequenceNames;
    createData.numSequences = 1U;
    const std::string& modelName = s_world.doodadNames[definition.nameId];
    std::shared_ptr<CModel> source;
    if (const auto found = s_world.doodadTemplates.find(modelName);
        found != s_world.doodadTemplates.end()) {
        source = found->second;
    } else {
        source = ModelCreate(modelName.c_str(), &createData);
        if (source && source->data && ModelIsLoaded(*source))
            s_world.doodadTemplates.emplace(modelName, source);
    }
    std::shared_ptr<CModel> model = source ? ModelDuplicate(*source) : nullptr;
    if (!model || !model->data || !ModelIsLoaded(*model)) {
        static unsigned failedReports = 0U;
        if (failedReports < 16U) {
            std::fprintf(stderr,
                         "WORLD_DOODAD_LOAD_FAILED id=%u nameId=%u file=%s\n",
                         definition.uniqueId, definition.nameId,
                         modelName.c_str());
            ++failedReports;
        }
        return false;
    }

    ModelSetSequence(*model, 0U);
    ModelSetLightSelectCallback(*model, &CWorld::SelectLight, nullptr);
    s_world.doodadIds.insert(definition.uniqueId);
    s_world.doodads.push_back({definition.uniqueId, definition.position,
                               definition.rotation, definition.scale,
                               std::move(model)});
    return true;
}

bool InstantiateMapObj(const TerrainArea& area,
                       std::uint32_t definitionIndex) {
    if (definitionIndex >= area.mapObjDefs.size()) return false;
    const TerrainMapObjDef& definition = area.mapObjDefs[definitionIndex];
    if (s_world.mapObjIds.contains(definition.uniqueId)) return true;
    if (definition.nameId >= s_world.mapObjNames.size()) return false;
    const std::string& mapObjName = s_world.mapObjNames[definition.nameId];
    std::shared_ptr<CMapObj> mapObj;
    if (const auto found = s_world.mapObjTemplates.find(mapObjName);
        found != s_world.mapObjTemplates.end()) {
        mapObj = found->second;
    } else {
        mapObj = std::make_shared<CMapObj>();
        if (!mapObj->Read(mapObjName.c_str())) {
            static unsigned failedReports = 0U;
            if (failedReports < 16U) {
                std::fprintf(stderr,
                             "WORLD_MAPOBJ_LOAD_FAILED id=%u nameId=%u file=%s\n",
                             definition.uniqueId, definition.nameId,
                             mapObjName.c_str());
                ++failedReports;
            }
            return false;
        }
        s_world.mapObjTemplates.emplace(mapObjName, mapObj);
    }
    s_world.mapObjIds.insert(definition.uniqueId);
    s_world.mapObjects.push_back({
        definition.uniqueId, definition.position, definition.rotation,
        definition.flags, definition.doodadSet, std::move(mapObj),
    });
    return true;
}

bool CreateChunkRefs(const TerrainArea& area, const std::uint8_t* bytes,
                     std::size_t size) {
    constexpr std::size_t kDataBase = 0x88U;
    if (!bytes || size < kDataBase) return false;
    const std::uint32_t doodadRefCount = ReadU32(bytes + 0x1cU);
    const std::uint32_t mapObjRefCount = ReadU32(bytes + 0x44U);
    if (doodadRefCount == 0U && mapObjRefCount == 0U) return true;
    const std::uint32_t refsOffset = static_cast<std::uint32_t>(kDataBase) +
        ReadU32(bytes + 0x2cU);
    if (refsOffset + 8U > size || ReadU32(bytes + refsOffset) != MCRF)
        return false;
    const std::uint32_t refsBytes = ReadU32(bytes + refsOffset + 4U);
    const std::uint64_t requiredBytes =
        static_cast<std::uint64_t>(doodadRefCount + mapObjRefCount) * 4U;
    if (refsBytes > size - refsOffset - 8U || requiredBytes > refsBytes)
        return false;
    for (std::uint32_t index = 0; index < doodadRefCount; ++index) {
        InstantiateDoodad(area,
            ReadU32(bytes + refsOffset + 8U + index * 4U));
    }
    for (std::uint32_t index = 0; index < mapObjRefCount; ++index) {
        InstantiateMapObj(area,
            ReadU32(bytes + refsOffset + 8U +
                    (doodadRefCount + index) * 4U));
    }
    return true;
}

bool LoadChunk(unsigned globalX, unsigned globalY) {
    if (globalX >= 1024U || globalY >= 1024U) return false;
    const unsigned areaX = globalX / 16U;
    const unsigned areaY = globalY / 16U;
    TerrainArea* area = LoadArea(areaX, areaY);
    if (!area) return false;
    const unsigned localX = globalX & 15U;
    const unsigned localY = globalY & 15U;
    const DiskEntry& entry = area->chunks[localY * 16U + localX];
    if (!entry.offset || !entry.size || entry.size >= 0x3a98U) return false;
    std::vector<std::uint8_t> bytes(entry.size);
    if (!ReadFileRange(entry.offset, bytes.data(), entry.size) ||
        bytes.size() < 0x4a0U || ReadU32(bytes.data()) != MCNK ||
        ReadU32(bytes.data() + 0x0cU) != localX ||
        ReadU32(bytes.data() + 0x10U) != localY) {
        return false;
    }
    TerrainChunk chunk;
    chunk.globalX = globalX;
    chunk.globalY = globalY;
    chunk.textures = &area->textures;
    BuildVertices(chunk, bytes.data(), bytes.size());
    if (chunk.indices.empty() || !ParseLayers(chunk, bytes.data(), bytes.size()))
        return false;
    if (!CreateChunkRefs(*area, bytes.data(), bytes.size())) return false;
    s_world.chunks.push_back(std::move(chunk));
    return true;
}

void AddCollisionFacet(std::vector<NTempest::CFacet>& destination,
                       const NTempest::C3Vector& a,
                       const NTempest::C3Vector& b,
                       const NTempest::C3Vector& c) {
    NTempest::CFacet facet;
    facet.Set(a, b, c);
    if (facet.plane.n.SquaredMag() > 0.0000001F)
        destination.push_back(facet);
}

void BuildWorldCollision() {
    s_world.terrainCollision.clear();
    s_world.objectCollision.clear();

    std::size_t terrainIndexCount = 0U;
    for (const TerrainChunk& chunk : s_world.chunks)
        terrainIndexCount += chunk.indices.size();
    s_world.terrainCollision.reserve(terrainIndexCount / 3U);
    for (const TerrainChunk& chunk : s_world.chunks) {
        for (std::size_t i = 0; i + 2U < chunk.indices.size(); i += 3U) {
            const std::uint16_t ia = chunk.indices[i];
            const std::uint16_t ib = chunk.indices[i + 1U];
            const std::uint16_t ic = chunk.indices[i + 2U];
            if (ia >= chunk.vertices.size() || ib >= chunk.vertices.size() ||
                ic >= chunk.vertices.size()) {
                continue;
            }
            AddCollisionFacet(s_world.terrainCollision,
                              chunk.corner + chunk.vertices[ia],
                              chunk.corner + chunk.vertices[ib],
                              chunk.corner + chunk.vertices[ic]);
        }
    }

    std::size_t mapObjectFacets = 0U;
    for (const WorldMapObj& instance : s_world.mapObjects) {
        if (!instance.mapObj || !instance.mapObj->IsLoaded()) continue;
        const NTempest::C44Matrix transform = BuildMapObjPlacement(
            instance.position, instance.rotation);
        for (const CMapObjGroup& group : instance.mapObj->groups) {
            if (!group.loaded) continue;
            for (std::size_t i = 0; i + 2U < group.indices.size(); i += 3U) {
                const std::uint16_t ia = group.indices[i];
                const std::uint16_t ib = group.indices[i + 1U];
                const std::uint16_t ic = group.indices[i + 2U];
                if (ia >= group.vertices.size() || ib >= group.vertices.size() ||
                    ic >= group.vertices.size()) {
                    continue;
                }
                AddCollisionFacet(s_world.objectCollision,
                                  transform * group.vertices[ia],
                                  transform * group.vertices[ib],
                                  transform * group.vertices[ic]);
                ++mapObjectFacets;
            }
        }
    }

    std::size_t doodadFacets = 0U;
    std::size_t doodadsWithCollision = 0U;
    for (const WorldDoodad& doodad : s_world.doodads) {
        if (!doodad.model || !doodad.model->shared ||
            !doodad.model->shared->collision) {
            continue;
        }
        const CCollisionData& collision = *doodad.model->shared->collision;
        NTempest::C34Matrix transform;
        transform.Translate(doodad.position);
        transform *= NTempest::C34Matrix::Rotation(
            {0.0F, 0.0F, 1.0F}, doodad.rotation.z);
        transform *= NTempest::C34Matrix::Rotation(
            {0.0F, 1.0F, 0.0F}, doodad.rotation.y);
        transform *= NTempest::C34Matrix::Rotation(
            {1.0F, 0.0F, 0.0F}, doodad.rotation.x);
        transform.Scale(doodad.scale);
        bool added = false;
        for (std::size_t i = 0; i + 2U < collision.indices.size(); i += 3U) {
            const std::uint16_t ia = collision.indices[i];
            const std::uint16_t ib = collision.indices[i + 1U];
            const std::uint16_t ic = collision.indices[i + 2U];
            if (ia >= collision.vertices.size() || ib >= collision.vertices.size() ||
                ic >= collision.vertices.size()) {
                continue;
            }
            AddCollisionFacet(s_world.objectCollision,
                              transform * collision.vertices[ia],
                              transform * collision.vertices[ib],
                              transform * collision.vertices[ic]);
            ++doodadFacets;
            added = true;
        }
        if (added) ++doodadsWithCollision;
    }

    std::fprintf(stderr,
                 "WORLD_COLLISION terrain=%zu mapObject=%zu doodad=%zu "
                 "doodadsAuthored=%zu/%zu totalObjects=%zu\n",
                 s_world.terrainCollision.size(), mapObjectFacets,
                 doodadFacets, doodadsWithCollision, s_world.doodads.size(),
                 s_world.objectCollision.size());
}

bool IntersectFacetSet(const std::vector<NTempest::CFacet>& facets,
                       const NTempest::C3Vector& start,
                       const NTempest::C3Vector& end,
                       float& nearest,
                       NTempest::C3Vector& normal) {
    const NTempest::C3Vector delta = end - start;
    const float minX = (std::min)(start.x, end.x) - 0.001F;
    const float minY = (std::min)(start.y, end.y) - 0.001F;
    const float minZ = (std::min)(start.z, end.z) - 0.001F;
    const float maxX = (std::max)(start.x, end.x) + 0.001F;
    const float maxY = (std::max)(start.y, end.y) + 0.001F;
    const float maxZ = (std::max)(start.z, end.z) + 0.001F;
    bool hit = false;
    for (const NTempest::CFacet& facet : facets) {
        const auto& a = facet.vertices[0];
        const auto& b = facet.vertices[1];
        const auto& c = facet.vertices[2];
        if ((a.x < minX && b.x < minX && c.x < minX) ||
            (a.y < minY && b.y < minY && c.y < minY) ||
            (a.z < minZ && b.z < minZ && c.z < minZ) ||
            (a.x > maxX && b.x > maxX && c.x > maxX) ||
            (a.y > maxY && b.y > maxY && c.y > maxY) ||
            (a.z > maxZ && b.z > maxZ && c.z > maxZ)) {
            continue;
        }
        float fraction = 0.0F;
        if (NTempest::Intersect(facet, start, delta, fraction) &&
            fraction >= 0.00001F && fraction <= 1.0F && fraction < nearest) {
            nearest = fraction;
            normal = facet.plane.n;
            if (NTempest::C3Vector::Dot(normal, delta) > 0.0F)
                normal *= -1.0F;
            hit = true;
        }
    }
    return hit;
}

std::uint8_t LayerAlpha(const TerrainChunk& chunk, const TerrainLayer& layer,
                        unsigned vertex) {
    const NTempest::C2Vector& uv = chunk.textureCoordinates[vertex];
    const unsigned x = (std::min)(63U, static_cast<unsigned>(uv.x * 8.0F));
    const unsigned y = (std::min)(63U, static_cast<unsigned>(uv.y * 8.0F));
    return LayerAlphaPixel(chunk, layer, y * 64U + x);
}

bool RenderChunkLayer(TerrainChunk& chunk, unsigned layerIndex) {
    if (!chunk.textures || layerIndex >= chunk.layerCount) return false;
    const TerrainLayer& layer = chunk.layers[layerIndex];
    if (layer.textureId >= chunk.textures->size()) return false;
    HTEXTURE__* texture = (*chunk.textures)[layer.textureId];
    CGxTex* gxTexture = texture ? TextureGetGxTex(texture, 1, nullptr) : nullptr;
    if (!gxTexture) return false;

    if (layerIndex == 0U) {
        std::fill(chunk.vertexColors.begin(), chunk.vertexColors.end(),
                  NTempest::CImVector(0xffffffffU));
        GxRsSet(GxRs_Blend, GxBlend_Opaque);
        GxRsSet(GxRs_DepthWrite, 1);
        GxRsSet(GxRs_PolygonOffset, 0.0F);
        GxRsSet(GxRs_Texture1, static_cast<CGxTex*>(nullptr));
    } else {
        const TerrainAlphaMask* mask = chunk.alphaMasks[layerIndex].get();
        if (!mask || !mask->texture) return false;
        std::fill(chunk.vertexColors.begin(), chunk.vertexColors.end(),
                  NTempest::CImVector(0xffffffffU));
        GxRsSet(GxRs_Blend, GxBlend_Alpha);
        GxRsSet(GxRs_DepthWrite, 0);
        GxRsSet(GxRs_PolygonOffset, 0.0F);
        GxRsSet(GxRs_Texture1, mask->texture);
    }

    GxRsSet(GxRs_Texture0, gxTexture);
    GxRsSet(GxRs_TexBlend0, static_cast<int>(GxTexBlend_Mod));
    GxRsSet(GxRs_TexBlend1, static_cast<int>(GxTexBlend_Mod));
    GxPrimLockVertexPtrs(static_cast<unsigned>(chunk.vertices.size()),
                         chunk.vertices.data(), sizeof(NTempest::C3Vector),
                         chunk.normals.data(), sizeof(NTempest::C3Vector),
                         chunk.vertexColors.data(), sizeof(NTempest::CImVector),
                         nullptr, 0,
                         chunk.textureCoordinates.data(), sizeof(NTempest::C2Vector),
                         chunk.alphaTextureCoordinates.data(), sizeof(NTempest::C2Vector));
    GxPrimDrawElements(GxPrim_Triangles, static_cast<unsigned>(chunk.indices.size()),
                       chunk.indices.data());
    GxPrimUnlockVertexPtrs();
    return true;
}

bool PointInTriangle(float x, float y,
                     const NTempest::C3Vector& a,
                     const NTempest::C3Vector& b,
                     const NTempest::C3Vector& c,
                     float& z) {
    const float denominator = (b.y - c.y) * (a.x - c.x) +
        (c.x - b.x) * (a.y - c.y);
    if (std::fabs(denominator) <= std::numeric_limits<float>::epsilon())
        return false;
    const float wa = ((b.y - c.y) * (x - c.x) +
        (c.x - b.x) * (y - c.y)) / denominator;
    const float wb = ((c.y - a.y) * (x - c.x) +
        (a.x - c.x) * (y - c.y)) / denominator;
    const float wc = 1.0F - wa - wb;
    constexpr float kEdgeTolerance = -0.0001F;
    if (wa < kEdgeTolerance || wb < kEdgeTolerance || wc < kEdgeTolerance)
        return false;
    z = wa * a.z + wb * b.z + wc * c.z;
    return true;
}

const char* MapDirectory(int mapID) {
    switch (mapID) {
    case 0: return "Azeroth";
    case 1: return "Kalimdor";
    case 2: return "UnderMine";
    case 13: return "test";
    case 25: return "ScottTest";
    case 29: return "CashTest";
    case 30: return "PVPZone01";
    case 33: return "Shadowfang";
    case 34: return "StormwindJail";
    case 36: return "DeadminesInstance";
    case 43: return "WailingCaverns";
    case 47: return "RazorfenKraulInstance";
    case 48: return "Blackfathom";
    case 70: return "Uldaman";
    case 90: return "GnomeragonInstance";
    case 109: return "SunkenTemple";
    case 129: return "RazorfenDowns";
    case 189: return "MonasteryInstances";
    case 209: return "TanarisInstance";
    case 229: return "BlackRockSpire";
    case 230: return "BlackrockDepths";
    case 249: return "OnyxiasLairInstance";
    default: return nullptr;
    }
}

} // namespace

void CWorld::Initialize() {
    ResetWorld();
    ClientServices_SetMessageHandler(kSmsgDeathNotify,
                                     WorldDeathNotifyHandler, nullptr);
}

void CWorld::Destroy() {
    ClientServices_ClearMessageHandler(kSmsgDeathNotify);
    ResetWorld();
}

bool CWorld::LoadMap(const char* mapName, const NTempest::C3Vector& position,
                     int preLoad) {
    (void)preLoad;
    ResetWorld();
    if (!mapName || !*mapName) return false;
    s_world.mapName = mapName;
    s_world.playerPosition = position;

    char lightPath[256]{};
    std::snprintf(lightPath, sizeof(lightPath),
                  "World\\Maps\\%s\\Lights.lit", mapName);
    const bool loadedLights = LoadLightsAndFog(lightPath, &g_areaLights);
    DayNightInitialize(nullptr);
    DayNightForceFullUpdate();
    std::fprintf(stderr, "WORLD_LIGHTS path=%s loaded=%u groups=%u\n",
                 lightPath, loadedLights ? 1U : 0U,
                 g_areaLights.m_lightData.m_count);
    if (CGlueMgr::m_characterInfo) {
        s_world.playerCharacter.m_characterInfo = *CGlueMgr::m_characterInfo;
        s_world.playerCharacter.UpdateCharacterInfo(nullptr, nullptr, nullptr);
        s_world.playerCharacterReady =
            s_world.playerCharacter.m_characterModelOwner != nullptr;
        if (s_world.playerCharacter.m_characterModelOwner) {
            ModelSetLightSelectCallback(
                *s_world.playerCharacter.m_characterModelOwner,
                &CWorld::SelectLight, nullptr);
            if (auto* complex = dynamic_cast<CModelComplex*>(
                    s_world.playerCharacter.m_characterModelOwner->data.get())) {
                for (std::size_t textureIndex = 0;
                     textureIndex < complex->m_textures.size(); ++textureIndex) {
                    const CModelTexture& modelTexture =
                        complex->m_textures[textureIndex];
                    if (modelTexture.replaceableId == 0U)
                        continue;
                    HTEXTURE__* handle = modelTexture.handle;
                    CGxTex* gxTexture = TextureGetGxTex(handle, 1, nullptr);
                    std::fprintf(
                        stderr,
                        "WORLD_PLAYER_TEXTURE slot=%zu replaceable=%u file=%s "
                        "handle=%p gx=%p size=%ux%u mips=%zu\n",
                        textureIndex, modelTexture.replaceableId,
                        handle ? handle->filename.c_str() : "<null>",
                        static_cast<void*>(handle), static_cast<void*>(gxTexture),
                        gxTexture ? gxTexture->width : 0U,
                        gxTexture ? gxTexture->height : 0U,
                        handle ? handle->mipBits.size() : 0U);
                }
            }
        }
        std::fprintf(stderr,
                     "WORLD_PLAYER_CREATE guid=%llu race=%u sex=%u ready=%u\n",
                     static_cast<unsigned long long>(
                         s_world.playerCharacter.m_characterInfo.guid),
                     static_cast<unsigned>(
                         s_world.playerCharacter.m_characterInfo.raceID),
                     static_cast<unsigned>(
                         s_world.playerCharacter.m_characterInfo.sexID),
                     s_world.playerCharacterReady ? 1U : 0U);
    }

    char archivePath[MAX_PATH]{};
    char memberPath[MAX_PATH]{};
    sprintf_s(archivePath, "Data\\World\\Maps\\%s\\%s.wdt.MPQ", mapName, mapName);
    sprintf_s(memberPath, "%s.wdt", mapName);
    if (!SFile::OpenArchive(archivePath, 10, 0, &s_world.archive)) {
        std::fprintf(stderr,
                     "WORLD_LOAD_OPEN_ARCHIVE_FAILED map=%s archive=%s error=%lu\n",
                     mapName, archivePath, GetLastError());
        ResetWorld();
        return false;
    }
    if (!SFile::OpenEx(s_world.archive, memberPath, 0, &s_world.wdt)) {
        std::fprintf(stderr,
                     "WORLD_LOAD_OPEN_MEMBER_FAILED map=%s member=%s error=%lu\n",
                     mapName, memberPath, GetLastError());
        ResetWorld();
        return false;
    }
    if (!LoadWdtHeader()) {
        std::fprintf(stderr, "WORLD_LOAD failed map=%s archive=%s error=%lu\n",
                     mapName, archivePath, GetLastError());
        ResetWorld();
        return false;
    }

    const int centerX = static_cast<int>(std::floor((kMapOrigin - position.y) * kChunkToIndex));
    const int centerY = static_cast<int>(std::floor((kMapOrigin - position.x) * kChunkToIndex));
    constexpr int kChunkRadius = 10;
    unsigned attempted = 0;
    unsigned loaded = 0;
    for (int y = centerY - kChunkRadius; y <= centerY + kChunkRadius; ++y) {
        for (int x = centerX - kChunkRadius; x <= centerX + kChunkRadius; ++x) {
            if (x < 0 || y < 0 || x >= 1024 || y >= 1024) continue;
            ++attempted;
            if (LoadChunk(static_cast<unsigned>(x), static_cast<unsigned>(y))) ++loaded;
        }
    }
    s_world.loaded = loaded != 0U;
    if (s_world.loaded)
        BuildWorldCollision();
    std::fprintf(stderr,
                 "WORLD_LOAD map=%s pos=(%.3f,%.3f,%.3f) center=(%d,%d) chunks=%u/%u areas=%zu doodads=%zu mapObjects=%zu\n",
                 mapName, position.x, position.y, position.z, centerX, centerY,
                 loaded, attempted, s_world.loadedAreas.size(),
                 s_world.doodads.size(), s_world.mapObjects.size());
    std::fflush(stderr);
    if (s_world.loaded)
        GxSceneSetClearColor(NTempest::CImVector(0xff315b82U));
    return s_world.loaded;
}

bool CWorld::LoadMap(int mapID, const NTempest::C3Vector& position, int preLoad) {
    const char* mapName = MapDirectory(mapID);
    return mapName && LoadMap(mapName, position, preLoad);
}

void CWorld::UnloadMap() {
    ResetWorld();
}

void CWorld::SetEnvironment() {
    // Recovered CWorld::SetEnvironment (0x663370) installs the interpolated
    // outdoor fog immediately before CWorld::Render.  The master switch was
    // temporarily disabled during renderer bring-up, so restore it here as
    // well as the original linear fog state.
    const DNInfo* info = DayNightGetInfo();
    GxMasterEnableSet(GxMasterEnable_Fog, true);
    GxRsSet(GxRs_FogStyle, static_cast<int>(GxFog_Linear));
    GxRsSet(GxRs_FogStart, info->fogInfo.start);
    GxRsSet(GxRs_FogEnd, info->fogInfo.end);
    GxRsSet(GxRs_FogDensity, 0.0F);
    GxRsSet(GxRs_FogColor, info->fogInfo.color);
    GxRsSet(GxRs_Fog, 1);
}

bool CWorld::Intersect(const NTempest::C3Vector& start,
                       const NTempest::C3Vector& end,
                       NTempest::C3Vector* hitPoint,
                       NTempest::C3Vector* hitNormal,
                       float* hitFraction,
                       bool includeTerrain,
                       bool includeObjects) {
    if (!s_world.loaded || (end - start).SquaredMag() <= 0.0000001F)
        return false;
    float nearest = 1.000001F;
    NTempest::C3Vector normal{};
    bool hit = false;
    if (includeTerrain)
        hit |= IntersectFacetSet(s_world.terrainCollision, start, end,
                                 nearest, normal);
    if (includeObjects)
        hit |= IntersectFacetSet(s_world.objectCollision, start, end,
                                 nearest, normal);
    if (!hit) return false;
    if (hitPoint) *hitPoint = start + (end - start) * nearest;
    if (hitNormal) *hitNormal = normal;
    if (hitFraction) *hitFraction = nearest;
    return true;
}

NTempest::C3Vector CWorld::ResolvePlayerMovement(
    const NTempest::C3Vector& start,
    const NTempest::C3Vector& desired,
    float radius,
    float height) {
    if (!s_world.loaded) return desired;
    NTempest::C3Vector position = start;
    NTempest::C3Vector goal = desired;
    const float clampedRadius = (std::max)(0.0F, radius);
    const float lower = (std::clamp)(height * 0.22F, 0.25F, 0.55F);
    const float upper = (std::max)(lower, height * 0.72F);
    const std::array<NTempest::C3Vector, 5> offsets = {{
        {},
        { clampedRadius, 0.0F, 0.0F},
        {-clampedRadius, 0.0F, 0.0F},
        {0.0F,  clampedRadius, 0.0F},
        {0.0F, -clampedRadius, 0.0F},
    }};

    // The recovered mover resolves a swept character volume from CWorld's
    // facets, then retries the residual displacement along the hit plane.
    // Sample the capsule body at two heights and its horizontal radius; this
    // preserves wall sliding while preventing the point-sized tunnelling that
    // occurred in the partial port.
    for (unsigned pass = 0U; pass < 2U; ++pass) {
        const NTempest::C3Vector displacement = goal - position;
        const float horizontalLength = std::sqrt(
            displacement.x * displacement.x + displacement.y * displacement.y);
        if (horizontalLength <= 0.00001F) break;

        float nearest = 1.000001F;
        NTempest::C3Vector nearestNormal{};
        for (float sampleHeight : {lower, upper}) {
            for (const NTempest::C3Vector& offset : offsets) {
                const NTempest::C3Vector rayStart =
                    position + offset + NTempest::C3Vector(0.0F, 0.0F,
                                                           sampleHeight);
                const NTempest::C3Vector rayEnd = rayStart + displacement;
                float fraction = 1.000001F;
                NTempest::C3Vector normal{};
                if (CWorld::Intersect(rayStart, rayEnd, nullptr, &normal,
                                      &fraction, true, true) &&
                    fraction < nearest) {
                    nearest = fraction;
                    nearestNormal = normal;
                }
            }
        }
        if (nearest > 1.0F) {
            position = goal;
            break;
        }

        const float skinFraction = 0.03F / horizontalLength;
        const float allowed = (std::clamp)(nearest - skinFraction, 0.0F, 1.0F);
        position += displacement * allowed;
        NTempest::C3Vector residual = displacement * (1.0F - allowed);
        nearestNormal.z = 0.0F;
        if (nearestNormal.SquaredMag() <= 0.00001F) break;
        nearestNormal.Normalize();
        const float intoWall = NTempest::C3Vector::Dot(residual, nearestNormal);
        if (intoWall < 0.0F)
            residual -= nearestNormal * intoWall;
        goal = position + residual;
    }
    return position;
}

float CWorld::CalcAltitude(float x, float y, float radius) {
    (void)radius;
    for (const TerrainChunk& chunk : s_world.chunks) {
        const float minX = chunk.corner.x - kChunkSize;
        const float minY = chunk.corner.y - kChunkSize;
        if (x < minX || x > chunk.corner.x ||
            y < minY || y > chunk.corner.y)
            continue;
        for (std::size_t index = 0; index + 2U < chunk.indices.size(); index += 3U) {
            const NTempest::C3Vector a =
                chunk.vertices[chunk.indices[index]] + chunk.corner;
            const NTempest::C3Vector b =
                chunk.vertices[chunk.indices[index + 1U]] + chunk.corner;
            const NTempest::C3Vector c =
                chunk.vertices[chunk.indices[index + 2U]] + chunk.corner;
            float altitude = 0.0F;
            if (PointInTriangle(x, y, a, b, c, altitude))
                return altitude;
        }
    }
    return std::numeric_limits<float>::quiet_NaN();
}

void CWorld::SelectLight(void*, NTempest::C3Vector,
                         const NTempest::C3Vector* cameraWorldPosition,
                         unsigned maxLightsToUse) {
    // CWorld::UpdateDayNight copies DNInfo's interpolated outdoor light into
    // CMap::sunLight.  CMap::GxuLightSelect then installs that sun in slot zero
    // when no local map lights apply.
    const DNInfo* dayNight = DayNightGetInfo();
    CGxLight sunLight;
    sunLight.m_flags = 1U;
    sunLight.m_dir = dayNight->lightInfo.dir;
    sunLight.m_ambColor = dayNight->lightInfo.ambColor;
    sunLight.m_dirColor = dayNight->lightInfo.dirColor;
    sunLight.m_specColor = dayNight->light.CloudArray[0];
    sunLight.m_specIntensity = 1.0F;
    static bool reportedSun = false;
    if (!reportedSun) {
        std::fprintf(stderr,
                     "WORLD_SUN progression=%.6f dir=(%.4f,%.4f,%.4f) "
                     "ambient=(%u,%u,%u,%u) diffuse=(%u,%u,%u,%u)\n",
                     dayNight->dayProgression,
                     sunLight.m_dir.x, sunLight.m_dir.y, sunLight.m_dir.z,
                     sunLight.m_ambColor.r, sunLight.m_ambColor.g,
                     sunLight.m_ambColor.b, sunLight.m_ambColor.a,
                     sunLight.m_dirColor.r, sunLight.m_dirColor.g,
                     sunLight.m_dirColor.b, sunLight.m_dirColor.a);
        reportedSun = true;
    }
    const NTempest::C3Vector camera = cameraWorldPosition
        ? *cameraWorldPosition : s_world.cameraPosition;
    if (maxLightsToUse != 0U) {
        GxLightSet(0U, &sunLight, camera);
        GxLightEnable(0U, true);
    }
    for (unsigned slot = 1U; slot < 8U; ++slot)
        GxLightEnable(slot, false);
}

void CWorld::SetCameraPosition(const NTempest::C3Vector& position) {
    s_world.cameraPosition = position;
    ParticleSystem2SetCameraWorldPosition(position);
}

void CWorld::RelocatePlayer(const NTempest::C3Vector& position,
                            float facing) {
    ClntObjMgrSetActivePosition(position);
    ClntObjMgrSetActiveFacing(facing);
    s_world.playerPosition = position;

    // The recovered near-teleport path calls CWorld::Preload when the jump is
    // large. Load the destination neighborhood only when it is not already
    // covered by the active collision set, preserving the current world and
    // death state while the teleport handshake finishes.
    if (s_world.loaded &&
        !std::isfinite(CalcAltitude(position.x, position.y, 1.0F))) {
        const int centerX = static_cast<int>(std::floor(
            (kMapOrigin - position.y) * kChunkToIndex));
        const int centerY = static_cast<int>(std::floor(
            (kMapOrigin - position.x) * kChunkToIndex));
        constexpr int kPreloadRadius = 10;
        unsigned loaded = 0U;
        for (int y = centerY - kPreloadRadius;
             y <= centerY + kPreloadRadius; ++y) {
            for (int x = centerX - kPreloadRadius;
                 x <= centerX + kPreloadRadius; ++x) {
                if (x < 0 || y < 0 || x >= 1024 || y >= 1024)
                    continue;
                loaded += LoadChunk(static_cast<unsigned>(x),
                                    static_cast<unsigned>(y)) ? 1U : 0U;
            }
        }
        if (loaded != 0U)
            BuildWorldCollision();
        std::fprintf(stderr,
                     "WORLD_TELEPORT_PRELOAD pos=(%.3f,%.3f,%.3f) chunks=%u\n",
                     position.x, position.y, position.z, loaded);
    }
    std::fprintf(stderr,
                 "WORLD_PLAYER_RELOCATE pos=(%.3f,%.3f,%.3f) facing=%.3f\n",
                 position.x, position.y, position.z, facing);
}

void CWorld::SendPlayerMovementUpdate(unsigned opcode, unsigned moveFlags) {
    SendPlayerMovementUpdatePacket(opcode, moveFlags);
}

void CWorld::BeginSpellCast(std::uint64_t casterGuid,
                            std::uint64_t targetGuid,
                            unsigned spellID,
                            unsigned castTimeMs) {
    const SpellVisualDefinition* definition = FindSpellVisual(spellID);
    if (!definition) return;
    SetUnitTransientSequence(casterGuid, definition->precastAnimation,
                             (std::max)(castTimeMs, 500U));
    NTempest::C3Vector position = UnitWorldPosition(casterGuid);
    position.z += 1.15F;
    SpawnStationarySpellEffect(definition->precastModel, casterGuid,
                               position, (std::max)(castTimeMs, 500U));
    std::fprintf(stderr,
                 "WORLD_SPELL_BEGIN spell=%u caster=%llu target=%llu "
                 "anim=%u castTime=%u effect=%s\n",
                 spellID, static_cast<unsigned long long>(casterGuid),
                 static_cast<unsigned long long>(targetGuid),
                 definition->precastAnimation, castTimeMs,
                 definition->precastModel.c_str());
}

void CWorld::LaunchSpell(std::uint64_t casterGuid,
                         std::uint64_t targetGuid,
                         unsigned spellID) {
    const SpellVisualDefinition* definition = FindSpellVisual(spellID);
    if (!definition) return;
    SetUnitTransientSequence(casterGuid, definition->castAnimation, 750U);

    NTempest::C3Vector source = UnitWorldPosition(casterGuid);
    NTempest::C3Vector target = UnitWorldPosition(targetGuid);
    source.z += 1.2F;
    target.z += 1.0F;
    SpawnStationarySpellEffect(definition->castModel, casterGuid,
                               source, 650U);
    if (!definition->missileModel.empty() && targetGuid != 0U)
        SpawnProjectileSpellEffect(*definition, source, target);
    else
        SpawnStationarySpellEffect(definition->impactModel, targetGuid,
                                   target, 1100U);

    std::fprintf(stderr,
                 "WORLD_SPELL_LAUNCH spell=%u caster=%llu target=%llu "
                 "anim=%u missile=%s impact=%s speed=%.3f\n",
                 spellID, static_cast<unsigned long long>(casterGuid),
                 static_cast<unsigned long long>(targetGuid),
                 definition->castAnimation,
                 definition->missileModel.c_str(),
                 definition->impactModel.c_str(), definition->missileSpeed);
}

void CWorld::PlayMeleeAttack(std::uint64_t attackerGuid,
                             std::uint64_t victimGuid) {
    constexpr unsigned kAttackUnarmed = 16U;
    SetUnitTransientSequence(attackerGuid, kAttackUnarmed, 800U);
    std::fprintf(stderr, "WORLD_MELEE attacker=%llu victim=%llu\n",
                 static_cast<unsigned long long>(attackerGuid),
                 static_cast<unsigned long long>(victimGuid));
}

void CWorld::NotifyPlayerDeath(std::uint64_t killerGuid) {
    s_world.playerKillerGuid = killerGuid;
    if (s_world.playerLifeStateKnown && s_world.playerHealth == 0U)
        BeginPlayerDeath(true);
    std::fprintf(stderr, "WORLD_DEATH_NOTIFY killer=%llu\n",
                 static_cast<unsigned long long>(killerGuid));
}

bool CWorld::IsPlayerDead() {
    return s_world.playerDead;
}

void CWorld::Jump(unsigned timestamp) {
    if (!s_world.loaded || ClntObjMgrGetActivePlayer() == 0U ||
        s_world.playerJumping || s_world.playerDead) {
        return;
    }

    NTempest::C3Vector position = ClntObjMgrGetActivePosition();
    const float ground = CalcAltitude(position.x, position.y, 1.0F);
    if (std::isfinite(ground)) position.z = ground;
    ClntObjMgrSetActivePosition(position);

    s_world.playerJumping = true;
    s_world.playerJumpVelocity = 7.9555473F;
    s_world.jumpStartTime = timestamp ? timestamp : OsGetAsyncTimeMs();
    s_world.landAnimationUntil = 0U;
    SendPlayerMovementUpdatePacket(0x00BBU, 0x00004000U); // MSG_MOVE_JUMP
}

void CWorld::UpdatePlayerMovement(float elapsedSec) {
    if (!s_world.loaded || ClntObjMgrGetActivePlayer() == 0U ||
        s_world.playerDead) return;

    NTempest::C3Vector position = ClntObjMgrGetActivePosition();
    const float ground = CalcAltitude(position.x, position.y, 1.0F);
    if (s_world.playerJumping) {
        constexpr float gravity = 19.291105F;
        const float elapsed = (std::clamp)(elapsedSec, 0.0F, 0.1F);
        position.z += s_world.playerJumpVelocity * elapsed
            - 0.5F * gravity * elapsed * elapsed;
        s_world.playerJumpVelocity -= gravity * elapsed;
        if (std::isfinite(ground) && position.z <= ground &&
            s_world.playerJumpVelocity <= 0.0F) {
            position.z = ground;
            s_world.playerJumping = false;
            s_world.playerJumpVelocity = 0.0F;
            s_world.landAnimationUntil = OsGetAsyncTimeMs() + 300U;
            ClntObjMgrSetActivePosition(position);
            SendPlayerMovementUpdatePacket(0x00E9U, 0U); // MSG_MOVE_HEARTBEAT
            return;
        }
    } else if (std::isfinite(ground)) {
        position.z = ground;
    }
    ClntObjMgrSetActivePosition(position);
}

bool InstantiateCreature(const CLNT_VISIBLE_OBJECT& object) {
    if ((object.objectType != 3U && object.objectType != 7U) ||
        object.guid == 0U) {
        return false;
    }

    // A corpse owned by the local player carries the same fully composited
    // character model and equipment textures as the living player.  Reusing
    // that model is the recovered Corpse_C path; treating it as an ordinary
    // CreatureDisplayInfo row loses the player skin/hair/equipment layers.
    if (object.objectType == 7U &&
        object.ownerGuid == ClntObjMgrGetActivePlayer() &&
        s_world.playerCharacterReady &&
        s_world.playerCharacter.m_characterModelOwner) {
        std::shared_ptr<CModel> model = ModelDuplicate(
            *s_world.playerCharacter.m_characterModelOwner);
        if (!model || !model->data || !ModelIsLoaded(*model))
            return false;
        WorldCreature corpse;
        corpse.guid = object.guid;
        corpse.displayId = object.displayId;
        corpse.position = object.position;
        corpse.facing = object.facing;
        corpse.scale = object.scale > 0.0F ? object.scale : 1.0F;
        corpse.corpse = true;
        corpse.dead = true;
        corpse.sequence = kAnimDeath;
        corpse.model = std::move(model);
        ModelForceSequenceTime(
            *corpse.model,
            kAnimDeath,
            ModelGetSequenceDuration(*corpse.model, kAnimDeath));
        ModelSetLightSelectCallback(*corpse.model, &CWorld::SelectLight, nullptr);
        s_world.creatures.emplace(object.guid, std::move(corpse));
        std::fprintf(stderr,
                     "WORLD_PLAYER_CORPSE_MODEL guid=%llu owner=%llu display=%u\n",
                     static_cast<unsigned long long>(object.guid),
                     static_cast<unsigned long long>(object.ownerGuid),
                     object.displayId);
        return true;
    }

    if (object.displayId == 0U || !LoadCreatureDatabase())
        return false;

    const auto displayIt = s_creatureDatabase.displays.find(object.displayId);
    if (displayIt == s_creatureDatabase.displays.end()) {
        static unsigned missingDisplayReports = 0U;
        if (missingDisplayReports++ < 16U)
            std::fprintf(stderr,
                         "WORLD_CREATURE_DISPLAY_MISSING guid=%llu entry=%u display=%u\n",
                         static_cast<unsigned long long>(object.guid),
                         object.entryId, object.displayId);
        return false;
    }
    const CreatureDisplayRecord& display = displayIt->second;
    const auto modelIt = s_creatureDatabase.models.find(display.modelId);
    if (modelIt == s_creatureDatabase.models.end() ||
        modelIt->second.modelName.empty()) {
        static unsigned missingModelReports = 0U;
        if (missingModelReports++ < 16U)
            std::fprintf(stderr,
                         "WORLD_CREATURE_MODEL_RECORD_MISSING guid=%llu entry=%u "
                         "display=%u model=%u\n",
                         static_cast<unsigned long long>(object.guid),
                         object.entryId, object.displayId, display.modelId);
        return false;
    }
    const CreatureModelRecord& modelData = modelIt->second;

    std::shared_ptr<CModel> source;
    if (const auto found = s_world.creatureTemplates.find(modelData.modelName);
        found != s_world.creatureTemplates.end()) {
        source = found->second;
    } else {
        CModelCreate createData{};
        createData.flags = 0x00100800U;
        createData.sequenceNames = g_animationNames;
        createData.numSequences =
            static_cast<std::uint32_t>(g_animationNameCount);
        source = ModelCreate(modelData.modelName.c_str(), &createData);
        if (source && source->data && ModelIsLoaded(*source))
            s_world.creatureTemplates.emplace(modelData.modelName, source);
    }

    std::shared_ptr<CModel> model = source ? ModelDuplicate(*source) : nullptr;
    if (!model || !model->data || !ModelIsLoaded(*model)) {
        static unsigned loadFailureReports = 0U;
        if (loadFailureReports++ < 32U)
            std::fprintf(stderr,
                         "WORLD_CREATURE_MODEL_LOAD_FAILED guid=%llu entry=%u "
                         "display=%u model=%u file=%s\n",
                         static_cast<unsigned long long>(object.guid),
                         object.entryId, object.displayId, display.modelId,
                         modelData.modelName.c_str());
        return false;
    }

    WorldCreature creature;
    creature.guid = object.guid;
    creature.entryId = object.entryId;
    creature.displayId = object.displayId;
    creature.position = object.position;
    creature.facing = object.facing;
    creature.scale = object.scale > 0.0F ? object.scale : 1.0F;
    creature.corpse = object.objectType == 7U;
    creature.model = std::move(model);
    if (creature.corpse) {
        ModelForceSequenceTime(
            *creature.model,
            kAnimDeath,
            ModelGetSequenceDuration(*creature.model, kAnimDeath));
    } else {
        ModelSetSequence(*creature.model, kAnimStand);
    }
    creature.sequence = creature.corpse ? kAnimDeath : kAnimStand;
    creature.dead = creature.corpse;
    ModelSetLightSelectCallback(*creature.model, &CWorld::SelectLight, nullptr);
    if (display.alpha < 255U) {
        ModelSetVertexAlpha(*creature.model,
                            static_cast<float>(display.alpha) / 255.0F);
        ModelEnableFullAlpha(*creature.model, true);
    }

    unsigned textureReplacements = 0U;
    if ((modelData.flags & 4U) == 0U) {
        for (unsigned index = 0; index < display.textureVariations.size(); ++index) {
            const std::string filename = CreatureTextureFile(
                modelData.modelName, display.textureVariations[index]);
            if (filename.empty()) continue;
            HTEXTURE__* texture = TextureCreate(filename.c_str(), nullptr, 0U, 0);
            if (texture && ModelReplaceTexture(*creature.model, 11U + index,
                                               texture, false)) {
                creature.textures[index] = texture;
                ++textureReplacements;
            } else if (texture) {
                TextureRelease(texture);
            }
        }
    } else if (const auto extra = s_creatureDatabase.extras.find(
                   display.extendedDisplayInfoId);
               extra != s_creatureDatabase.extras.end()) {
        const std::string filename =
            BakedCreatureTextureFile(extra->second.bakedTexture);
        if (!filename.empty()) {
            HTEXTURE__* texture = TextureCreate(filename.c_str(), nullptr, 0U, 0);
            if (texture && ModelReplaceTexture(*creature.model, 1U, texture,
                                               false)) {
                creature.textures[0] = texture;
                ++textureReplacements;
            } else if (texture) {
                TextureRelease(texture);
            }
        }
    }

    std::fprintf(stderr,
                 "WORLD_CREATURE_MODEL guid=%llu entry=%u display=%u model=%u "
                 "flags=%08X file=%s scale=%.3f textures=%u\n",
                 static_cast<unsigned long long>(object.guid), object.entryId,
                 object.displayId, display.modelId, modelData.flags,
                 modelData.modelName.c_str(), creature.scale,
                 textureReplacements);
    s_world.creatures.emplace(object.guid, std::move(creature));
    return true;
}

void SyncCreatures() {
    std::unordered_set<std::uint64_t> visible;
    for (const CLNT_VISIBLE_OBJECT& object : ClntObjMgrGetVisibleObjects()) {
        if ((object.objectType != 3U && object.objectType != 7U) ||
            object.guid == 0U) continue;
        visible.insert(object.guid);
        auto found = s_world.creatures.find(object.guid);
        if (found != s_world.creatures.end() &&
            found->second.displayId != object.displayId) {
            ReleaseCreature(found->second);
            s_world.creatures.erase(found);
            found = s_world.creatures.end();
        }
        if (found == s_world.creatures.end()) {
            InstantiateCreature(object);
            found = s_world.creatures.find(object.guid);
        }
        if (found == s_world.creatures.end()) continue;
        WorldCreature& creature = found->second;
        creature.entryId = object.entryId;
        creature.position = object.position;
        creature.facing = object.facing;
        creature.scale = object.scale > 0.0F ? object.scale : 1.0F;
        creature.moving = !creature.corpse && object.moving;
        creature.movementSpeed = object.movementSpeed;
        const std::uint32_t oldHealth = creature.health;
        const bool wasLifeStateKnown = creature.lifeStateKnown;
        creature.health = object.health;
        creature.maxHealth = object.maxHealth;
        creature.lifeStateKnown = !creature.corpse && object.maxHealth != 0U;
        if (creature.lifeStateKnown) {
            if (!wasLifeStateKnown && creature.health == 0U)
                BeginCreatureDeath(creature, false);
            else if (wasLifeStateKnown && oldHealth > 0U &&
                     creature.health == 0U)
                BeginCreatureDeath(creature, true);
            else if (wasLifeStateKnown && oldHealth == 0U &&
                     creature.health > 0U)
                RestoreCreature(creature);
        }

        const unsigned now = OsGetAsyncTimeMs();
        const unsigned desiredSequence = creature.corpse
            ? kAnimDeath
            : creature.dead
            ? (now < creature.deathSequenceUntil ? kAnimDeath : kAnimDead)
            : now < creature.spellSequenceUntil
                ? creature.spellSequence
                : (creature.moving
                    ? (creature.movementSpeed > 3.5F ? 5U : 4U)
                    : kAnimStand);
        if (creature.sequence != desiredSequence && creature.model &&
            ModelSetSequence(*creature.model, desiredSequence)) {
            creature.sequence = desiredSequence;
        }
    }

    for (auto iterator = s_world.creatures.begin();
         iterator != s_world.creatures.end();) {
        if (visible.contains(iterator->first)) {
            ++iterator;
            continue;
        }
        ReleaseCreature(iterator->second);
        iterator = s_world.creatures.erase(iterator);
    }
}

void CWorld::Update(float elapsedSec) {
    ClntObjMgrUpdateVisibleObjects(elapsedSec);
    if (ClntObjMgrGetActivePlayer() != 0U)
        s_world.playerPosition = ClntObjMgrGetActivePosition();

    SyncPlayerLifeState();
    DayNightUpdateLighting();
    SyncCreatures();

    const auto elapsedMs = static_cast<std::uint32_t>(std::lround(
        (std::max)(0.0F, elapsedSec) * 1000.0F));
    for (WorldDoodad& doodad : s_world.doodads) {
        if (!doodad.model || !doodad.model->data) continue;
        NTempest::C34Matrix transform;
        transform.Translate(doodad.position - s_world.cameraPosition);
        transform *= NTempest::C34Matrix::Rotation(
            {0.0F, 0.0F, 1.0F}, doodad.rotation.z);
        transform *= NTempest::C34Matrix::Rotation(
            {0.0F, 1.0F, 0.0F}, doodad.rotation.y);
        transform *= NTempest::C34Matrix::Rotation(
            {1.0F, 0.0F, 0.0F}, doodad.rotation.x);
        transform.Scale(doodad.scale);
        doodad.model->data->m_modelToWorld = transform;
        ModelAnimateAttached(*doodad.model, elapsedMs);
    }
    for (auto& [guid, creature] : s_world.creatures) {
        (void)guid;
        if (!creature.model || !creature.model->data) continue;
        NTempest::C3Vector visualPosition = creature.position;
        const float terrainZ = CalcAltitude(visualPosition.x,
                                            visualPosition.y, 1.0F);
        if (std::isfinite(terrainZ)) visualPosition.z = terrainZ;
        NTempest::C34Matrix transform = NTempest::C34Matrix::Rotation(
            {0.0F, 0.0F, 1.0F}, creature.facing);
        const NTempest::C3Vector relative =
            visualPosition - s_world.cameraPosition;
        transform.d0 = relative.x;
        transform.d1 = relative.y;
        transform.d2 = relative.z;
        transform.Scale(creature.scale);
        creature.model->data->m_modelToWorld = transform;
        ModelAnimateAttached(*creature.model, elapsedMs);
    }

    UpdateSpellEffects(elapsedMs);

    CModel* player = s_world.playerCharacter.m_characterModelOwner.get();
    if (!s_world.playerCharacterReady || !player || !player->data)
        return;

    const float terrainZ = CalcAltitude(s_world.playerPosition.x,
                                        s_world.playerPosition.y, 1.0F);
    NTempest::C3Vector visualPosition = s_world.playerPosition;
    if (!s_world.playerJumping && std::isfinite(terrainZ)) {
        // The mover normally resolves the authoritative position against
        // CMap::PointIntersect before Unit_C builds its render transform.
        // Keep the network position intact and resolve only the visual pose.
        visualPosition.z = terrainZ;
    }

    unsigned desiredSequence = kAnimStand;
    const unsigned now = OsGetAsyncTimeMs();
    if (s_world.playerDead) {
        desiredSequence = now < s_world.playerDeathSequenceUntil
            ? kAnimDeath : kAnimDead;
    } else if (now < s_world.playerSpellSequenceUntil) {
        desiredSequence = s_world.playerSpellSequence;
    } else if (s_world.playerJumping) {
        if (now - s_world.jumpStartTime < 150U)
            desiredSequence = 37U; // ANIM_JUMP_START
        else if (s_world.playerJumpVelocity > 0.0F)
            desiredSequence = 38U; // ANIM_JUMP
        else
            desiredSequence = 40U; // ANIM_FALL
    } else if (now < s_world.landAnimationUntil) {
        desiredSequence = 39U; // ANIM_JUMP_END
    } else if (const CGInputControl* input = CGInputControl::GetActive()) {
        const unsigned flags = input->m_controlFlags;
        if ((flags & INPUT_MOVE_PLAYER_BACKWARD_KEY) != 0U) {
            desiredSequence = 13U; // ANIM_WALK_BACKWARDS
        } else if ((flags & INPUT_STRAFE_PLAYER_LEFT_KEY) != 0U) {
            desiredSequence = 11U; // ANIM_SHUFFLE_LEFT
        } else if ((flags & INPUT_STRAFE_PLAYER_RIGHT_KEY) != 0U) {
            desiredSequence = 12U; // ANIM_SHUFFLE_RIGHT
        } else if ((flags & (INPUT_MOVE_PLAYER_FORWARD_KEY |
                             INPUT_MOVE_PLAYER_AUTORUN)) != 0U ||
                   (flags & INPUT_MOVE_AND_TURN_PLAYER) ==
                       INPUT_MOVE_AND_TURN_PLAYER) {
            desiredSequence = 5U; // ANIM_RUN
        }
    }
    if (ModelGetPrimarySequence(*player) != desiredSequence)
        ModelSetSequence(*player, desiredSequence);

    NTempest::C34Matrix standing = NTempest::C34Matrix::Rotation(
        {0.0F, 0.0F, 1.0F}, ClntObjMgrGetActiveFacing());
    const NTempest::C3Vector cameraRelative =
        visualPosition - s_world.cameraPosition;
    standing.d0 = cameraRelative.x;
    standing.d1 = cameraRelative.y;
    standing.d2 = cameraRelative.z;
    player->data->m_modelToWorld = standing;
    ModelAnimateAttached(*player, elapsedMs);

    static unsigned worldAnimationReports = 0U;
    if (worldAnimationReports < 8U) {
        std::fprintf(stderr,
                     "WORLD_PLAYER_UPDATE dt=%g ms=%u seq=%u time=%u "
                     "pos=(%.3f,%.3f,%.3f) terrainZ=%.3f deltaZ=%.3f "
                     "facing=%.3f relative=(%.3f,%.3f,%.3f)\n",
                     elapsedSec, elapsedMs, ModelGetPrimarySequence(*player),
                     ModelGetSequenceTime(*player),
                     s_world.playerPosition.x, s_world.playerPosition.y,
                     s_world.playerPosition.z, terrainZ,
                     std::isfinite(terrainZ)
                         ? s_world.playerPosition.z - terrainZ
                         : std::numeric_limits<float>::quiet_NaN(),
                     ClntObjMgrGetActiveFacing(), standing.d0, standing.d1,
                     standing.d2);
        ++worldAnimationReports;
    }
}

void CWorld::Render() {
    if (!s_world.loaded) return;
    static bool loggedFirstRender = false;
    unsigned submittedLayers = 0;
    unsigned missingLayers = 0;
    GxRsPush();
    GxXformPush(GxXform_World);
    GxVertexShaderSelect(GxVS_PassThru);
    GxRsSet(GxRs_MatDiffuse, NTempest::CImVector(0xffffffffU));
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 1);
    GxRsSet(GxRs_DepthTest, 1);
    GxRsSet(GxRs_Culling, 0);
    GxRsSet(GxRs_TexBlend0, static_cast<int>(GxTexBlend_Mod));
    for (TerrainChunk& chunk : s_world.chunks) {
        NTempest::C44Matrix chunkMatrix;
        chunkMatrix.Translate(chunk.corner - s_world.cameraPosition);
        GxXformSet(GxXform_World, chunkMatrix);
        for (unsigned layer = 0; layer < chunk.layerCount; ++layer) {
            if (RenderChunkLayer(chunk, layer)) ++submittedLayers;
            else ++missingLayers;
        }
    }
    GxRsSet(GxRs_PolygonOffset, 0.0F);
    GxRsSet(GxRs_DepthWrite, 1);
    GxXformPop(GxXform_World);
    GxRsPop();

    for (const WorldMapObj& instance : s_world.mapObjects) {
        if (!instance.mapObj || !instance.mapObj->IsLoaded()) continue;
        const NTempest::C44Matrix transform =
            BuildCameraRelativeMapObjPlacement(
                instance.position, instance.rotation,
                s_world.cameraPosition);
        instance.mapObj->Render(transform);
    }

    NTempest::C3Vector cameraDirection =
        s_world.playerPosition - s_world.cameraPosition;
    if (cameraDirection.SquaredMag() > 0.0F)
        cameraDirection.Normalize();
    // World objects are maintained camera-relative in the recovered client,
    // so the model scene camera itself remains at the origin.
    ModelScenePlaceCamera({}, cameraDirection);
    CWorld::SelectLight(nullptr, s_world.playerPosition,
                        &s_world.cameraPosition, 8U);
    unsigned submittedModels = 0U;
    for (WorldDoodad& doodad : s_world.doodads) {
        if (!doodad.model) continue;
        // MDX doodads use the same front-face convention as character models.
        // Two-sided material layers still disable culling in ModelRenderPass.
        ModelAddToScene(*doodad.model, MODEL_RENDER_REVERSE_CULL);
        ++submittedModels;
    }
    for (auto& [guid, creature] : s_world.creatures) {
        (void)guid;
        if (!creature.model) continue;
        ModelAddToScene(*creature.model, MODEL_RENDER_REVERSE_CULL);
        ++submittedModels;
    }
    for (WorldSpellEffect& effect : s_world.spellEffects) {
        if (!effect.model) continue;
        ModelAddToScene(*effect.model);
        ++submittedModels;
    }
    if (s_world.playerCharacterReady) {
        if (CModel* player = s_world.playerCharacter.m_characterModelOwner.get()) {
            static bool loggedPlayerSubmit = false;
            if (!loggedPlayerSubmit) {
                std::fprintf(stderr,
                             "WORLD_PLAYER_SUBMIT model=%s relative=(%.3f,%.3f,%.3f) "
                             "direction=(%.3f,%.3f,%.3f)\n",
                             player->shared ? player->shared->name.data() : "<unnamed>",
                             player->data ? player->data->m_modelToWorld.d0 : 0.0F,
                             player->data ? player->data->m_modelToWorld.d1 : 0.0F,
                             player->data ? player->data->m_modelToWorld.d2 : 0.0F,
                             cameraDirection.x, cameraDirection.y, cameraDirection.z);
                loggedPlayerSubmit = true;
            }
            ModelAddToScene(*player, MODEL_RENDER_REVERSE_CULL);
            ++submittedModels;
        }
    }
    if (submittedModels != 0U) ModelRenderScene();
    RenderUnitNames();
    if (!loggedFirstRender) {
        NTempest::C44Matrix view;
        NTempest::C44Matrix projection;
        GxXformView(&view);
        GxXformProjection(&projection);

        projection.c0 = -projection.c0;
        projection.c1 = -projection.c1;
        projection.c2 = -projection.c2;
        projection.c3 = -projection.c3;
        if (!GxMasterEnable(GxMasterEnable_NormalProjection)
            && projection.d3 != 1.0F) {
            projection.a0 *= 0.2F; projection.a1 *= 0.2F; projection.a2 *= 0.2F;
            projection.b0 *= 0.2F; projection.b1 *= 0.2F; projection.b2 *= 0.2F;
            projection.c0 *= 0.2F; projection.c1 *= 0.2F; projection.c2 *= 0.2F;
            projection.d0 *= 0.2F; projection.d1 *= 0.2F; projection.d2 *= 0.2F;
        }

        std::size_t testedVertices = 0;
        std::size_t frontVertices = 0;
        std::size_t visibleVertices = 0;
        NTempest::C3Vector traceCorner{};
        NTempest::C3Vector traceVertex{};
        NTempest::C4Vector eye{};
        NTempest::C4Vector clip{};
        bool foundVisibleVertex = false;
        for (const TerrainChunk& chunk : s_world.chunks) {
            NTempest::C44Matrix world;
            world.Translate(chunk.corner - s_world.cameraPosition);
            NTempest::C44Matrix modelView = world * view;
            modelView.a2 = -modelView.a2;
            modelView.b2 = -modelView.b2;
            modelView.c2 = -modelView.c2;
            modelView.d2 = -modelView.d2;

            for (const NTempest::C3Vector& vertex : chunk.vertices) {
                ++testedVertices;
                const NTempest::C4Vector candidateEye = modelView * NTempest::C4Vector(
                    vertex.x, vertex.y, vertex.z, 1.0F);
                const NTempest::C4Vector candidateClip = projection * candidateEye;
                if (candidateClip.w <= 0.0F)
                    continue;
                ++frontVertices;
                const bool inside = candidateClip.x >= -candidateClip.w
                    && candidateClip.x <= candidateClip.w
                    && candidateClip.y >= -candidateClip.w
                    && candidateClip.y <= candidateClip.w
                    && candidateClip.z >= -candidateClip.w
                    && candidateClip.z <= candidateClip.w;
                if (!inside)
                    continue;
                ++visibleVertices;
                if (!foundVisibleVertex) {
                    foundVisibleVertex = true;
                    traceCorner = chunk.corner;
                    traceVertex = vertex;
                    eye = candidateEye;
                    clip = candidateClip;
                }
            }
        }
        std::fprintf(stderr,
                     "WORLD_RENDER chunks=%zu layers=%u missing=%u camera=(%.3f,%.3f,%.3f) firstCorner=(%.3f,%.3f,%.3f) firstVerts=(%.3f,%.3f,%.3f) clipVertices=%zu front=%zu visible=%zu traceCorner=(%.3f,%.3f,%.3f) traceVertex=(%.3f,%.3f,%.3f) eye=(%.3f,%.3f,%.3f,%.3f) clip=(%.3f,%.3f,%.3f,%.3f) ndc=(%.3f,%.3f,%.3f)\n",
                     s_world.chunks.size(), submittedLayers, missingLayers,
                     s_world.cameraPosition.x,
                     s_world.cameraPosition.y,
                     s_world.cameraPosition.z,
                     s_world.chunks.empty() ? 0.0F : s_world.chunks.front().corner.x,
                     s_world.chunks.empty() ? 0.0F : s_world.chunks.front().corner.y,
                     s_world.chunks.empty() ? 0.0F : s_world.chunks.front().corner.z,
                     s_world.chunks.empty() ? 0.0F : s_world.chunks.front().vertices[0].x,
                     s_world.chunks.empty() ? 0.0F : s_world.chunks.front().vertices[0].y,
                     s_world.chunks.empty() ? 0.0F : s_world.chunks.front().vertices[0].z,
                     testedVertices, frontVertices, visibleVertices,
                     traceCorner.x, traceCorner.y, traceCorner.z,
                     traceVertex.x, traceVertex.y, traceVertex.z,
                     eye.x, eye.y, eye.z, eye.w,
                     clip.x, clip.y, clip.z, clip.w,
                     clip.w != 0.0F ? clip.x / clip.w : 0.0F,
                     clip.w != 0.0F ? clip.y / clip.w : 0.0F,
                     clip.w != 0.0F ? clip.z / clip.w : 0.0F);
        std::fflush(stderr);
        loggedFirstRender = true;
    }
}

void CWorld::RenderAlpha() {
}

bool CWorld::IsLoaded() {
    return s_world.loaded;
}

const char* CWorld::GetMapName() {
    return s_world.mapName.c_str();
}

const NTempest::C3Vector& CWorld::PlayerPosition() {
    return s_world.playerPosition;
}

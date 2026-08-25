#include "WoW/Source/Magic/spellvisuals.h"

#include <algorithm>
#include <array>
#include <limits>
#include <string>

float INFINITY_374 = 0.0F;
static std::vector<BlizzardObject*> s_blizzards;
static std::vector<LightningObject*> s_lightning;
static float s_elapsed = 0.0F;
unsigned s_spellVisualCameraShake = 0;

int _E2_374(){return _E1_374();} int _E1_374(){INFINITY_374=std::numeric_limits<float>::infinity();return 2139095040;}
int _E5_344(){_E4_344();return 0;} void _E4_344(){}
int _E8_334(){_E7_344();return 0;} void _E7_344(){}
int _E177_5(){return 0;} unsigned _E173_2(){return 0;} int _E176_3(){return 0;} void _E174_5(){}
int _E182_9(){return 0;} unsigned _E179_3(){return 0;} int _E181_1(){return 0;} void _E180_4(){}
int _E187_12(){return 0;} unsigned _E184_12(){return 0;} int _E186_10(){return 0;} void _E185_17(){}
int _E190_11(){return 0;} void _E189_9(){} int _E193_11(){return 0;} void _E192_12(){}
int _E198_10(){return 0;} int _E195_9(){return 0;} int _E197_9(){return 0;} void _E196_8(){}
int _E203_7(){return 0;} unsigned _E200_9(){return 0;} int _E202_8(){return 0;} void _E201_7(){}
int _E208_3(){return 0;} unsigned _E205_7(){return 0;} int _E207_5(){return 0;} void _E206_4(){}
int _E213_4(){return 0;} unsigned _E210_5(){return 0;} int _E212_3(){return 0;} void _E211_3(){}
int _E218_3(){return 0;} int _E215_3(){return 0;} int _E217_3(){return 0;} void _E216_2(){}

BlizzardObject::Shard* BlizzardObject::AllocShard() { return new Shard; }
void BlizzardObject::FreeShard(BlizzardObject* object) { if (object) object->m_shards.clear(); }
void BlizzardObject::WorldObjectRender(BlizzardObject* object, const NTempest::C44Matrix*) { if (object) object->Render(nullptr); }
void BlizzardObject::Destroy() { m_shards.clear(); m_visible = false; m_renderCount = 0; }
void BlizzardObject::Update() {
    for (auto it = m_shards.begin(); it != m_shards.end();) {
        it->lifetime -= 0.016F;
        if (it->lifetime <= 0.0F) it = m_shards.erase(it); else ++it;
    }
}
int BlizzardObject::ShardSeqFinished() const { return m_shards.empty() ? 1 : 0; }
void BlizzardObject::Render(const NTempest::C44Matrix*) { if (m_visible) ++m_renderCount; }
void FishingLineObject::Render() { if (visible) ++renderCount; }
bool LightningObject::Tick(unsigned currentTime) {
    const bool alive = m_expireTime == 0 || currentTime < m_expireTime;
    if (!alive) m_bolts.clear();
    return alive;
}
void LightningObject::AddRef() { ++m_refCount; }
void LightningObject::DelRef() { if (m_refCount) --m_refCount; }
void EclipseObject::Update(unsigned currentTime) { m_lastUpdate = currentTime; ++m_updateCount; }
bool GetFishingLineStartPos(void* model, NTempest::C3Vector* position) {
    if (model == nullptr || position == nullptr) return false;
    *position = static_cast<FishingLineObject*>(model)->start;
    return true;
}

void SpellVisualsInitialize() { s_elapsed = 0.0F; s_spellVisualCameraShake = 0; s_blizzards.clear(); s_lightning.clear(); }
void SpellVisualsShutdown() { for (auto* object : s_blizzards) delete object; s_blizzards.clear(); for (auto* object : s_lightning) delete object; s_lightning.clear(); }
unsigned InitializeAuraNames() { return 1; }
void InitializeFishingLineIntervals() {}
unsigned InitializeFishingLineIndices() { return 1; }
void SpellVisualsTick(float elapsed) { s_elapsed += elapsed; for (auto* object : s_blizzards) if (object) object->Update(); for (auto* object : s_lightning) if (object) object->Tick(static_cast<unsigned>(s_elapsed * 1000.0F)); }
void SpellVisualsRender() { for (auto* object : s_blizzards) if (object) object->Render(nullptr); }
void RenderFishingLines() {}
void SpellVisualsPlayCameraShakeID(int position, const NTempest::C3Vector*) { s_spellVisualCameraShake = static_cast<unsigned>(position); }
void SpellVisualClearLightning(LightningObject* object) { if (object) object->m_bolts.clear(); }
BlizzardObject* AllocBlizzard() { auto* object = new BlizzardObject; s_blizzards.push_back(object); return object; }
void SpellVisualsBlizzardDestroy(BlizzardObject* object) { if (!object) return; object->Destroy(); s_blizzards.erase(std::remove(s_blizzards.begin(), s_blizzards.end(), object), s_blizzards.end()); delete object; }
void SpellVisualsFishingLineDestroy(void* object) { delete static_cast<FishingLineObject*>(object); }
void SpellVisualFishingLineSetVisible(FishingLineObject* object) { if (object) object->visible = true; }
bool IsShapeshiftSpell(int spellID) { return spellID > 0 && spellID < 1000; }
void SpellVisualsPlayKit(CGUnit_C*, int kitID) { s_spellVisualCameraShake = static_cast<unsigned>(kitID); }
void PlayImpactKit(CGUnit_C*, int kitID) { s_spellVisualCameraShake = static_cast<unsigned>(kitID); }
ANIMENUMERATION SpellGetRangedPrecastHoldAnim(unsigned value) { return static_cast<ANIMENUMERATION>(value); }
NTempest::C3Vector* GetSpellChainEffectSource(float* values, int index) {
    if (values == nullptr || index < 0) return nullptr;
    return reinterpret_cast<NTempest::C3Vector*>(values) + index;
}
void SpellVisualsProc_Eclipse(const SpellVisualKitRec*, int flags, unsigned spellID) {
    s_spellVisualCameraShake = flags != 0 ? static_cast<unsigned>(flags) : spellID;
}
char* GetSpellAuraEffectName(int effectID) {
    static std::array<std::string, 32> names;
    const unsigned index = effectID < 0 ? 0U : static_cast<unsigned>(effectID) % names.size();
    if (names[index].empty()) names[index] = "SPELL_AURA_" + std::to_string(effectID);
    return names[index].data();
}
const char* GetSpellAuraEffectToken(int effectID) {
    static std::array<std::string, 32> tokens;
    const unsigned index = effectID < 0 ? 0U : static_cast<unsigned>(effectID) % tokens.size();
    if (tokens[index].empty()) tokens[index] = "SPELL_AURA_" + std::to_string(effectID);
    return tokens[index].c_str();
}

int _E20_76(){return 0;} void _E19_58(){} int _E23_78(){return 0;} void _E22_66(){} int _E26_75(){return 0;} void _E25_66(){}
int _E29_74(){return 0;} void _E28_102(){} int _E32_66(){return 0;} void _E31_100(){} int _E98_4(){return 0;} void _E97_5(){}
int _E101_25(){return 0;} void _E100_29(){} int _E104_29(){return 0;} void _E103_8(){} int _E107_26(){return 0;} void _E106_6(){}
int _E110_27(){return 0;} void _E109_4(){} int _E113_27(){return 0;} void _E112_3(){} int _E116_26(){return 0;} void _E115_4(){}
int _E119_27(){return 0;} void _E118_3(){} int _E122_26(){return 0;} void _E121_2(){} int _E125_25(){return 0;} void _E124_3(){}
int _E128_25(){return 0;} void _E127_2(){} int _E131_24(){return 0;} void _E130_2(){} int _E150_7(){return 0;} void _E147_2(){}
int _E149_6(){return 0;} void _E148_6(){}

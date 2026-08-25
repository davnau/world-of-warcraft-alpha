#pragma once

#include "ENGINE/Source/Tempest/c3vector.h"
#include "ENGINE/Source/Tempest/c44matrix.h"

#include <cstdint>
#include <vector>

struct SpellVisualKitRec;
struct CGUnit_C;
using ANIMENUMERATION = int;

class BlizzardObject {
public:
    struct Shard { NTempest::C3Vector position{}; float lifetime = 0.0F; };
    static Shard* AllocShard();
    static void FreeShard(BlizzardObject*);
    static void WorldObjectRender(BlizzardObject*, const NTempest::C44Matrix*);
    void Destroy();
    void Update();
    int ShardSeqFinished() const;
    void Render(const NTempest::C44Matrix*);
    std::vector<Shard> m_shards;
    bool m_visible = true;
    unsigned m_renderCount = 0;
};

class FishingLineObject {
public:
    void Render();
    bool visible = true;
    NTempest::C3Vector start{};
    NTempest::C3Vector end{};
    unsigned renderCount = 0;
};
class LightningObject {
public:
    struct Bolt { NTempest::C3Vector start{}, end{}; };
    LightningObject() = default;
    ~LightningObject() = default;
    bool Tick(unsigned currentTime);
    void AddRef(); void DelRef();
    std::vector<Bolt> m_bolts;
    unsigned m_refCount = 1;
    unsigned m_expireTime = 0;
    unsigned m_renderCount = 0;
};
class EclipseObject { public: void Update(unsigned currentTime); unsigned m_lastUpdate = 0; unsigned m_updateCount = 0; };

bool GetFishingLineStartPos(void* model, NTempest::C3Vector* position);
void SpellVisualsInitialize();
void SpellVisualsShutdown();
unsigned InitializeAuraNames();
void InitializeFishingLineIntervals();
unsigned InitializeFishingLineIndices();
void SpellVisualsTick(float elapsed);
void SpellVisualsRender();
void RenderFishingLines();
void SpellVisualsPlayCameraShakeID(int position, const NTempest::C3Vector* direction);
void SpellVisualClearLightning(LightningObject* object);
BlizzardObject* AllocBlizzard();
void SpellVisualsBlizzardDestroy(BlizzardObject* object);
void SpellVisualsFishingLineDestroy(void* object);
void SpellVisualFishingLineSetVisible(FishingLineObject* object);
bool IsShapeshiftSpell(int spellID);
void SpellVisualsPlayKit(CGUnit_C* unit, int kitID);
void PlayImpactKit(CGUnit_C* unit, int kitID);
ANIMENUMERATION SpellGetRangedPrecastHoldAnim(unsigned value);
NTempest::C3Vector* GetSpellChainEffectSource(float* values, int index);
void SpellVisualsProc_Eclipse(const SpellVisualKitRec* kit, int flags, unsigned spellID);
char* GetSpellAuraEffectName(int effectID);
const char* GetSpellAuraEffectToken(int effectID);

extern float INFINITY_374;
extern unsigned s_spellVisualCameraShake;
int _E2_374(); int _E1_374(); int _E5_344(); void _E4_344(); int _E8_334(); void _E7_344();
int _E177_5(); unsigned _E173_2(); int _E176_3(); void _E174_5(); int _E182_9(); unsigned _E179_3();
int _E181_1(); void _E180_4(); int _E187_12(); unsigned _E184_12(); int _E186_10(); void _E185_17();
int _E190_11(); void _E189_9(); int _E193_11(); void _E192_12(); int _E198_10(); int _E195_9();
int _E197_9(); void _E196_8(); int _E203_7(); unsigned _E200_9(); int _E202_8(); void _E201_7();
int _E208_3(); unsigned _E205_7(); int _E207_5(); void _E206_4(); int _E213_4(); unsigned _E210_5();
int _E212_3(); void _E211_3(); int _E218_3(); int _E215_3(); int _E217_3(); void _E216_2();
int _E20_76(); void _E19_58(); int _E23_78(); void _E22_66(); int _E26_75(); void _E25_66();
int _E29_74(); void _E28_102(); int _E32_66(); void _E31_100(); int _E98_4(); void _E97_5();
int _E101_25(); void _E100_29(); int _E104_29(); void _E103_8(); int _E107_26(); void _E106_6();
int _E110_27(); void _E109_4(); int _E113_27(); void _E112_3(); int _E116_26(); void _E115_4();
int _E119_27(); void _E118_3(); int _E122_26(); void _E121_2(); int _E125_25(); void _E124_3();
int _E128_25(); void _E127_2(); int _E131_24(); void _E130_2(); int _E150_7(); void _E147_2();
int _E149_6(); void _E148_6();

#include "WoW/Source/Object/objectclient/unitcombatlog_c.h"

unsigned g_unitCombatLogMessages = 0;
bool g_unitCombatLogEnabled = false;
namespace { bool s_fileLogging = false; }

COMBATLOGDESC::COMBATLOGDESC() = default;
void COMBATLOGDESC::Clear() { attackCount = 0; victimCount = 0; guidCount = 0; valid = false; }
void COMBATLOGDESC::LogAttack() { ++attackCount; valid = true; ++g_unitCombatLogMessages; }
void COMBATLOGDESC::LogUnitGUID() { ++guidCount; valid = true; }
void COMBATLOGDESC::LogVictim() { ++victimCount; valid = true; }
COMBATLOGDESC::~COMBATLOGDESC() { Clear(); }

#define COMBAT_LOG_EVENT(name) void name() { if (g_unitCombatLogEnabled) ++g_unitCombatLogMessages; }
COMBAT_LOG_EVENT(Capitalize)
COMBAT_LOG_EVENT(ClearUnitDataStructs)
COMBAT_LOG_EVENT(CloseDebugLogHandle)
COMBAT_LOG_EVENT(DebugCombatLogHandler)
COMBAT_LOG_EVENT(DetermineResultType)
COMBAT_LOG_EVENT(FormatSpellMissString)
COMBAT_LOG_EVENT(FormatSpellString)
COMBAT_LOG_EVENT(GeneralLogPrintf)
COMBAT_LOG_EVENT(GeneratePronouns)
COMBAT_LOG_EVENT(GetLogDistance)
COMBAT_LOG_EVENT(HandleGeneralCombatEvadeLogging)
COMBAT_LOG_EVENT(HandleGeneralCombatLogging)
COMBAT_LOG_EVENT(HandleGeneralCombatLoggingMissed)
COMBAT_LOG_EVENT(HandleGeneralCombatOrSpellHitLogging)
COMBAT_LOG_EVENT(HandleGeneralHealLogging)
COMBAT_LOG_EVENT(HandleSpellLogTerse)
COMBAT_LOG_EVENT(HandleTerseVictimLogging)
COMBAT_LOG_EVENT(IsSpellAbility)
COMBAT_LOG_EVENT(IsSpellHarmful)
COMBAT_LOG_EVENT(IsSpellOpenLock)
COMBAT_LOG_EVENT(IsSpellQuiet)
COMBAT_LOG_EVENT(IsSpellTeach)
COMBAT_LOG_EVENT(ItemEnchantmentCacheCallback)
COMBAT_LOG_EVENT(LogEnchantmentRequest)
COMBAT_LOG_EVENT(LogResults)
COMBAT_LOG_EVENT(NormalBlockHandler)
COMBAT_LOG_EVENT(NormalDodgeHandler)
COMBAT_LOG_EVENT(NormalEvadeHandler)
COMBAT_LOG_EVENT(NormalHitHandler)
COMBAT_LOG_EVENT(NormalImmuneHandler)
COMBAT_LOG_EVENT(NormalMissHandler)
COMBAT_LOG_EVENT(NormalParryHandler)
COMBAT_LOG_EVENT(OutputCombatMessage)
COMBAT_LOG_EVENT(ReportError)
COMBAT_LOG_EVENT(RoundTo)
COMBAT_LOG_EVENT(Script_ToggleCombatLogFileWrite)
COMBAT_LOG_EVENT(ShouldLog)
COMBAT_LOG_EVENT(ShouldLogAttacker)
COMBAT_LOG_EVENT(UnitCombatDebugLogEnable)
COMBAT_LOG_EVENT(UnitCombatLog)
COMBAT_LOG_EVENT(UnitCombatLogAuraAddedOrRemoved)
COMBAT_LOG_EVENT(UnitCombatLogCastGo)
COMBAT_LOG_EVENT(UnitCombatLogCastStart)
COMBAT_LOG_EVENT(UnitCombatLogEnchantment)
COMBAT_LOG_EVENT(UnitCombatLogEnchantmentAdded)
COMBAT_LOG_EVENT(UnitCombatLogEnchantmentRemoved)
COMBAT_LOG_EVENT(UnitCombatLogFactionChanged)
COMBAT_LOG_EVENT(UnitCombatLogHeartbeatResist)
COMBAT_LOG_EVENT(UnitCombatLogPartyKill)
COMBAT_LOG_EVENT(UnitCombatLogSetActivePlayer)
COMBAT_LOG_EVENT(UnitCombatLogShowXPGained)
COMBAT_LOG_EVENT(UnitCombatLogSpellFail)
COMBAT_LOG_EVENT(UnitCombatLogSpellMissed)
COMBAT_LOG_EVENT(UnitCombatLogSpellTeach)
COMBAT_LOG_EVENT(UnitCombatLogString)
COMBAT_LOG_EVENT(UnitCombatLogUnitDead)
COMBAT_LOG_EVENT(UnitCombatLogXPGain)
COMBAT_LOG_EVENT(UnitDebugCombatLogOnEnable)
COMBAT_LOG_EVENT(WriteAttemptsHitsMisses)
COMBAT_LOG_EVENT(WriteDamageTallies)
COMBAT_LOG_EVENT(WriteMessage)
COMBAT_LOG_EVENT(WriteSpellInfo)
COMBAT_LOG_EVENT(WriteString)
COMBAT_LOG_EVENT(WriteVictimStates)
#undef COMBAT_LOG_EVENT

void UnitCombatLogInitialize() { g_unitCombatLogMessages = 0; g_unitCombatLogEnabled = true; }
void UnitCombatLogShutdown() { g_unitCombatLogEnabled = false; s_fileLogging = false; }
void UnitCombatLogEnableFileLog() { s_fileLogging = true; }

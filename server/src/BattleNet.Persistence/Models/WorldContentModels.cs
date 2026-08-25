namespace BattleNet.Persistence.Models;

public sealed record WorldCreatureTemplate(
    uint Entry,
    uint DisplayId,
    string Name,
    string SubName,
    uint StaticFlags,
    uint LevelMin,
    uint LevelMax,
    uint Faction,
    uint Type,
    uint BeastFamily,
    uint NpcFlags,
    uint VendorId,
    uint LootId = 0,
    uint SpellListId = 0,
    uint MovementType = 0,
    float SpeedRun = 1.0f,
    float DetectionRange = 18.0f,
    float LeashRange = 80.0f,
    uint BaseAttackTime = 2000,
    uint GoldMin = 0,
    uint GoldMax = 0,
    uint Civilian = 0);

public sealed record WorldCreatureSpawn(
    uint SpawnId,
    uint Entry,
    int Map,
    float PositionX,
    float PositionY,
    float PositionZ,
    float Orientation,
    uint MovementType = 0,
    float WanderDistance = 0.0f);

public sealed record WorldGameObjectSpawn(
    uint SpawnId,
    uint Entry,
    int Map,
    float PositionX,
    float PositionY,
    float PositionZ,
    float Orientation,
    float Rotation0,
    float Rotation1,
    float Rotation2,
    float Rotation3,
    uint State);

public sealed record WorldGameObjectTemplate(
    uint Entry,
    uint Type,
    uint DisplayId,
    string Name,
    uint Faction,
    uint Flags,
    float Scale,
    IReadOnlyList<uint> Data);

public sealed record QuestRequirement(
    int CreatureOrGameObjectId,
    uint CreatureOrGameObjectCount,
    uint ItemId,
    uint ItemCount,
    string Name);

public sealed record WorldQuestTemplate(
    uint Entry,
    uint Method,
    uint QuestLevel,
    int ZoneOrSort,
    uint Type,
    uint NextQuestInChain,
    int RewardOrRequiredMoney,
    uint SourceItemId,
    IReadOnlyList<uint> RewardItemIds,
    IReadOnlyList<uint> RewardItemCounts,
    IReadOnlyList<uint> RewardChoiceItemIds,
    IReadOnlyList<uint> RewardChoiceItemCounts,
    string Title,
    string Details,
    string Objectives,
    string EndText,
    string RequestItemsText,
    string OfferRewardText,
    uint IncompleteEmote,
    uint CompleteEmote,
    uint PointMapId,
    float PointX,
    float PointY,
    uint PointOption,
    IReadOnlyList<QuestRequirement> Requirements);

public sealed record WorldQuestGiverQuest(
    uint Entry,
    uint QuestGiverState,
    uint QuestLevel,
    string Title);

public sealed record WorldVendorItem(
    uint ItemEntry,
    uint MaxCount,
    uint BuyPrice,
    uint MaxDurability,
    uint BuyCount,
    uint DisplayId);

public sealed record WorldSpellEffect(
    int EffectType,
    int DieSides,
    int BaseDice,
    int DicePerLevel,
    float RealPointsPerLevel,
    int BasePoints,
    int ImplicitTargetA,
    int ImplicitTargetB,
    int RadiusIndex,
    int AuraType,
    int AuraPeriod,
    float Amplitude,
    int ChainTargets,
    int ItemType,
    int MiscValue,
    uint TriggerSpell);

public sealed record WorldSpellDefinition(
    uint Id,
    uint School,
    uint Attributes,
    uint AttributesEx,
    uint Targets,
    uint CastingTimeIndex,
    int CastTimeMs,
    int RecoveryTime,
    int CategoryRecoveryTime,
    uint RangeIndex,
    float MinRange,
    float MaxRange,
    float Speed,
    uint ManaCost,
    uint DurationIndex,
    int DurationMs,
    IReadOnlyList<WorldSpellEffect> Effects);

public sealed record WorldCreatureSpell(
    uint SpellId,
    float Probability,
    int CastTarget,
    int TargetParam1,
    int TargetParam2,
    int CastFlags,
    int DelayInitialMin,
    int DelayInitialMax,
    int DelayRepeatMin,
    int DelayRepeatMax,
    uint ScriptId);

public sealed record WorldCreatureWaypoint(
    uint Point,
    float PositionX,
    float PositionY,
    float PositionZ,
    float Orientation,
    uint WaitTime,
    float WanderDistance,
    uint ScriptId);

public sealed record WorldLootItem(
    uint ItemEntry,
    float ChanceOrQuestChance,
    uint GroupId,
    int MinCountOrReference,
    uint MaxCount,
    uint ConditionId);

public sealed record WorldCreatureAiEvent(
    uint Id,
    uint CreatureEntry,
    uint ConditionId,
    int EventType,
    int InversePhaseMask,
    int Chance,
    int EventFlags,
    int EventParam1,
    int EventParam2,
    int EventParam3,
    int EventParam4,
    uint Action1Script,
    uint Action2Script,
    uint Action3Script,
    string Comment);

public sealed record WorldScriptCommand(
    uint Id,
    int Delay,
    int Priority,
    int Command,
    int DataLong,
    int DataLong2,
    int DataLong3,
    int DataLong4,
    int TargetParam1,
    int TargetParam2,
    int TargetType,
    int DataFlags,
    int DataInt,
    int DataInt2,
    int DataInt3,
    int DataInt4,
    float X,
    float Y,
    float Z,
    float O,
    uint ConditionId,
    string Comments);

public sealed record WorldBroadcastText(
    uint Id,
    string MaleText,
    string FemaleText,
    int ChatType,
    int LanguageId);

public interface IWorldContentRepository
{
    ValueTask<WorldCreatureTemplate?> GetCreatureTemplateAsync(
        uint entry,
        CancellationToken cancellationToken = default);

    ValueTask<WorldGameObjectTemplate?> GetGameObjectTemplateAsync(
        uint entry,
        CancellationToken cancellationToken = default);

    ValueTask<IReadOnlyList<WorldGameObjectSpawn>> GetGameObjectSpawnsNearAsync(
        int map,
        float positionX,
        float positionY,
        float positionZ,
        float radius,
        int limit = 128,
        CancellationToken cancellationToken = default);

    ValueTask<WorldQuestTemplate?> GetQuestTemplateAsync(
        uint entry,
        CancellationToken cancellationToken = default);

    ValueTask<WorldCreatureSpawn?> GetCreatureSpawnAsync(
        ulong guid,
        CancellationToken cancellationToken = default);

    ValueTask<IReadOnlyList<uint>> GetCreatureQuestIdsAsync(
        uint creatureEntry,
        bool starter,
        CancellationToken cancellationToken = default);

    ValueTask<IReadOnlyList<WorldVendorItem>> GetVendorItemsAsync(
        uint creatureEntry,
        uint vendorId,
        CancellationToken cancellationToken = default);

    ValueTask<IReadOnlyList<WorldCreatureSpawn>> GetCreatureSpawnsNearAsync(
        int map,
        float positionX,
        float positionY,
        float positionZ,
        float radius,
        int limit = 128,
        CancellationToken cancellationToken = default);

    ValueTask<WorldSpellDefinition?> GetSpellDefinitionAsync(
        uint spellId,
        CancellationToken cancellationToken = default);

    ValueTask<IReadOnlyList<WorldCreatureSpell>> GetCreatureSpellsAsync(
        uint creatureEntry,
        CancellationToken cancellationToken = default);

    ValueTask<IReadOnlyList<WorldCreatureAiEvent>> GetCreatureAiEventsAsync(
        uint creatureEntry,
        CancellationToken cancellationToken = default);

    ValueTask<IReadOnlyList<WorldScriptCommand>> GetCreatureAiScriptsAsync(
        IReadOnlyCollection<uint> scriptIds,
        CancellationToken cancellationToken = default);

    ValueTask<IReadOnlyList<WorldScriptCommand>> GetGenericScriptsAsync(
        IReadOnlyCollection<uint> scriptIds,
        CancellationToken cancellationToken = default);

    ValueTask<WorldBroadcastText?> GetBroadcastTextAsync(
        uint entry,
        CancellationToken cancellationToken = default);

    ValueTask<IReadOnlyList<WorldCreatureWaypoint>> GetCreatureWaypointsAsync(
        uint spawnId,
        uint creatureEntry,
        CancellationToken cancellationToken = default);

    ValueTask<IReadOnlyList<WorldLootItem>> GetCreatureLootAsync(
        uint creatureEntry,
        CancellationToken cancellationToken = default);

    ValueTask<IReadOnlyList<WorldLootItem>> GetReferenceLootAsync(
        uint referenceEntry,
        CancellationToken cancellationToken = default);
}

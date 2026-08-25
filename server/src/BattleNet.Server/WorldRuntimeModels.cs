using BattleNet.Persistence.Models;

namespace BattleNet.Server;

internal sealed class PlayerRuntimeState
{
    public PlayerRuntimeState(CharacterSummary character, IReadOnlyList<CharacterSpellRecord> spells)
    {
        Character = character;
        MaxHealth = Math.Max(100u, 80u + (character.Level * 25u));
        var legacyPlaceholderStats = character.Level <= 1
            && character.Health <= 3
            && character.Power1 <= 25;
        // A persisted zero is an actual dead character and must survive a
        // reconnect so the client can reopen PLAYER_DEAD/Release Spirit.
        // Health 1-3 is retained as the old reconstructed-table placeholder.
        Health = character.Health == 0
            ? 0
            : legacyPlaceholderStats || character.Health <= 1
                ? MaxHealth
                : Math.Min(MaxHealth, character.Health);
        MaxPower = 1000;
        Power = legacyPlaceholderStats || character.Power1 == 0
            ? MaxPower
            : Math.Min(MaxPower, character.Power1);
        KnownSpells = spells.Where(spell => spell.Active && !spell.Disabled).Select(spell => spell.SpellId).ToHashSet();
    }

    public CharacterSummary Character { get; set; }
    public uint Health { get; set; }
    public uint MaxHealth { get; }
    public uint Power { get; set; }
    public uint MaxPower { get; }
    public HashSet<uint> KnownSpells { get; }
    public Dictionary<uint, DateTimeOffset> SpellCooldowns { get; } = new();
    public Dictionary<uint, ActiveAuraRuntime> Auras { get; } = new();
    public ulong ActiveLootGuid { get; set; }
    public DateTimeOffset LastStateSave { get; set; }
    public PendingPlayerSpellCast? PendingSpellCast { get; set; }
    public PlayerCorpseRuntime? Corpse { get; set; }
    public PendingPlayerTeleport? PendingTeleport { get; set; }
    public bool HasResurrectionSickness { get; set; }
}

internal sealed record PlayerCorpseRuntime(
    ulong Guid,
    ulong OwnerGuid,
    uint Map,
    float PositionX,
    float PositionY,
    float PositionZ,
    float Orientation,
    uint DisplayId,
    float Scale,
    uint Bytes1,
    uint Bytes2,
    uint GuildId,
    uint Level,
    IReadOnlyList<uint> ItemFields,
    DateTimeOffset ExpiresAt);

internal sealed record PendingPlayerTeleport(
    uint Map,
    uint Zone,
    float PositionX,
    float PositionY,
    float PositionZ,
    float Orientation,
    bool IsResurrection,
    bool IsLongTeleport);

internal sealed record PendingPlayerSpellCast(
    WorldSpellDefinition Spell,
    ushort TargetMask,
    ulong TargetGuid,
    DateTimeOffset CompletesAt,
    uint ConsumedItemEntry);

internal sealed record PendingCreatureSpellCast(
    WorldSpellDefinition Spell,
    DateTimeOffset CompletesAt);

internal sealed class ActiveAuraRuntime(
    WorldSpellDefinition spell,
    DateTimeOffset expiresAt,
    DateTimeOffset nextTickAt,
    int effectIndex)
{
    public WorldSpellDefinition Spell { get; } = spell;
    public DateTimeOffset ExpiresAt { get; } = expiresAt;
    public DateTimeOffset NextTickAt { get; set; } = nextTickAt;
    public int EffectIndex { get; } = effectIndex;
}

internal sealed class CreatureRuntimeState
{
    public CreatureRuntimeState(
        WorldCreatureSpawn spawn,
        WorldCreatureTemplate template,
        IReadOnlyList<WorldCreatureWaypoint> waypoints,
        IReadOnlyList<WorldCreatureSpell> spells,
        IReadOnlyList<WorldCreatureAiEvent> aiEvents,
        IReadOnlyDictionary<uint, IReadOnlyList<WorldScriptCommand>> aiScripts)
    {
        Spawn = spawn;
        Template = template;
        PositionX = spawn.PositionX;
        PositionY = spawn.PositionY;
        PositionZ = spawn.PositionZ;
        Orientation = spawn.Orientation;
        MaxHealth = Math.Max(100u, Math.Max(template.LevelMin, template.LevelMax) * 25u);
        Health = MaxHealth;
        Waypoints = waypoints;
        Spells = spells;
        AiEvents = aiEvents;
        AiScripts = aiScripts;
        WaypointIndex = waypoints.Count > 1 ? 1 : 0;
        var now = DateTimeOffset.UtcNow;
        NextMeleeAttack = now.AddMilliseconds(Math.Max(250, template.BaseAttackTime));
        NextSpellCast = now.AddSeconds(spells.Count == 0 ? 1 : Math.Max(1, spells.Min(spell => spell.DelayInitialMin)));
    }

    public WorldCreatureSpawn Spawn { get; }
    public WorldCreatureTemplate Template { get; }
    public IReadOnlyList<WorldCreatureWaypoint> Waypoints { get; }
    public IReadOnlyList<WorldCreatureSpell> Spells { get; }
    public IReadOnlyList<WorldCreatureAiEvent> AiEvents { get; }
    public IReadOnlyDictionary<uint, IReadOnlyList<WorldScriptCommand>> AiScripts { get; }
    public float PositionX { get; set; }
    public float PositionY { get; set; }
    public float PositionZ { get; set; }
    public float Orientation { get; set; }
    public uint Health { get; set; }
    public uint MaxHealth { get; }
    public ulong ThreatTargetGuid { get; set; }
    public int WaypointIndex { get; set; }
    public DateTimeOffset NextMeleeAttack { get; set; }
    public DateTimeOffset NextSpellCast { get; set; }
    public PendingCreatureSpellCast? PendingSpellCast { get; set; }
    public DateTimeOffset NextPatrolMove { get; set; }
    public DateTimeOffset RespawnAt { get; set; }
    public Dictionary<uint, DateTimeOffset> AiEventNext { get; } = new();
    public HashSet<uint> AiOneShotEventsTriggered { get; } = new();
    public int AiPhase { get; set; }
    public bool WasInCombat { get; set; }
    public bool MeleeAttackEnabled { get; set; } = true;
    public bool CombatMovementEnabled { get; set; } = true;
    public bool Invincible { get; set; }
    public DateTimeOffset FleeUntil { get; set; }
    public bool IsDead => Health == 0;
}

internal sealed class LootRuntimeState
{
    public LootRuntimeState(ulong objectGuid, uint money, IReadOnlyList<LootRuntimeItem> items, bool shared = false)
    {
        ObjectGuid = objectGuid;
        Money = money;
        Items = items.ToList();
        Shared = shared;
    }

    public ulong ObjectGuid { get; }
    public uint Money { get; set; }
    public List<LootRuntimeItem> Items { get; }
    public bool Shared { get; }
}

internal sealed class LootRuntimeItem(uint itemEntry, uint count, uint displayId, int slot = 0)
{
    public uint ItemEntry { get; } = itemEntry;
    public uint Count { get; } = count;
    public uint DisplayId { get; } = displayId;
    public int Slot { get; } = slot;
    public bool Claimed { get; set; }
}

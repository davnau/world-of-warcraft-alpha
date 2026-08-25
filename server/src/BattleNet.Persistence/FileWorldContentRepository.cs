using System.Text.Json;
using BattleNet.Core.Configuration;
using BattleNet.Persistence.Models;

namespace BattleNet.Persistence;

/// <summary>
/// Reads the complete Alpha Core world JSON export from tables/world.
/// Tables remain generic on disk so future systems can consume additional
/// Alpha tables without changing the importer.
/// </summary>
public sealed class FileWorldContentRepository : IWorldContentRepository
{
    private readonly object _gate = new();
    private readonly string _directory;
    private readonly Dictionary<string, IReadOnlyList<Dictionary<string, JsonElement>>> _tables = new(StringComparer.OrdinalIgnoreCase);

    public FileWorldContentRepository(ServerOptions options)
        : this(options.DataDirectory)
    {
    }

    public FileWorldContentRepository(string dataDirectory)
    {
        _directory = Path.Combine(dataDirectory, "world");
    }

    public ValueTask<WorldCreatureTemplate?> GetCreatureTemplateAsync(
        uint entry,
        CancellationToken cancellationToken = default)
    {
        var row = FindByUInt("creature_template", "entry", entry);
        if (row is null)
            return ValueTask.FromResult<WorldCreatureTemplate?>(null);

        return ValueTask.FromResult<WorldCreatureTemplate?>(new WorldCreatureTemplate(
            entry,
            ReadUInt(row, "display_id1"),
            ReadString(row, "name"),
            ReadString(row, "subname"),
            ReadUInt(row, "static_flags"),
            ReadUInt(row, "level_min"),
            ReadUInt(row, "level_max"),
            ReadUInt(row, "faction"),
            ReadUInt(row, "type"),
            ReadUInt(row, "beast_family"),
            ReadUInt(row, "npc_flags"),
            ReadUInt(row, "vendor_id"),
            ReadUInt(row, "loot_id"),
            ReadUInt(row, "spell_list_id"),
            ReadUInt(row, "movement_type"),
            ReadFloat(row, "speed_run"),
            ReadFloat(row, "detection_range"),
            ReadFloat(row, "leash_range"),
            ReadUInt(row, "base_attack_time"),
            ReadUInt(row, "gold_min"),
            ReadUInt(row, "gold_max"),
            ReadUInt(row, "civilian")));
    }

    public ValueTask<WorldGameObjectTemplate?> GetGameObjectTemplateAsync(
        uint entry,
        CancellationToken cancellationToken = default)
    {
        var row = FindByUInt("gameobject_template", "entry", entry);
        if (row is null)
            return ValueTask.FromResult<WorldGameObjectTemplate?>(null);

        var data = Enumerable.Range(0, 10)
            .Select(index => ReadUInt(row, $"data{index}"))
            .ToArray();
        return ValueTask.FromResult<WorldGameObjectTemplate?>(new WorldGameObjectTemplate(
            entry,
            ReadUInt(row, "type"),
            ReadUInt(row, "displayId"),
            ReadString(row, "name"),
            ReadUInt(row, "faction"),
            ReadUInt(row, "flags"),
            ReadFloat(row, "size"),
            data));
    }

    public ValueTask<IReadOnlyList<WorldGameObjectSpawn>> GetGameObjectSpawnsNearAsync(
        int map,
        float positionX,
        float positionY,
        float positionZ,
        float radius,
        int limit = 128,
        CancellationToken cancellationToken = default)
    {
        var radiusSquared = radius * radius;
        var spawns = LoadTable("spawns_gameobjects")
            .Where(row => ReadInt(row, "spawn_map") == map && ReadInt(row, "ignored") == 0)
            .Select(row => new
            {
                Row = row,
                DistanceSquared = DistanceSquared(
                    ReadFloat(row, "spawn_positionX"),
                    ReadFloat(row, "spawn_positionY"),
                    ReadFloat(row, "spawn_positionZ"),
                    positionX,
                    positionY,
                    positionZ)
            })
            .Where(item => item.DistanceSquared <= radiusSquared)
            .OrderBy(item => item.DistanceSquared)
            .Take(Math.Max(0, limit))
            .Select(item => new WorldGameObjectSpawn(
                ReadUInt(item.Row, "spawn_id"),
                ReadUInt(item.Row, "spawn_entry"),
                map,
                ReadFloat(item.Row, "spawn_positionX"),
                ReadFloat(item.Row, "spawn_positionY"),
                ReadFloat(item.Row, "spawn_positionZ"),
                ReadFloat(item.Row, "spawn_orientation"),
                ReadFloat(item.Row, "spawn_rotation0"),
                ReadFloat(item.Row, "spawn_rotation1"),
                ReadFloat(item.Row, "spawn_rotation2"),
                ReadFloat(item.Row, "spawn_rotation3"),
                ReadUInt(item.Row, "spawn_state")))
            .Where(spawn => spawn.Entry > 0)
            .ToArray();
        return ValueTask.FromResult<IReadOnlyList<WorldGameObjectSpawn>>(spawns);
    }

    public async ValueTask<WorldQuestTemplate?> GetQuestTemplateAsync(
        uint entry,
        CancellationToken cancellationToken = default)
    {
        var row = FindByUInt("quest_template", "entry", entry);
        if (row is null)
            return null;

        var requirements = new List<QuestRequirement>(4);
        for (var index = 1; index <= 4; index++)
        {
            var targetId = ReadInt(row, $"ReqCreatureOrGOId{index}");
            var itemId = ReadUInt(row, $"ReqItemId{index}");
            if (targetId == 0 && itemId == 0)
                continue;

            var name = string.Empty;
            if (targetId > 0)
            {
                var creature = await GetCreatureTemplateAsync((uint)targetId, cancellationToken).ConfigureAwait(false);
                name = creature?.Name ?? string.Empty;
            }
            else if (targetId < 0)
            {
                var gameObject = await GetGameObjectTemplateAsync((uint)-targetId, cancellationToken).ConfigureAwait(false);
                name = gameObject?.Name ?? string.Empty;
            }

            requirements.Add(new QuestRequirement(
                targetId,
                ReadUInt(row, $"ReqCreatureOrGOCount{index}"),
                itemId,
                ReadUInt(row, $"ReqItemCount{index}"),
                name));
        }

        return new WorldQuestTemplate(
            entry,
            ReadUInt(row, "Method"),
            ReadUInt(row, "QuestLevel"),
            ReadInt(row, "ZoneOrSort"),
            ReadUInt(row, "Type"),
            ReadUInt(row, "NextQuestInChain"),
            ReadInt(row, "RewOrReqMoney"),
            ReadUInt(row, "SrcItemId"),
            ReadUIntArray(row, "RewItemId"),
            ReadUIntArray(row, "RewItemCount"),
            ReadUIntArray(row, "RewChoiceItemId"),
            ReadUIntArray(row, "RewChoiceItemCount"),
            ReadString(row, "Title"),
            ReadString(row, "Details"),
            ReadString(row, "Objectives"),
            ReadString(row, "EndText"),
            ReadString(row, "RequestItemsText"),
            ReadString(row, "OfferRewardText"),
            ReadUInt(row, "IncompleteEmote"),
            ReadUInt(row, "CompleteEmote"),
            ReadUInt(row, "PointMapId"),
            ReadFloat(row, "PointX"),
            ReadFloat(row, "PointY"),
            ReadUInt(row, "PointOpt"),
            requirements);
    }

    public ValueTask<WorldCreatureSpawn?> GetCreatureSpawnAsync(
        ulong guid,
        CancellationToken cancellationToken = default)
    {
        var spawnId = unchecked((uint)guid);
        var row = FindByUInt("spawns_creatures", "spawn_id", spawnId);
        if (row is null)
            return ValueTask.FromResult<WorldCreatureSpawn?>(null);

        return ValueTask.FromResult<WorldCreatureSpawn?>(new WorldCreatureSpawn(
            spawnId,
            ReadUInt(row, "spawn_entry1"),
            ReadInt(row, "map"),
            ReadFloat(row, "position_x"),
            ReadFloat(row, "position_y"),
            ReadFloat(row, "position_z"),
            ReadFloat(row, "orientation"),
            ReadUInt(row, "movement_type"),
            ReadFloat(row, "wander_distance")));
    }

    public ValueTask<IReadOnlyList<uint>> GetCreatureQuestIdsAsync(
        uint creatureEntry,
        bool starter,
        CancellationToken cancellationToken = default)
    {
        var table = starter ? "creature_quest_starter" : "creature_quest_finisher";
        var quests = LoadTable(table)
            .Where(row => ReadUInt(row, "entry") == creatureEntry)
            .Select(row => ReadUInt(row, "quest"))
            .Where(quest => quest > 0)
            .Distinct()
            .OrderBy(quest => quest)
            .ToArray();
        return ValueTask.FromResult<IReadOnlyList<uint>>(quests);
    }

    public ValueTask<IReadOnlyList<WorldVendorItem>> GetVendorItemsAsync(
        uint creatureEntry,
        uint vendorId,
        CancellationToken cancellationToken = default)
    {
        var table = vendorId == 0 ? "npc_vendor" : "npc_vendor_template";
        var key = vendorId == 0 ? creatureEntry : vendorId;
        var items = new List<WorldVendorItem>();
        foreach (var row in LoadTable(table).Where(row => ReadUInt(row, "entry") == key))
        {
            var itemEntry = ReadUInt(row, "item");
            var item = FindByUInt("item_template", "entry", itemEntry);
            if (item is null)
                continue;

            items.Add(new WorldVendorItem(
                itemEntry,
                ReadUInt(row, "maxcount"),
                ReadUInt(item, "buy_price"),
                ReadUInt(item, "max_durability"),
                Math.Max(1u, ReadUInt(item, "buy_count")),
                ReadUInt(item, "display_id")));
        }

        return ValueTask.FromResult<IReadOnlyList<WorldVendorItem>>(items);
    }

    public ValueTask<IReadOnlyList<WorldCreatureSpawn>> GetCreatureSpawnsNearAsync(
        int map,
        float positionX,
        float positionY,
        float positionZ,
        float radius,
        int limit = 128,
        CancellationToken cancellationToken = default)
    {
        var radiusSquared = radius * radius;
        var spawns = LoadTable("spawns_creatures")
            .Where(row => ReadInt(row, "map") == map && ReadInt(row, "ignored") == 0)
            .Select(row => new
            {
                Row = row,
                DistanceSquared = DistanceSquared(
                    ReadFloat(row, "position_x"),
                    ReadFloat(row, "position_y"),
                    ReadFloat(row, "position_z"),
                    positionX,
                    positionY,
                    positionZ)
            })
            .Where(item => item.DistanceSquared <= radiusSquared)
            .OrderBy(item => item.DistanceSquared)
            .Take(Math.Max(0, limit))
            .Select(item => new WorldCreatureSpawn(
                ReadUInt(item.Row, "spawn_id"),
                ReadUInt(item.Row, "spawn_entry1"),
                map,
                ReadFloat(item.Row, "position_x"),
                ReadFloat(item.Row, "position_y"),
                ReadFloat(item.Row, "position_z"),
                ReadFloat(item.Row, "orientation"),
                ReadUInt(item.Row, "movement_type"),
                ReadFloat(item.Row, "wander_distance")))
            .Where(spawn => spawn.Entry > 0)
            .ToArray();
        return ValueTask.FromResult<IReadOnlyList<WorldCreatureSpawn>>(spawns);
    }

    public ValueTask<WorldSpellDefinition?> GetSpellDefinitionAsync(
        uint spellId,
        CancellationToken cancellationToken = default)
    {
        var row = FindByUInt("spell", "id", spellId);
        if (row is null)
            return ValueTask.FromResult<WorldSpellDefinition?>(null);

        var effects = Enumerable.Range(1, 3)
            .Select(index => new WorldSpellEffect(
                ReadInt(row, $"effect_type{index}"),
                ReadInt(row, $"die_sides{index}"),
                ReadInt(row, $"base_dice{index}"),
                ReadInt(row, $"dice_per_level{index}"),
                ReadFloat(row, $"real_points_per_level{index}"),
                ReadInt(row, $"base_points{index}"),
                ReadInt(row, $"implicit_target_a{index}"),
                ReadInt(row, $"implicit_target_b{index}"),
                ReadInt(row, $"radius_index{index}"),
                ReadInt(row, $"aura_type{index}"),
                ReadInt(row, $"aura_period{index}"),
                ReadFloat(row, $"amplitude{index}"),
                ReadInt(row, $"chain_targets{index}"),
                ReadInt(row, $"item_type{index}"),
                ReadInt(row, $"misc_value{index}"),
                ReadUInt(row, $"trigger_spell{index}")))
            .ToArray();

        return ValueTask.FromResult<WorldSpellDefinition?>(new WorldSpellDefinition(
            spellId,
            ReadUInt(row, "school"),
            ReadUInt(row, "attributes"),
            ReadUInt(row, "attributes_ex"),
            ReadUInt(row, "targets"),
            ReadUInt(row, "casting_time_index"),
            ReadInt(row, "cast_time_ms"),
            ReadInt(row, "recovery_time"),
            ReadInt(row, "category_recovery_time"),
            ReadUInt(row, "range_index"),
            ReadFloat(row, "min_range"),
            ReadFloat(row, "max_range"),
            ReadFloat(row, "speed"),
            ReadUInt(row, "mana_cost"),
            ReadUInt(row, "duration_index"),
            ReadInt(row, "duration_ms"),
            effects));
    }

    public ValueTask<IReadOnlyList<WorldCreatureSpell>> GetCreatureSpellsAsync(
        uint creatureEntry,
        CancellationToken cancellationToken = default)
    {
        var template = FindByUInt("creature_template", "entry", creatureEntry);
        var spellListId = template is null ? 0 : ReadUInt(template, "spell_list_id");
        var row = FindByUInt("creature_spells", "entry", spellListId > 0 ? spellListId : creatureEntry);
        if (row is null)
            return ValueTask.FromResult<IReadOnlyList<WorldCreatureSpell>>(Array.Empty<WorldCreatureSpell>());

        var spells = Enumerable.Range(1, 8)
            .Select(index => new WorldCreatureSpell(
                ReadUInt(row, $"spellId_{index}"),
                ReadFloat(row, $"probability_{index}"),
                ReadInt(row, $"castTarget_{index}"),
                ReadInt(row, $"targetParam1_{index}"),
                ReadInt(row, $"targetParam2_{index}"),
                ReadInt(row, $"castFlags_{index}"),
                ReadInt(row, $"delayInitialMin_{index}"),
                ReadInt(row, $"delayInitialMax_{index}"),
                ReadInt(row, $"delayRepeatMin_{index}"),
                ReadInt(row, $"delayRepeatMax_{index}"),
                ReadUInt(row, $"scriptId_{index}")))
            .Where(spell => spell.SpellId > 0)
            .ToArray();
        return ValueTask.FromResult<IReadOnlyList<WorldCreatureSpell>>(spells);
    }

    public ValueTask<IReadOnlyList<WorldCreatureAiEvent>> GetCreatureAiEventsAsync(
        uint creatureEntry,
        CancellationToken cancellationToken = default)
    {
        var events = LoadTable("creature_ai_events")
            .Where(row => ReadUInt(row, "creature_id") == creatureEntry)
            .Select(row => new WorldCreatureAiEvent(
                ReadUInt(row, "id"),
                creatureEntry,
                ReadUInt(row, "condition_id"),
                ReadInt(row, "event_type"),
                ReadInt(row, "event_inverse_phase_mask"),
                ReadInt(row, "event_chance"),
                ReadInt(row, "event_flags"),
                ReadInt(row, "event_param1"),
                ReadInt(row, "event_param2"),
                ReadInt(row, "event_param3"),
                ReadInt(row, "event_param4"),
                ReadUInt(row, "action1_script"),
                ReadUInt(row, "action2_script"),
                ReadUInt(row, "action3_script"),
                ReadString(row, "comment")))
            .ToArray();
        return ValueTask.FromResult<IReadOnlyList<WorldCreatureAiEvent>>(events);
    }

    public ValueTask<IReadOnlyList<WorldScriptCommand>> GetCreatureAiScriptsAsync(
        IReadOnlyCollection<uint> scriptIds,
        CancellationToken cancellationToken = default) =>
        ValueTask.FromResult<IReadOnlyList<WorldScriptCommand>>(ReadScriptCommands("creature_ai_scripts", scriptIds));

    public ValueTask<IReadOnlyList<WorldScriptCommand>> GetGenericScriptsAsync(
        IReadOnlyCollection<uint> scriptIds,
        CancellationToken cancellationToken = default) =>
        ValueTask.FromResult<IReadOnlyList<WorldScriptCommand>>(ReadScriptCommands("generic_scripts", scriptIds));

    public ValueTask<WorldBroadcastText?> GetBroadcastTextAsync(
        uint entry,
        CancellationToken cancellationToken = default)
    {
        var row = FindByUInt("broadcast_text", "entry", entry);
        if (row is null)
            return ValueTask.FromResult<WorldBroadcastText?>(null);

        return ValueTask.FromResult<WorldBroadcastText?>(new WorldBroadcastText(
            entry,
            ReadString(row, "male_text"),
            ReadString(row, "female_text"),
            ReadInt(row, "chat_type"),
            ReadInt(row, "language_id")));
    }

    public ValueTask<IReadOnlyList<WorldCreatureWaypoint>> GetCreatureWaypointsAsync(
        uint spawnId,
        uint creatureEntry,
        CancellationToken cancellationToken = default)
    {
        var rows = LoadTable("creature_movement")
            .Where(row => ReadUInt(row, "id") == spawnId)
            .ToArray();
        if (rows.Length == 0)
        {
            rows = LoadTable("creature_movement_template")
                .Where(row => ReadUInt(row, "entry") == creatureEntry)
                .ToArray();
        }

        var waypoints = rows
            .OrderBy(row => ReadUInt(row, "point"))
            .Select(row => new WorldCreatureWaypoint(
                ReadUInt(row, "point"),
                ReadFloat(row, "position_x"),
                ReadFloat(row, "position_y"),
                ReadFloat(row, "position_z"),
                ReadFloat(row, "orientation"),
                ReadUInt(row, "waittime"),
                ReadFloat(row, "wander_distance"),
                ReadUInt(row, "script_id")))
            .ToArray();
        return ValueTask.FromResult<IReadOnlyList<WorldCreatureWaypoint>>(waypoints);
    }

    public ValueTask<IReadOnlyList<WorldLootItem>> GetCreatureLootAsync(
        uint creatureEntry,
        CancellationToken cancellationToken = default) =>
        ValueTask.FromResult<IReadOnlyList<WorldLootItem>>(ReadLootTable("creature_loot_template", creatureEntry));

    public ValueTask<IReadOnlyList<WorldLootItem>> GetReferenceLootAsync(
        uint referenceEntry,
        CancellationToken cancellationToken = default) =>
        ValueTask.FromResult<IReadOnlyList<WorldLootItem>>(ReadLootTable("reference_loot_template", referenceEntry));

    private IReadOnlyList<WorldLootItem> ReadLootTable(string table, uint entry)
    {
        return LoadTable(table)
            .Where(row => ReadUInt(row, "entry") == entry)
            .Select(row => new WorldLootItem(
                ReadUInt(row, "item"),
                ReadFloat(row, "ChanceOrQuestChance"),
                ReadUInt(row, "groupid"),
                ReadInt(row, "mincountOrRef"),
                ReadUInt(row, "maxcount"),
                ReadUInt(row, "condition_id")))
            .Where(item => item.ItemEntry > 0)
            .ToArray();
    }

    private IReadOnlyList<WorldScriptCommand> ReadScriptCommands(
        string table,
        IReadOnlyCollection<uint> scriptIds)
    {
        if (scriptIds.Count == 0)
            return Array.Empty<WorldScriptCommand>();

        var wanted = scriptIds.ToHashSet();
        return LoadTable(table)
            .Where(row => wanted.Contains(ReadUInt(row, "id")))
            .Select(row => new WorldScriptCommand(
                ReadUInt(row, "id"),
                ReadInt(row, "delay"),
                ReadInt(row, "priority"),
                ReadInt(row, "command"),
                ReadInt(row, "datalong"),
                ReadInt(row, "datalong2"),
                ReadInt(row, "datalong3"),
                ReadInt(row, "datalong4"),
                ReadInt(row, "target_param1"),
                ReadInt(row, "target_param2"),
                ReadInt(row, "target_type"),
                ReadInt(row, "data_flags"),
                ReadInt(row, "dataint"),
                ReadInt(row, "dataint2"),
                ReadInt(row, "dataint3"),
                ReadInt(row, "dataint4"),
                ReadFloat(row, "x"),
                ReadFloat(row, "y"),
                ReadFloat(row, "z"),
                ReadFloat(row, "o"),
                ReadUInt(row, "condition_id"),
                ReadString(row, "comments")))
            .OrderBy(command => command.Priority)
            .ThenBy(command => command.Delay)
            .ToArray();
    }

    private Dictionary<string, JsonElement>? FindByUInt(string table, string key, uint value)
    {
        foreach (var row in LoadTable(table))
        {
            if (ReadUInt(row, key) == value)
                return row;
        }
        return null;
    }

    private IReadOnlyList<Dictionary<string, JsonElement>> LoadTable(string table)
    {
        lock (_gate)
        {
            if (_tables.TryGetValue(table, out var cached))
                return cached;

            var path = Path.Combine(_directory, table + ".json");
            if (!File.Exists(path))
            {
                cached = Array.Empty<Dictionary<string, JsonElement>>();
                _tables[table] = cached;
                return cached;
            }

            cached = JsonSerializer.Deserialize<List<Dictionary<string, JsonElement>>>(
                File.ReadAllText(path),
                new JsonSerializerOptions { PropertyNameCaseInsensitive = true })
                ?? [];
            _tables[table] = cached;
            return cached;
        }
    }

    private static JsonElement? Find(Dictionary<string, JsonElement> row, string key)
    {
        foreach (var pair in row)
        {
            if (string.Equals(pair.Key, key, StringComparison.OrdinalIgnoreCase))
                return pair.Value;
        }
        return null;
    }

    private static int ReadInt(Dictionary<string, JsonElement> row, string key)
    {
        var value = Find(row, key);
        if (value is null || value.Value.ValueKind == JsonValueKind.Null)
            return 0;
        if (value.Value.ValueKind == JsonValueKind.Number && value.Value.TryGetInt32(out var integer))
            return integer;
        if (value.Value.ValueKind == JsonValueKind.String && int.TryParse(value.Value.GetString(), out integer))
            return integer;
        return (int)value.Value.GetDouble();
    }

    private static uint ReadUInt(Dictionary<string, JsonElement> row, string key)
    {
        var value = Find(row, key);
        if (value is null || value.Value.ValueKind == JsonValueKind.Null)
            return 0;
        if (value.Value.ValueKind == JsonValueKind.Number && value.Value.TryGetUInt32(out var integer))
            return integer;
        if (value.Value.ValueKind == JsonValueKind.String && uint.TryParse(value.Value.GetString(), out integer))
            return integer;
        return unchecked((uint)ReadInt(row, key));
    }

    private static float ReadFloat(Dictionary<string, JsonElement> row, string key)
    {
        var value = Find(row, key);
        if (value is null || value.Value.ValueKind == JsonValueKind.Null)
            return 0;
        if (value.Value.ValueKind == JsonValueKind.Number && value.Value.TryGetSingle(out var number))
            return number;
        return float.TryParse(value.Value.GetString(), out number) ? number : 0;
    }

    private static string ReadString(Dictionary<string, JsonElement> row, string key)
    {
        var value = Find(row, key);
        return value is null || value.Value.ValueKind == JsonValueKind.Null
            ? string.Empty
            : value.Value.GetString() ?? string.Empty;
    }

    private static uint[] ReadUIntArray(Dictionary<string, JsonElement> row, string prefix)
    {
        var values = new uint[6];
        for (var index = 1; index <= values.Length; index++)
            values[index - 1] = ReadUInt(row, prefix + index);
        return values;
    }

    private static float DistanceSquared(
        float x1,
        float y1,
        float z1,
        float x2,
        float y2,
        float z2)
    {
        var x = x1 - x2;
        var y = y1 - y2;
        var z = z1 - z2;
        return (x * x) + (y * y) + (z * z);
    }
}

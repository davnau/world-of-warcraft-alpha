using System.Globalization;
using BattleNet.Persistence.Models;
using MySqlConnector;

namespace BattleNet.Persistence;

public sealed class InMemoryWorldRepository : IWorldRepository
{
    private readonly Dictionary<uint, ItemTemplateRecord> _items = new();

    public void AddItemTemplate(ItemTemplateRecord template) => _items[template.Entry] = template;

    public ValueTask<IReadOnlyList<uint>> GetStartingSpellIdsAsync(
        byte race,
        byte classId,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyList<uint> result = Array.Empty<uint>();
        return ValueTask.FromResult(result);
    }

    public ValueTask<IReadOnlyDictionary<int, int>> GetStartingActionButtonsAsync(
        byte race,
        byte classId,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyDictionary<int, int> result = new Dictionary<int, int>();
        return ValueTask.FromResult(result);
    }

    public ValueTask<IReadOnlyList<StartingItemRecord>> GetStartingItemRecordsAsync(
        byte race,
        byte classId,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyList<StartingItemRecord> result = Array.Empty<StartingItemRecord>();
        return ValueTask.FromResult(result);
    }

    public ValueTask<ItemTemplateRecord?> GetItemTemplateAsync(
        uint entry,
        CancellationToken cancellationToken = default) =>
        ValueTask.FromResult(_items.TryGetValue(entry, out var template) ? template : null);
}

public sealed class MySqlWorldRepository(string connectionString) : IWorldRepository
{
    public async ValueTask<IReadOnlyList<uint>> GetStartingSpellIdsAsync(
        byte race,
        byte classId,
        CancellationToken cancellationToken = default)
    {
        await using var connection = await OpenConnectionAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT Spell
            FROM playercreateinfo_spell
            WHERE race = @race AND `class` = @class_id
            ORDER BY Spell
            """;
        command.Parameters.AddWithValue("@race", race);
        command.Parameters.AddWithValue("@class_id", classId);

        var spells = new List<uint>();
        await using var reader = await command.ExecuteReaderAsync(cancellationToken).ConfigureAwait(false);
        while (await reader.ReadAsync(cancellationToken).ConfigureAwait(false))
            spells.Add(Convert.ToUInt32(reader.GetValue(0), CultureInfo.InvariantCulture));
        return spells;
    }

    public async ValueTask<IReadOnlyDictionary<int, int>> GetStartingActionButtonsAsync(
        byte race,
        byte classId,
        CancellationToken cancellationToken = default)
    {
        await using var connection = await OpenConnectionAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT button, action
            FROM playercreateinfo_action
            WHERE race = @race AND `class` = @class_id
            ORDER BY button
            """;
        command.Parameters.AddWithValue("@race", race);
        command.Parameters.AddWithValue("@class_id", classId);

        var buttons = new Dictionary<int, int>();
        await using var reader = await command.ExecuteReaderAsync(cancellationToken).ConfigureAwait(false);
        while (await reader.ReadAsync(cancellationToken).ConfigureAwait(false))
        {
            buttons[Convert.ToInt32(reader.GetValue(0), CultureInfo.InvariantCulture)] =
                Convert.ToInt32(reader.GetValue(1), CultureInfo.InvariantCulture);
        }
        return buttons;
    }

    public async ValueTask<IReadOnlyList<StartingItemRecord>> GetStartingItemRecordsAsync(
        byte race,
        byte classId,
        CancellationToken cancellationToken = default)
    {
        await using var connection = await OpenConnectionAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT itemid, amount
            FROM playercreateinfo_item
            WHERE race = @race AND `class` = @class_id
            ORDER BY id
            """;
        command.Parameters.AddWithValue("@race", race);
        command.Parameters.AddWithValue("@class_id", classId);

        var items = new List<StartingItemRecord>();
        await using var reader = await command.ExecuteReaderAsync(cancellationToken).ConfigureAwait(false);
        while (await reader.ReadAsync(cancellationToken).ConfigureAwait(false))
        {
            items.Add(new StartingItemRecord(
                Convert.ToUInt32(reader.GetValue(0), CultureInfo.InvariantCulture),
                Convert.ToUInt32(reader.GetValue(1), CultureInfo.InvariantCulture)));
        }
        return items;
    }

    public async ValueTask<ItemTemplateRecord?> GetItemTemplateAsync(
        uint entry,
        CancellationToken cancellationToken = default)
    {
        await using var connection = await OpenConnectionAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT entry, `class`, subclass, name, description, display_id, quality, flags,
                   buy_price, sell_price, inventory_type, allowable_class, allowable_race,
                   item_level, required_level, required_skill, required_skill_rank,
                   max_count, stackable, container_slots,
                   stat_type1, stat_value1, stat_type2, stat_value2, stat_type3, stat_value3,
                   stat_type4, stat_value4, stat_type5, stat_value5, stat_type6, stat_value6,
                   stat_type7, stat_value7, stat_type8, stat_value8, stat_type9, stat_value9,
                   stat_type10, stat_value10,
                   dmg_min1, dmg_max1, dmg_type1, dmg_min2, dmg_max2, dmg_type2,
                   dmg_min3, dmg_max3, dmg_type3, dmg_min4, dmg_max4, dmg_type4,
                   dmg_min5, dmg_max5, dmg_type5,
                   armor, holy_res, fire_res, nature_res, frost_res, shadow_res,
                   delay, ammo_type,
                   spellid_1, spelltrigger_1, spellcharges_1, spellcooldown_1, spellcategory_1, spellcategorycooldown_1,
                   spellid_2, spelltrigger_2, spellcharges_2, spellcooldown_2, spellcategory_2, spellcategorycooldown_2,
                   spellid_3, spelltrigger_3, spellcharges_3, spellcooldown_3, spellcategory_3, spellcategorycooldown_3,
                   spellid_4, spelltrigger_4, spellcharges_4, spellcooldown_4, spellcategory_4, spellcategorycooldown_4,
                   spellid_5, spelltrigger_5, spellcharges_5, spellcooldown_5, spellcategory_5, spellcategorycooldown_5,
                   bonding, page_text, page_language, page_material, start_quest, lock_id, material, sheath,
                   duration
            FROM item_template
            WHERE entry = @entry
            LIMIT 1
            """;
        command.Parameters.AddWithValue("@entry", entry);

        await using var reader = await command.ExecuteReaderAsync(cancellationToken).ConfigureAwait(false);
        return await reader.ReadAsync(cancellationToken).ConfigureAwait(false)
            ? ReadItemTemplate(reader)
            : null;
    }

    private static ItemTemplateRecord ReadItemTemplate(MySqlDataReader reader)
    {
        var ordinal = 0;
        int ReadInt() => Convert.ToInt32(reader.GetValue(ordinal++), CultureInfo.InvariantCulture);
        uint ReadUInt() => Convert.ToUInt32(reader.GetValue(ordinal++), CultureInfo.InvariantCulture);
        float ReadFloat() => Convert.ToSingle(reader.GetValue(ordinal++), CultureInfo.InvariantCulture);
        string ReadString()
        {
            var value = reader.IsDBNull(ordinal) ? string.Empty : Convert.ToString(reader.GetValue(ordinal), CultureInfo.InvariantCulture) ?? string.Empty;
            ordinal++;
            return value;
        }

        var entry = ReadUInt();
        var classId = ReadInt();
        var subclass = ReadInt();
        var name = ReadString();
        var description = ReadString();
        var displayId = ReadUInt();
        var quality = ReadUInt();
        var flags = ReadUInt();
        var buyPrice = ReadUInt();
        var sellPrice = ReadUInt();
        var inventoryType = ReadUInt();
        var allowableClass = ReadInt();
        var allowableRace = ReadInt();
        var itemLevel = ReadUInt();
        var requiredLevel = ReadUInt();
        var requiredSkill = ReadUInt();
        var requiredSkillRank = ReadUInt();
        var maxCount = ReadUInt();
        var stackable = ReadUInt();
        var containerSlots = ReadUInt();

        var stats = Enumerable.Range(0, 10)
            .Select(_ => new ItemStatRecord(ReadInt(), ReadInt()))
            .ToArray();
        var damages = Enumerable.Range(0, 5)
            .Select(_ => new ItemDamageRecord(ReadFloat(), ReadFloat(), ReadInt()))
            .ToArray();
        var armor = ReadInt();
        var holyResistance = ReadInt();
        var fireResistance = ReadInt();
        var natureResistance = ReadInt();
        var frostResistance = ReadInt();
        var shadowResistance = ReadInt();
        var delay = ReadInt();
        var ammoType = ReadUInt();
        var spells = Enumerable.Range(0, 5)
            .Select(_ => new ItemSpellRecord(ReadInt(), ReadInt(), ReadInt(), ReadInt(), ReadInt(), ReadInt()))
            .ToArray();

        return new ItemTemplateRecord(
            entry,
            classId,
            subclass,
            name,
            description,
            displayId,
            quality,
            flags,
            buyPrice,
            sellPrice,
            inventoryType,
            allowableClass,
            allowableRace,
            itemLevel,
            requiredLevel,
            requiredSkill,
            requiredSkillRank,
            maxCount,
            stackable,
            containerSlots,
            stats,
            damages,
            armor,
            holyResistance,
            fireResistance,
            natureResistance,
            frostResistance,
            shadowResistance,
            delay,
            ammoType,
            spells,
            ReadUInt(),
            ReadUInt(),
            ReadUInt(),
            ReadUInt(),
            ReadUInt(),
            ReadUInt(),
            ReadInt(),
            ReadUInt(),
            ReadUInt(),
            spells[0].Charges,
            spells[1].Charges,
            spells[2].Charges,
            spells[3].Charges,
            spells[4].Charges);
    }

    private async Task<MySqlConnection> OpenConnectionAsync(CancellationToken cancellationToken)
    {
        var connection = new MySqlConnection(connectionString);
        try
        {
            await connection.OpenAsync(cancellationToken).ConfigureAwait(false);
            return connection;
        }
        catch
        {
            await connection.DisposeAsync().ConfigureAwait(false);
            throw;
        }
    }
}

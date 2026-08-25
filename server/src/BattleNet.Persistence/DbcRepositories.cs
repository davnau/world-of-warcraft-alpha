using System.Globalization;
using BattleNet.Persistence.Models;
using MySqlConnector;

namespace BattleNet.Persistence;

/// <summary>
/// Alpha's eight playable ChrRaces rows. These values are copied from the
/// Alpha Core dbc.sql seed and keep local protocol tests useful without a DBC
/// MySQL server.
/// </summary>
public sealed class InMemoryDbcRepository : IDbcRepository
{
    private static readonly IReadOnlyDictionary<byte, PlayerRaceDefinition> Races =
        new Dictionary<byte, PlayerRaceDefinition>
        {
            [1] = new(1, 1, 49, 50, 1.0f, 7, 7),
            [2] = new(2, 2, 51, 52, 1.1f, 1, 7),
            [3] = new(3, 3, 53, 54, 1.0f, 7, 7),
            [4] = new(4, 4, 55, 56, 1.0f, 7, 7),
            [5] = new(5, 5, 57, 58, 1.0f, 6, 6),
            [6] = new(6, 6, 59, 60, 1.0f, 7, 7),
            [7] = new(7, 115, 1563, 1564, 0.8f, 7, 7),
            [8] = new(8, 116, 1478, 1479, 1.0f, 7, 7)
        };

    public ValueTask<PlayerRaceDefinition?> GetRaceDefinitionAsync(
        byte race,
        CancellationToken cancellationToken = default) =>
        ValueTask.FromResult(Races.TryGetValue(race, out var definition) ? definition : null);

    public ValueTask<IReadOnlyList<uint>> GetStartingOutfitItemIdsAsync(
        byte race,
        byte classId,
        byte gender,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyList<uint> result = Array.Empty<uint>();
        return ValueTask.FromResult(result);
    }
}

public sealed class MySqlDbcRepository(string connectionString) : IDbcRepository
{
    public async ValueTask<PlayerRaceDefinition?> GetRaceDefinitionAsync(
        byte race,
        CancellationToken cancellationToken = default)
    {
        await using var connection = new MySqlConnection(connectionString);
        await connection.OpenAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT ID, FactionID, MaleDisplayId, FemaleDisplayId,
                   MountScale, BaseLanguage, CreatureType
            FROM ChrRaces
            WHERE ID = @race
            LIMIT 1
            """;
        command.Parameters.AddWithValue("@race", race);

        await using var reader = await command.ExecuteReaderAsync(cancellationToken).ConfigureAwait(false);
        if (!await reader.ReadAsync(cancellationToken).ConfigureAwait(false))
            return null;

        return new PlayerRaceDefinition(
            Convert.ToByte(reader.GetValue(0), CultureInfo.InvariantCulture),
            Convert.ToUInt32(reader.GetValue(1), CultureInfo.InvariantCulture),
            Convert.ToUInt32(reader.GetValue(2), CultureInfo.InvariantCulture),
            Convert.ToUInt32(reader.GetValue(3), CultureInfo.InvariantCulture),
            Convert.ToSingle(reader.GetValue(4), CultureInfo.InvariantCulture),
            Convert.ToUInt32(reader.GetValue(5), CultureInfo.InvariantCulture),
            Convert.ToUInt32(reader.GetValue(6), CultureInfo.InvariantCulture));
    }

    public async ValueTask<IReadOnlyList<uint>> GetStartingOutfitItemIdsAsync(
        byte race,
        byte classId,
        byte gender,
        CancellationToken cancellationToken = default)
    {
        await using var connection = new MySqlConnection(connectionString);
        await connection.OpenAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT ItemID_1, ItemID_2, ItemID_3, ItemID_4, ItemID_5, ItemID_6,
                   ItemID_7, ItemID_8, ItemID_9, ItemID_10, ItemID_11, ItemID_12
            FROM CharStartOutfit
            WHERE RaceID = @race AND ClassID = @class_id AND GenderID = @gender
            LIMIT 1
            """;
        command.Parameters.AddWithValue("@race", race);
        command.Parameters.AddWithValue("@class_id", classId);
        command.Parameters.AddWithValue("@gender", gender);

        await using var reader = await command.ExecuteReaderAsync(cancellationToken).ConfigureAwait(false);
        if (!await reader.ReadAsync(cancellationToken).ConfigureAwait(false))
            return Array.Empty<uint>();

        var itemIds = new List<uint>(capacity: 12);
        for (var index = 0; index < 12; index++)
        {
            var itemId = Convert.ToInt32(reader.GetValue(index), CultureInfo.InvariantCulture);
            if (itemId > 0)
                itemIds.Add(checked((uint)itemId));
        }

        return itemIds;
    }
}

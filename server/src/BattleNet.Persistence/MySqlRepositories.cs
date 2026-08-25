using System.Globalization;
using System.Security.Cryptography;
using System.Text;
using BattleNet.Core.Configuration;
using BattleNet.Persistence.Models;
using BattleNet.Protocol;
using MySqlConnector;

namespace BattleNet.Persistence;

/// <summary>
/// Direct MySQL compatibility repositories for Alpha Core's existing auth schema.
/// The SQL stays deliberately close to the original Python queries so the C# port
/// can be introduced without changing the installed database.
/// </summary>
public sealed class MySqlAuthRepository(string connectionString, ServerOptions options) : IAuthRepository
{
    private static readonly SemaphoreSlim RegistrationGate = new(1, 1);

    public async ValueTask<AccountRecord?> FindAccountAsync(
        string username,
        CancellationToken cancellationToken = default)
    {
        await using var connection = await OpenConnectionAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT id, name, password, ip, gmlevel, salt, verifier, sessionkey
            FROM accounts
            WHERE name = @name
            LIMIT 1
            """;
        command.Parameters.AddWithValue("@name", username);

        await using var reader = await command.ExecuteReaderAsync(cancellationToken).ConfigureAwait(false);
        if (!await reader.ReadAsync(cancellationToken).ConfigureAwait(false))
            return null;

        return new AccountRecord(
            Convert.ToInt32(reader.GetValue(0), CultureInfo.InvariantCulture),
            ReadString(reader, 1),
            ReadString(reader, 2),
            ReadString(reader, 5),
            ReadString(reader, 6),
            ReadString(reader, 7),
            Convert.ToInt32(reader.GetValue(4), CultureInfo.InvariantCulture),
            ReadString(reader, 3));
    }

    public async ValueTask<AccountCreateResult> CreateAccountAsync(
        string username,
        string password,
        CancellationToken cancellationToken = default)
    {
        var normalizedUsername = AccountRegistration.NormalizeUsername(username);
        if (!AccountRegistration.IsValidUsername(normalizedUsername))
            return AccountCreateResult.InvalidUsername;
        if (!AccountRegistration.IsValidPassword(password))
            return AccountCreateResult.InvalidPassword;

        await RegistrationGate.WaitAsync(cancellationToken).ConfigureAwait(false);
        try
        {
            await using var connection = await OpenConnectionAsync(cancellationToken).ConfigureAwait(false);
            await using (var findCommand = connection.CreateCommand())
            {
                findCommand.CommandText = "SELECT 1 FROM accounts WHERE UPPER(name) = @name LIMIT 1";
                findCommand.Parameters.AddWithValue("@name", normalizedUsername);
                if (await findCommand.ExecuteScalarAsync(cancellationToken).ConfigureAwait(false) is not null)
                    return AccountCreateResult.AlreadyExists;
            }

            var account = AccountRegistration.CreateRecord(0, normalizedUsername, password);
            await using var insertCommand = connection.CreateCommand();
            insertCommand.CommandText = """
                INSERT INTO accounts (name, password, ip, gmlevel, salt, verifier, sessionkey)
                VALUES (@name, @password, '', 0, @salt, @verifier, '')
                """;
            insertCommand.Parameters.AddWithValue("@name", account.Name);
            insertCommand.Parameters.AddWithValue("@password", account.PasswordHash);
            insertCommand.Parameters.AddWithValue("@salt", account.SaltHex);
            insertCommand.Parameters.AddWithValue("@verifier", account.VerifierHex);
            await insertCommand.ExecuteNonQueryAsync(cancellationToken).ConfigureAwait(false);
            return AccountCreateResult.Success;
        }
        finally
        {
            RegistrationGate.Release();
        }
    }

    public async ValueTask<bool> UpdateSessionKeyAsync(
        string username,
        string sessionKeyHex,
        string ipAddress,
        CancellationToken cancellationToken = default)
    {
        await using var connection = await OpenConnectionAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            UPDATE accounts
            SET sessionkey = @sessionkey, ip = @ip
            WHERE name = @name
            """;
        command.Parameters.AddWithValue("@sessionkey", sessionKeyHex);
        command.Parameters.AddWithValue("@ip", ipAddress);
        command.Parameters.AddWithValue("@name", username);
        return await command.ExecuteNonQueryAsync(cancellationToken).ConfigureAwait(false) == 1;
    }

    public async ValueTask<IReadOnlyList<RealmInfo>> GetRealmsAsync(CancellationToken cancellationToken = default)
    {
        await using var connection = await OpenConnectionAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT realm_id, realm_name, proxy_address, proxy_port,
                   realm_address, realm_port, online_player_count
            FROM realmlist
            ORDER BY realm_id
            """;

        var realms = new List<RealmInfo>();
        await using var reader = await command.ExecuteReaderAsync(cancellationToken).ConfigureAwait(false);
        while (await reader.ReadAsync(cancellationToken).ConfigureAwait(false))
        {
            var address = ResolveAddress(ReadString(reader, 4), options.WorldAdvertisedAddress);
            var proxyAddress = ResolveAddress(ReadString(reader, 2), options.RealmAdvertisedAddress);
            realms.Add(new RealmInfo(
                Convert.ToInt32(reader.GetValue(0), CultureInfo.InvariantCulture),
                ReadString(reader, 1),
                address,
                Convert.ToInt32(reader.GetValue(5), CultureInfo.InvariantCulture),
                proxyAddress,
                Convert.ToInt32(reader.GetValue(3), CultureInfo.InvariantCulture),
                Convert.ToInt32(reader.GetValue(6), CultureInfo.InvariantCulture)));
        }

        return realms;
    }

    public async ValueTask SetOnlinePlayerCountAsync(
        int realmId,
        int count,
        CancellationToken cancellationToken = default)
    {
        await using var connection = await OpenConnectionAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            UPDATE realmlist
            SET online_player_count = @count
            WHERE realm_id = @realm_id
            """;
        command.Parameters.AddWithValue("@count", count);
        command.Parameters.AddWithValue("@realm_id", realmId);
        await command.ExecuteNonQueryAsync(cancellationToken).ConfigureAwait(false);
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

    private static string ReadString(MySqlDataReader reader, int ordinal) =>
        reader.IsDBNull(ordinal) ? string.Empty : Convert.ToString(reader.GetValue(ordinal), CultureInfo.InvariantCulture) ?? string.Empty;

    private static string ResolveAddress(string value, string fallback) =>
        string.IsNullOrWhiteSpace(value) || value is "0.0.0.0" or "::" ? fallback : value;
}

public sealed class MySqlRealmRepository(string connectionString, string? worldConnectionString = null) : IRealmRepository
{
    public async ValueTask<IReadOnlyList<CharacterSummary>> GetCharactersAsync(
        int accountId,
        int realmId,
        CancellationToken cancellationToken = default)
    {
        await using var connection = new MySqlConnection(connectionString);
        await connection.OpenAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT guid, name, race, `class`, gender, skin, face, hairstyle,
                   haircolour, facialhair, level, zone, map,
                   position_x, position_y, position_z, orientation,
                   xp, money, bankslots, talentpoints, skillpoints,
                   health, power1, power2, power3, power4
            FROM characters
            WHERE account = @account_id AND realm_id = @realm_id
            ORDER BY guid
            LIMIT 10
            """;
        command.Parameters.AddWithValue("@account_id", accountId);
        command.Parameters.AddWithValue("@realm_id", realmId);

        var characters = new List<CharacterSummary>();
        await using var reader = await command.ExecuteReaderAsync(cancellationToken).ConfigureAwait(false);
        while (await reader.ReadAsync(cancellationToken).ConfigureAwait(false))
        {
            characters.Add(new CharacterSummary(
                Convert.ToUInt64(reader.GetValue(0), CultureInfo.InvariantCulture),
                ReadString(reader, 1),
                Convert.ToByte(reader.GetValue(2), CultureInfo.InvariantCulture),
                Convert.ToByte(reader.GetValue(3), CultureInfo.InvariantCulture),
                Convert.ToByte(reader.GetValue(4), CultureInfo.InvariantCulture),
                Convert.ToByte(reader.GetValue(5), CultureInfo.InvariantCulture),
                Convert.ToByte(reader.GetValue(6), CultureInfo.InvariantCulture),
                Convert.ToByte(reader.GetValue(7), CultureInfo.InvariantCulture),
                Convert.ToByte(reader.GetValue(8), CultureInfo.InvariantCulture),
                Convert.ToByte(reader.GetValue(9), CultureInfo.InvariantCulture),
                Convert.ToByte(reader.GetValue(10), CultureInfo.InvariantCulture),
                Convert.ToUInt32(reader.GetValue(11), CultureInfo.InvariantCulture),
                Convert.ToUInt32(reader.GetValue(12), CultureInfo.InvariantCulture),
                Convert.ToSingle(reader.GetValue(13), CultureInfo.InvariantCulture),
                Convert.ToSingle(reader.GetValue(14), CultureInfo.InvariantCulture),
                Convert.ToSingle(reader.GetValue(15), CultureInfo.InvariantCulture),
                Orientation: Convert.ToSingle(reader.GetValue(16), CultureInfo.InvariantCulture),
                Experience: Convert.ToUInt32(reader.GetValue(17), CultureInfo.InvariantCulture),
                Money: Convert.ToUInt32(reader.GetValue(18), CultureInfo.InvariantCulture),
                BankSlots: Convert.ToByte(reader.GetValue(19), CultureInfo.InvariantCulture),
                TalentPoints: Convert.ToUInt32(reader.GetValue(20), CultureInfo.InvariantCulture),
                SkillPoints: Convert.ToUInt32(reader.GetValue(21), CultureInfo.InvariantCulture),
                Health: Convert.ToUInt32(reader.GetValue(22), CultureInfo.InvariantCulture),
                Power1: Convert.ToUInt32(reader.GetValue(23), CultureInfo.InvariantCulture),
                Power2: Convert.ToUInt32(reader.GetValue(24), CultureInfo.InvariantCulture),
                Power3: Convert.ToUInt32(reader.GetValue(25), CultureInfo.InvariantCulture),
                Power4: Convert.ToUInt32(reader.GetValue(26), CultureInfo.InvariantCulture)));
        }

        return characters;
    }

    public async ValueTask<CharacterSummary?> FindCharacterAsync(
        ulong guid,
        int realmId,
        CancellationToken cancellationToken = default)
    {
        await using var connection = new MySqlConnection(connectionString);
        await connection.OpenAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT guid, name, race, `class`, gender, skin, face, hairstyle,
                   haircolour, facialhair, level, zone, map,
                   position_x, position_y, position_z, orientation,
                   xp, money, bankslots, talentpoints, skillpoints,
                   health, power1, power2, power3, power4
            FROM characters
            WHERE guid = @guid AND realm_id = @realm_id
            LIMIT 1
            """;
        command.Parameters.AddWithValue("@guid", guid);
        command.Parameters.AddWithValue("@realm_id", realmId);

        await using var reader = await command.ExecuteReaderAsync(cancellationToken).ConfigureAwait(false);
        return await reader.ReadAsync(cancellationToken).ConfigureAwait(false)
            ? ReadCharacter(reader)
            : null;
    }

    public async ValueTask<IReadOnlyList<CharacterSpellRecord>> GetCharacterSpellsAsync(
        ulong guid,
        CancellationToken cancellationToken = default)
    {
        await using var connection = new MySqlConnection(connectionString);
        await connection.OpenAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT spell, active, disabled
            FROM character_spells
            WHERE guid = @guid
            ORDER BY spell
            """;
        command.Parameters.AddWithValue("@guid", guid);

        var spells = new List<CharacterSpellRecord>();
        await using var reader = await command.ExecuteReaderAsync(cancellationToken).ConfigureAwait(false);
        while (await reader.ReadAsync(cancellationToken).ConfigureAwait(false))
        {
            spells.Add(new CharacterSpellRecord(
                Convert.ToUInt32(reader.GetValue(0), CultureInfo.InvariantCulture),
                Convert.ToBoolean(reader.GetValue(1), CultureInfo.InvariantCulture),
                Convert.ToBoolean(reader.GetValue(2), CultureInfo.InvariantCulture)));
        }

        return spells;
    }

    public async ValueTask<IReadOnlyDictionary<int, int>> GetActionButtonsAsync(
        ulong guid,
        CancellationToken cancellationToken = default)
    {
        await using var connection = new MySqlConnection(connectionString);
        await connection.OpenAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT `index`, action
            FROM character_buttons
            WHERE owner = @guid
            ORDER BY `index`
            """;
        command.Parameters.AddWithValue("@guid", guid);

        var buttons = new Dictionary<int, int>();
        await using var reader = await command.ExecuteReaderAsync(cancellationToken).ConfigureAwait(false);
        while (await reader.ReadAsync(cancellationToken).ConfigureAwait(false))
        {
            buttons[Convert.ToInt32(reader.GetValue(0), CultureInfo.InvariantCulture)] =
                Convert.ToInt32(reader.GetValue(1), CultureInfo.InvariantCulture);
        }

        return buttons;
    }

    public async ValueTask<IReadOnlyDictionary<uint, int>> GetSpellButtonsAsync(
        ulong guid,
        CancellationToken cancellationToken = default)
    {
        await using var connection = new MySqlConnection(connectionString);
        await connection.OpenAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT spell, `index`
            FROM character_spell_book
            WHERE owner = @guid
            ORDER BY `index`, spell
            """;
        command.Parameters.AddWithValue("@guid", guid);

        var buttons = new Dictionary<uint, int>();
        await using var reader = await command.ExecuteReaderAsync(cancellationToken).ConfigureAwait(false);
        while (await reader.ReadAsync(cancellationToken).ConfigureAwait(false))
        {
            buttons[Convert.ToUInt32(reader.GetValue(0), CultureInfo.InvariantCulture)] =
                Convert.ToInt32(reader.GetValue(1), CultureInfo.InvariantCulture);
        }

        return buttons;
    }

    public async ValueTask<IReadOnlyList<CharacterInventoryRecord>> GetCharacterInventoryAsync(
        ulong guid,
        CancellationToken cancellationToken = default)
    {
        await using var connection = new MySqlConnection(connectionString);
        await connection.OpenAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT guid, owner, creator, bag, slot, item_template, stackcount,
                   SpellCharges1, SpellCharges2, SpellCharges3, SpellCharges4, SpellCharges5,
                   item_flags, duration, enchantments
            FROM character_inventory
            WHERE owner = @guid
            ORDER BY bag, slot, guid
            """;
        command.Parameters.AddWithValue("@guid", guid);

        var inventory = new List<CharacterInventoryRecord>();
        await using var reader = await command.ExecuteReaderAsync(cancellationToken).ConfigureAwait(false);
        while (await reader.ReadAsync(cancellationToken).ConfigureAwait(false))
        {
            inventory.Add(new CharacterInventoryRecord(
                Convert.ToUInt64(reader.GetValue(0), CultureInfo.InvariantCulture),
                Convert.ToUInt64(reader.GetValue(1), CultureInfo.InvariantCulture),
                Convert.ToUInt64(reader.GetValue(2), CultureInfo.InvariantCulture),
                Convert.ToUInt32(reader.GetValue(3), CultureInfo.InvariantCulture),
                Convert.ToByte(reader.GetValue(4), CultureInfo.InvariantCulture),
                Convert.ToUInt32(reader.GetValue(5), CultureInfo.InvariantCulture),
                Convert.ToUInt32(reader.GetValue(6), CultureInfo.InvariantCulture),
                Convert.ToInt32(reader.GetValue(7), CultureInfo.InvariantCulture),
                Convert.ToInt32(reader.GetValue(8), CultureInfo.InvariantCulture),
                Convert.ToInt32(reader.GetValue(9), CultureInfo.InvariantCulture),
                Convert.ToInt32(reader.GetValue(10), CultureInfo.InvariantCulture),
                Convert.ToInt32(reader.GetValue(11), CultureInfo.InvariantCulture),
                Convert.ToUInt32(reader.GetValue(12), CultureInfo.InvariantCulture),
                Convert.ToUInt32(reader.GetValue(13), CultureInfo.InvariantCulture),
                ReadString(reader, 14)));
        }

        return inventory;
    }

    public async ValueTask InitializeCharacterStartupStateAsync(
        ulong guid,
        CharacterStartupState state,
        CancellationToken cancellationToken = default)
    {
        await using var connection = new MySqlConnection(connectionString);
        await connection.OpenAsync(cancellationToken).ConfigureAwait(false);
        await using var transaction = await connection.BeginTransactionAsync(cancellationToken).ConfigureAwait(false);

        foreach (var spellId in state.SpellIds.Distinct())
        {
            await using var command = connection.CreateCommand();
            command.Transaction = transaction;
            command.CommandText = """
                INSERT IGNORE INTO character_spells (guid, spell, active, disabled)
                VALUES (@guid, @spell, 1, 0)
                """;
            command.Parameters.AddWithValue("@guid", guid);
            command.Parameters.AddWithValue("@spell", spellId);
            await command.ExecuteNonQueryAsync(cancellationToken).ConfigureAwait(false);
        }

        foreach (var (index, action) in state.ActionButtons)
        {
            await using var command = connection.CreateCommand();
            command.Transaction = transaction;
            command.CommandText = """
                INSERT IGNORE INTO character_buttons (owner, `index`, action)
                VALUES (@owner, @index, @action)
                """;
            command.Parameters.AddWithValue("@owner", guid);
            command.Parameters.AddWithValue("@index", index);
            command.Parameters.AddWithValue("@action", action);
            await command.ExecuteNonQueryAsync(cancellationToken).ConfigureAwait(false);
        }

        foreach (var item in state.Inventory)
        {
            await using var command = connection.CreateCommand();
            command.Transaction = transaction;
            command.CommandText = """
                INSERT INTO character_inventory (
                    owner, creator, bag, slot, item_template, stackcount,
                    SpellCharges1, SpellCharges2, SpellCharges3, SpellCharges4, SpellCharges5,
                    item_flags, duration, enchantments)
                VALUES (
                    @owner, @creator, @bag, @slot, @item_template, @stackcount,
                    @spell_charges1, @spell_charges2, @spell_charges3, @spell_charges4, @spell_charges5,
                    @item_flags, @duration, @enchantments)
                """;
            command.Parameters.AddWithValue("@owner", guid);
            command.Parameters.AddWithValue("@creator", item.Creator);
            command.Parameters.AddWithValue("@bag", item.Bag);
            command.Parameters.AddWithValue("@slot", item.Slot);
            command.Parameters.AddWithValue("@item_template", item.ItemTemplate);
            command.Parameters.AddWithValue("@stackcount", item.StackCount);
            command.Parameters.AddWithValue("@spell_charges1", item.SpellCharges1);
            command.Parameters.AddWithValue("@spell_charges2", item.SpellCharges2);
            command.Parameters.AddWithValue("@spell_charges3", item.SpellCharges3);
            command.Parameters.AddWithValue("@spell_charges4", item.SpellCharges4);
            command.Parameters.AddWithValue("@spell_charges5", item.SpellCharges5);
            command.Parameters.AddWithValue("@item_flags", item.ItemFlags);
            command.Parameters.AddWithValue("@duration", item.Duration);
            command.Parameters.AddWithValue("@enchantments", item.Enchantments);
            await command.ExecuteNonQueryAsync(cancellationToken).ConfigureAwait(false);
        }

        await transaction.CommitAsync(cancellationToken).ConfigureAwait(false);
    }

    public async ValueTask<StartingLocation?> GetStartingLocationAsync(
        byte race,
        byte classId,
        CancellationToken cancellationToken = default)
    {
        if (string.IsNullOrWhiteSpace(worldConnectionString))
            return StartingLocation.Default;

        await using var connection = new MySqlConnection(worldConnectionString);
        await connection.OpenAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            SELECT map, zone, position_x, position_y, position_z, orientation
            FROM playercreateinfo
            WHERE race = @race AND `class` = @class_id
            ORDER BY id
            LIMIT 1
            """;
        command.Parameters.AddWithValue("@race", race);
        command.Parameters.AddWithValue("@class_id", classId);

        await using var reader = await command.ExecuteReaderAsync(cancellationToken).ConfigureAwait(false);
        if (!await reader.ReadAsync(cancellationToken).ConfigureAwait(false))
            return null;

        return new StartingLocation(
            Convert.ToUInt32(reader.GetValue(0), CultureInfo.InvariantCulture),
            Convert.ToUInt32(reader.GetValue(1), CultureInfo.InvariantCulture),
            Convert.ToSingle(reader.GetValue(2), CultureInfo.InvariantCulture),
            Convert.ToSingle(reader.GetValue(3), CultureInfo.InvariantCulture),
            Convert.ToSingle(reader.GetValue(4), CultureInfo.InvariantCulture),
            Convert.ToSingle(reader.GetValue(5), CultureInfo.InvariantCulture));
    }

    public async ValueTask<CharacterSummary?> CreateCharacterAsync(
        CharacterCreateRequest request,
        CancellationToken cancellationToken = default)
    {
        await using var connection = new MySqlConnection(connectionString);
        await connection.OpenAsync(cancellationToken).ConfigureAwait(false);

        await using (var countCommand = connection.CreateCommand())
        {
            countCommand.CommandText = """
                SELECT COUNT(*)
                FROM characters
                WHERE account = @account_id AND realm_id = @realm_id
                """;
            countCommand.Parameters.AddWithValue("@account_id", request.AccountId);
            countCommand.Parameters.AddWithValue("@realm_id", request.RealmId);
            if (Convert.ToInt32(await countCommand.ExecuteScalarAsync(cancellationToken).ConfigureAwait(false), CultureInfo.InvariantCulture) >= 10)
                return null;
        }

        await using (var nameCommand = connection.CreateCommand())
        {
            nameCommand.CommandText = """
                SELECT 1
                FROM characters
                WHERE realm_id = @realm_id AND LOWER(name) = LOWER(@name)
                LIMIT 1
                """;
            nameCommand.Parameters.AddWithValue("@realm_id", request.RealmId);
            nameCommand.Parameters.AddWithValue("@name", request.Name);
            if (await nameCommand.ExecuteScalarAsync(cancellationToken).ConfigureAwait(false) is not null)
                return null;
        }

        var location = request.StartingLocation;
        await using var insertCommand = connection.CreateCommand();
        insertCommand.CommandText = """
            INSERT INTO characters (
                account, realm_id, name, race, `class`, gender, level,
                skin, face, hairstyle, haircolour, facialhair,
                position_x, position_y, position_z, orientation, map, zone,
                online, health, power1, power2, power3, power4, power5)
            VALUES (
                @account_id, @realm_id, @name, @race, @class_id, @gender, @level,
                @skin, @face, @hairstyle, @haircolour, @facialhair,
                @position_x, @position_y, @position_z, @orientation, @map, @zone,
                0, 1, 0, 0, 0, 0, 0)
            """;
        insertCommand.Parameters.AddWithValue("@account_id", request.AccountId);
        insertCommand.Parameters.AddWithValue("@realm_id", request.RealmId);
        insertCommand.Parameters.AddWithValue("@name", request.Name);
        insertCommand.Parameters.AddWithValue("@race", request.Race);
        insertCommand.Parameters.AddWithValue("@class_id", request.Class);
        insertCommand.Parameters.AddWithValue("@gender", request.Gender);
        insertCommand.Parameters.AddWithValue("@level", request.Level);
        insertCommand.Parameters.AddWithValue("@skin", request.Skin);
        insertCommand.Parameters.AddWithValue("@face", request.Face);
        insertCommand.Parameters.AddWithValue("@hairstyle", request.HairStyle);
        insertCommand.Parameters.AddWithValue("@haircolour", request.HairColor);
        insertCommand.Parameters.AddWithValue("@facialhair", request.FacialHair);
        insertCommand.Parameters.AddWithValue("@position_x", location.PositionX);
        insertCommand.Parameters.AddWithValue("@position_y", location.PositionY);
        insertCommand.Parameters.AddWithValue("@position_z", location.PositionZ);
        insertCommand.Parameters.AddWithValue("@orientation", location.Orientation);
        insertCommand.Parameters.AddWithValue("@map", location.Map);
        insertCommand.Parameters.AddWithValue("@zone", location.Zone);
        await insertCommand.ExecuteNonQueryAsync(cancellationToken).ConfigureAwait(false);

        await using var idCommand = connection.CreateCommand();
        idCommand.CommandText = "SELECT LAST_INSERT_ID()";
        var guid = Convert.ToUInt64(await idCommand.ExecuteScalarAsync(cancellationToken).ConfigureAwait(false), CultureInfo.InvariantCulture);
        return new CharacterSummary(
            guid,
            request.Name,
            request.Race,
            request.Class,
            request.Gender,
            request.Skin,
            request.Face,
            request.HairStyle,
            request.HairColor,
            request.FacialHair,
            request.Level,
            location.Zone,
            location.Map,
            location.PositionX,
            location.PositionY,
            location.PositionZ,
            Orientation: location.Orientation);
    }

    public async ValueTask<bool> DeleteCharacterAsync(
        int accountId,
        int realmId,
        ulong guid,
        CancellationToken cancellationToken = default)
    {
        await using var connection = new MySqlConnection(connectionString);
        await connection.OpenAsync(cancellationToken).ConfigureAwait(false);
        await using var command = connection.CreateCommand();
        command.CommandText = """
            DELETE FROM characters
            WHERE guid = @guid
              AND account = @account_id
              AND realm_id = @realm_id
              AND NOT EXISTS (
                  SELECT 1 FROM guild_member
                  WHERE guild_member.guid = characters.guid AND guild_member.rank = 0)
            """;
        command.Parameters.AddWithValue("@guid", guid);
        command.Parameters.AddWithValue("@account_id", accountId);
        command.Parameters.AddWithValue("@realm_id", realmId);
        return await command.ExecuteNonQueryAsync(cancellationToken).ConfigureAwait(false) == 1;
    }

    private static string ReadString(MySqlDataReader reader, int ordinal) =>
        reader.IsDBNull(ordinal) ? string.Empty : Convert.ToString(reader.GetValue(ordinal), CultureInfo.InvariantCulture) ?? string.Empty;

    private static CharacterSummary ReadCharacter(MySqlDataReader reader) => new(
        Convert.ToUInt64(reader.GetValue(0), CultureInfo.InvariantCulture),
        ReadString(reader, 1),
        Convert.ToByte(reader.GetValue(2), CultureInfo.InvariantCulture),
        Convert.ToByte(reader.GetValue(3), CultureInfo.InvariantCulture),
        Convert.ToByte(reader.GetValue(4), CultureInfo.InvariantCulture),
        Convert.ToByte(reader.GetValue(5), CultureInfo.InvariantCulture),
        Convert.ToByte(reader.GetValue(6), CultureInfo.InvariantCulture),
        Convert.ToByte(reader.GetValue(7), CultureInfo.InvariantCulture),
        Convert.ToByte(reader.GetValue(8), CultureInfo.InvariantCulture),
        Convert.ToByte(reader.GetValue(9), CultureInfo.InvariantCulture),
        Convert.ToByte(reader.GetValue(10), CultureInfo.InvariantCulture),
        Convert.ToUInt32(reader.GetValue(11), CultureInfo.InvariantCulture),
        Convert.ToUInt32(reader.GetValue(12), CultureInfo.InvariantCulture),
        Convert.ToSingle(reader.GetValue(13), CultureInfo.InvariantCulture),
        Convert.ToSingle(reader.GetValue(14), CultureInfo.InvariantCulture),
        Convert.ToSingle(reader.GetValue(15), CultureInfo.InvariantCulture),
        Orientation: Convert.ToSingle(reader.GetValue(16), CultureInfo.InvariantCulture),
        Experience: Convert.ToUInt32(reader.GetValue(17), CultureInfo.InvariantCulture),
        Money: Convert.ToUInt32(reader.GetValue(18), CultureInfo.InvariantCulture),
        BankSlots: Convert.ToByte(reader.GetValue(19), CultureInfo.InvariantCulture),
        TalentPoints: Convert.ToUInt32(reader.GetValue(20), CultureInfo.InvariantCulture),
        SkillPoints: Convert.ToUInt32(reader.GetValue(21), CultureInfo.InvariantCulture),
        Health: Convert.ToUInt32(reader.GetValue(22), CultureInfo.InvariantCulture),
        Power1: Convert.ToUInt32(reader.GetValue(23), CultureInfo.InvariantCulture),
        Power2: Convert.ToUInt32(reader.GetValue(24), CultureInfo.InvariantCulture),
        Power3: Convert.ToUInt32(reader.GetValue(25), CultureInfo.InvariantCulture),
        Power4: Convert.ToUInt32(reader.GetValue(26), CultureInfo.InvariantCulture));
}

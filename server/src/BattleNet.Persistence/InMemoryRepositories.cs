using System.Collections.Concurrent;
using System.Security.Cryptography;
using System.Text;
using BattleNet.Core.Configuration;
using BattleNet.Persistence.Models;
using BattleNet.Protocol;

namespace BattleNet.Persistence;

public sealed class InMemoryAuthRepository : IAuthRepository
{
    private readonly ConcurrentDictionary<string, AccountRecord> _accounts = new(StringComparer.OrdinalIgnoreCase);
    private readonly ConcurrentDictionary<int, int> _onlineCounts = new();
    private readonly ServerOptions _options;
    private int _nextAccountId;

    public InMemoryAuthRepository(ServerOptions options)
    {
        _options = options;

        var username = Environment.GetEnvironmentVariable("BATTLE_NET_TEST_USERNAME");
        var password = Environment.GetEnvironmentVariable("BATTLE_NET_TEST_PASSWORD");
        if (!string.IsNullOrWhiteSpace(username) && password is not null)
            AddPasswordAccount(username, password);
    }

    public void AddPasswordAccount(string username, string password, int id = 1, int securityLevel = 0)
    {
        var account = AccountRegistration.CreateRecord(id, username, password) with
        {
            SecurityLevel = securityLevel
        };
        _accounts[account.Name] = account;
        while (true)
        {
            var current = Volatile.Read(ref _nextAccountId);
            if (current >= id || Interlocked.CompareExchange(ref _nextAccountId, id, current) == current)
                break;
        }
    }

    public ValueTask<AccountRecord?> FindAccountAsync(string username, CancellationToken cancellationToken = default) =>
        ValueTask.FromResult(_accounts.TryGetValue(username, out var account) ? account : null);

    public ValueTask<AccountCreateResult> CreateAccountAsync(
        string username,
        string password,
        CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();
        var normalizedUsername = AccountRegistration.NormalizeUsername(username);
        if (!AccountRegistration.IsValidUsername(normalizedUsername))
            return ValueTask.FromResult(AccountCreateResult.InvalidUsername);
        if (!AccountRegistration.IsValidPassword(password))
            return ValueTask.FromResult(AccountCreateResult.InvalidPassword);

        var account = AccountRegistration.CreateRecord(
            Interlocked.Increment(ref _nextAccountId), normalizedUsername, password);
        return ValueTask.FromResult(
            _accounts.TryAdd(normalizedUsername, account)
                ? AccountCreateResult.Success
                : AccountCreateResult.AlreadyExists);
    }

    public ValueTask<bool> UpdateSessionKeyAsync(string username, string sessionKeyHex, string ipAddress, CancellationToken cancellationToken = default)
    {
        if (!_accounts.TryGetValue(username, out var account))
            return ValueTask.FromResult(false);
        _accounts[username] = account.WithSessionKey(sessionKeyHex, ipAddress);
        return ValueTask.FromResult(true);
    }

    public ValueTask<IReadOnlyList<RealmInfo>> GetRealmsAsync(CancellationToken cancellationToken = default)
    {
        var online = _onlineCounts.GetValueOrDefault(_options.LocalRealmId);
        IReadOnlyList<RealmInfo> realms =
        [
            new RealmInfo(
                _options.LocalRealmId,
                _options.RealmName,
                _options.WorldAdvertisedAddress,
                _options.WorldPort,
                _options.RealmAdvertisedAddress,
                _options.ProxyPort,
                online)
        ];
        return ValueTask.FromResult(realms);
    }

    public ValueTask SetOnlinePlayerCountAsync(int realmId, int count, CancellationToken cancellationToken = default)
    {
        _onlineCounts[realmId] = count;
        return ValueTask.CompletedTask;
    }
}

public sealed class InMemoryRealmRepository : IRealmRepository
{
    private readonly object _mutationLock = new();
    private readonly ConcurrentDictionary<(int AccountId, int RealmId), ConcurrentDictionary<ulong, CharacterSummary>> _characters = new();
    private readonly ConcurrentDictionary<ulong, CharacterStartupState> _startupStates = new();
    private ulong _nextGuid = 1;
    private ulong _nextItemGuid = 1_000_000;

    public ValueTask<IReadOnlyList<CharacterSummary>> GetCharactersAsync(
        int accountId,
        int realmId,
        CancellationToken cancellationToken = default)
    {
        var characters = _characters.TryGetValue((accountId, realmId), out var accountCharacters)
            ? accountCharacters.Values.OrderBy(character => character.Guid).Take(10).ToArray()
            : Array.Empty<CharacterSummary>();
        IReadOnlyList<CharacterSummary> result = characters;
        return ValueTask.FromResult(result);
    }

    public ValueTask<StartingLocation?> GetStartingLocationAsync(
        byte race,
        byte classId,
        CancellationToken cancellationToken = default) =>
        ValueTask.FromResult<StartingLocation?>(StartingLocation.Default);

    public ValueTask<CharacterSummary?> FindCharacterAsync(
        ulong guid,
        int realmId,
        CancellationToken cancellationToken = default)
    {
        var character = _characters
            .Where(pair => pair.Key.RealmId == realmId)
            .SelectMany(pair => pair.Value.Values)
            .FirstOrDefault(candidate => candidate.Guid == guid);
        return ValueTask.FromResult(character);
    }

    public ValueTask<IReadOnlyList<CharacterSpellRecord>> GetCharacterSpellsAsync(
        ulong guid,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyList<CharacterSpellRecord> result = _startupStates.TryGetValue(guid, out var state)
            ? state.SpellIds.Select(spell => new CharacterSpellRecord(spell, Active: true, Disabled: false)).ToArray()
            : Array.Empty<CharacterSpellRecord>();
        return ValueTask.FromResult(result);
    }

    public ValueTask<IReadOnlyDictionary<int, int>> GetActionButtonsAsync(
        ulong guid,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyDictionary<int, int> result = _startupStates.TryGetValue(guid, out var state)
            ? state.ActionButtons
            : new Dictionary<int, int>();
        return ValueTask.FromResult(result);
    }

    public ValueTask<IReadOnlyDictionary<uint, int>> GetSpellButtonsAsync(
        ulong guid,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyDictionary<uint, int> result = new Dictionary<uint, int>();
        return ValueTask.FromResult(result);
    }

    public ValueTask<IReadOnlyList<CharacterInventoryRecord>> GetCharacterInventoryAsync(
        ulong guid,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyList<CharacterInventoryRecord> result = _startupStates.TryGetValue(guid, out var state)
            ? state.Inventory
            : Array.Empty<CharacterInventoryRecord>();
        return ValueTask.FromResult(result);
    }

    public ValueTask InitializeCharacterStartupStateAsync(
        ulong guid,
        CharacterStartupState state,
        CancellationToken cancellationToken = default)
    {
        lock (_mutationLock)
        {
            var inventory = state.Inventory
                .Select(item => item with
                {
                    Guid = item.Guid == 0 ? _nextItemGuid++ : item.Guid,
                    Owner = guid
                })
                .ToArray();
            _startupStates[guid] = new CharacterStartupState(
                state.SpellIds.Distinct().ToArray(),
                new Dictionary<int, int>(state.ActionButtons),
                inventory);
        }

        return ValueTask.CompletedTask;
    }

    public ValueTask<CharacterSummary?> CreateCharacterAsync(
        CharacterCreateRequest request,
        CancellationToken cancellationToken = default)
    {
        lock (_mutationLock)
        {
            var accountCharacters = _characters.GetOrAdd(
                (request.AccountId, request.RealmId),
                static _ => new ConcurrentDictionary<ulong, CharacterSummary>());

            if (accountCharacters.Count >= 10 || accountCharacters.Values.Any(character =>
                    string.Equals(character.Name, request.Name, StringComparison.OrdinalIgnoreCase)))
                return ValueTask.FromResult<CharacterSummary?>(null);

            var guid = _nextGuid++;
            var location = request.StartingLocation;
            var character = new CharacterSummary(
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
            accountCharacters[guid] = character;
            return ValueTask.FromResult<CharacterSummary?>(character);
        }
    }

    public ValueTask<bool> DeleteCharacterAsync(
        int accountId,
        int realmId,
        ulong guid,
        CancellationToken cancellationToken = default)
    {
        var removed = _characters.TryGetValue((accountId, realmId), out var accountCharacters)
                      && accountCharacters.TryRemove(guid, out _);
        return ValueTask.FromResult(removed);
    }
}

public sealed class InMemoryQuestRepository : IQuestRepository
{
    private readonly object _gate = new();
    private readonly Dictionary<ulong, List<CharacterQuestRecord>> _quests = new();

    public ValueTask<IReadOnlyList<CharacterQuestRecord>> GetCharacterQuestsAsync(
        ulong characterGuid,
        CancellationToken cancellationToken = default)
    {
        lock (_gate)
        {
            IReadOnlyList<CharacterQuestRecord> result = _quests.TryGetValue(characterGuid, out var rows)
                ? rows.ToArray()
                : Array.Empty<CharacterQuestRecord>();
            return ValueTask.FromResult(result);
        }
    }

    public ValueTask<bool> AcceptCharacterQuestAsync(
        CharacterQuestRecord quest,
        CancellationToken cancellationToken = default)
    {
        lock (_gate)
        {
            var rows = _quests.TryGetValue(quest.CharacterGuid, out var existing)
                ? existing
                : _quests[quest.CharacterGuid] = [];
            if (rows.Any(row => row.QuestId == quest.QuestId))
                return ValueTask.FromResult(false);
            rows.Add(quest);
            return ValueTask.FromResult(true);
        }
    }

    public ValueTask<bool> RemoveCharacterQuestAsync(
        ulong characterGuid,
        uint questId,
        CancellationToken cancellationToken = default)
    {
        lock (_gate)
        {
            var removed = _quests.TryGetValue(characterGuid, out var rows)
                && rows.RemoveAll(row => row.QuestId == questId) > 0;
            return ValueTask.FromResult(removed);
        }
    }

    public ValueTask<bool> UpdateCharacterQuestAsync(
        CharacterQuestRecord quest,
        CancellationToken cancellationToken = default)
    {
        lock (_gate)
        {
            if (!_quests.TryGetValue(quest.CharacterGuid, out var rows))
                return ValueTask.FromResult(false);
            var index = rows.FindIndex(row => row.QuestId == quest.QuestId);
            if (index < 0)
                return ValueTask.FromResult(false);
            rows[index] = quest;
            return ValueTask.FromResult(true);
        }
    }
}

/// <summary>
/// Fallback mutation store used only when an explicit MySQL realm connection
/// is selected. The standalone executable uses FileRepositories instead.
/// </summary>
public sealed class InMemoryCharacterMutationRepository : ICharacterMutationRepository
{
    public ValueTask<CharacterInventoryMutationResult?> ApplyInventoryMutationAsync(
        ulong characterGuid,
        IReadOnlyList<InventoryItemDelta> additions,
        IReadOnlyList<InventoryItemDelta> removals,
        long moneyDelta = 0,
        CancellationToken cancellationToken = default) =>
        ValueTask.FromResult<CharacterInventoryMutationResult?>(null);
}

public sealed class InMemoryCharacterStateRepository : ICharacterStateRepository
{
    public ValueTask<bool> SaveCharacterStateAsync(
        CharacterSummary character,
        CancellationToken cancellationToken = default) =>
        ValueTask.FromResult(false);
}

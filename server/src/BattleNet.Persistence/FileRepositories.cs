using System.Security.Cryptography;
using System.Text;
using System.Text.Json;
using BattleNet.Core.Configuration;
using BattleNet.Persistence.Models;
using BattleNet.Protocol;

namespace BattleNet.Persistence;

/// <summary>
/// Self-contained table storage for the standalone BattleNet executable.
/// Each Alpha-compatible table is a JSON file under the configured tables
/// directory; no database service or native database installation is needed.
/// MySQL repositories remain available when an explicit connection string is
/// configured.
/// </summary>
public sealed class FileRepositories : IAuthRepository, IRealmRepository, IQuestRepository, ICharacterMutationRepository, ICharacterStateRepository, IWorldRepository, IDbcRepository
{
    private const string AccountsFile = "accounts.json";
    private const string RealmsFile = "realms.json";
    private const string CharactersFile = "characters.json";
    private const string CharacterSpellsFile = "character_spells.json";
    private const string CharacterButtonsFile = "character_buttons.json";
    private const string CharacterSpellBookFile = "character_spell_book.json";
    private const string CharacterInventoryFile = "character_inventory.json";
    private const string CharacterQuestsFile = "character_quests.json";
    private const string StartingLocationsFile = "playercreateinfo.json";
    private const string StartingSpellsFile = "playercreateinfo_spell.json";
    private const string StartingActionsFile = "playercreateinfo_action.json";
    private const string StartingItemsFile = "playercreateinfo_item.json";
    private const string RacesFile = "chr_races.json";
    private const string OutfitsFile = "char_start_outfit.json";
    private const string ItemTemplatesFile = "item_template.json";

    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
        WriteIndented = true
    };

    private readonly object _gate = new();
    private readonly string _directory;
    private readonly ServerOptions _options;
    private Dictionary<uint, ItemTemplateRecord>? _itemTemplateCache;

    public FileRepositories(ServerOptions options)
    {
        _options = options;
        _directory = options.DataDirectory;
        Directory.CreateDirectory(_directory);
        InitializeTables();
    }

    public ValueTask<AccountRecord?> FindAccountAsync(string username, CancellationToken cancellationToken = default)
    {
        var account = Read(AccountsFile, static () => new List<AccountRecord>())
            .FirstOrDefault(candidate => string.Equals(candidate.Name, username, StringComparison.OrdinalIgnoreCase));
        return ValueTask.FromResult(account);
    }

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

        lock (_gate)
        {
            var accounts = ReadLocked(AccountsFile, static () => new List<AccountRecord>());
            if (accounts.Any(account => string.Equals(
                    account.Name, normalizedUsername, StringComparison.OrdinalIgnoreCase)))
                return ValueTask.FromResult(AccountCreateResult.AlreadyExists);

            var nextId = accounts.Count == 0 ? 1 : accounts.Max(static account => account.Id) + 1;
            accounts.Add(AccountRegistration.CreateRecord(nextId, normalizedUsername, password));
            WriteLocked(AccountsFile, accounts);
            return ValueTask.FromResult(AccountCreateResult.Success);
        }
    }

    public ValueTask<bool> UpdateSessionKeyAsync(
        string username,
        string sessionKeyHex,
        string ipAddress,
        CancellationToken cancellationToken = default)
    {
        lock (_gate)
        {
            var accounts = ReadLocked(AccountsFile, static () => new List<AccountRecord>());
            var index = accounts.FindIndex(account => string.Equals(account.Name, username, StringComparison.OrdinalIgnoreCase));
            if (index < 0)
                return ValueTask.FromResult(false);

            accounts[index] = accounts[index].WithSessionKey(sessionKeyHex, ipAddress);
            WriteLocked(AccountsFile, accounts);
            return ValueTask.FromResult(true);
        }
    }

    public ValueTask<IReadOnlyList<RealmInfo>> GetRealmsAsync(CancellationToken cancellationToken = default)
    {
        IReadOnlyList<RealmInfo> realms = Read<List<RealmInfo>>(RealmsFile, () =>
        [
            new RealmInfo(
                _options.LocalRealmId,
                _options.RealmName,
                _options.WorldAdvertisedAddress,
                _options.WorldPort,
                _options.RealmAdvertisedAddress,
                _options.ProxyPort,
                0)
        ]);
        return ValueTask.FromResult(realms);
    }

    public ValueTask SetOnlinePlayerCountAsync(
        int realmId,
        int count,
        CancellationToken cancellationToken = default)
    {
        lock (_gate)
        {
            var realms = ReadLocked(RealmsFile, () => new List<RealmInfo>
            {
                new(
                    _options.LocalRealmId,
                    _options.RealmName,
                    _options.WorldAdvertisedAddress,
                    _options.WorldPort,
                    _options.RealmAdvertisedAddress,
                    _options.ProxyPort,
                    0)
            });
            var index = realms.FindIndex(realm => realm.Id == realmId);
            if (index < 0)
            {
                realms.Add(new RealmInfo(
                    realmId,
                    _options.RealmName,
                    _options.WorldAdvertisedAddress,
                    _options.WorldPort,
                    _options.RealmAdvertisedAddress,
                    _options.ProxyPort,
                    count));
            }
            else
            {
                realms[index] = realms[index] with { OnlinePlayers = count };
            }

            WriteLocked(RealmsFile, realms);
            return ValueTask.CompletedTask;
        }
    }

    public ValueTask<IReadOnlyList<CharacterSummary>> GetCharactersAsync(
        int accountId,
        int realmId,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyList<CharacterSummary> characters = Read(CharactersFile, static () => new List<CharacterTableRow>())
            .Where(row => row.AccountId == accountId && row.RealmId == realmId)
            .OrderBy(row => row.Character.Guid)
            .Select(row => row.Character)
            .Take(10)
            .ToArray();
        return ValueTask.FromResult(characters);
    }

    public ValueTask<CharacterSummary?> FindCharacterAsync(
        ulong guid,
        int realmId,
        CancellationToken cancellationToken = default)
    {
        var character = Read(CharactersFile, static () => new List<CharacterTableRow>())
            .FirstOrDefault(row => row.RealmId == realmId && row.Character.Guid == guid)
            ?.Character;
        return ValueTask.FromResult(character);
    }

    public ValueTask<bool> SaveCharacterStateAsync(
        CharacterSummary character,
        CancellationToken cancellationToken = default)
    {
        lock (_gate)
        {
            var characters = ReadLocked(CharactersFile, static () => new List<CharacterTableRow>());
            var row = characters.FirstOrDefault(candidate => candidate.Character.Guid == character.Guid);
            if (row is null)
                return ValueTask.FromResult(false);

            row.Character = character;
            WriteLocked(CharactersFile, characters);
            return ValueTask.FromResult(true);
        }
    }

    public ValueTask<IReadOnlyList<CharacterSpellRecord>> GetCharacterSpellsAsync(
        ulong guid,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyList<CharacterSpellRecord> spells = Read(CharacterSpellsFile, static () => new List<CharacterSpellTableRow>())
            .Where(row => row.Guid == guid)
            .OrderBy(row => row.SpellId)
            .Select(row => new CharacterSpellRecord(row.SpellId, row.Active, row.Disabled))
            .ToArray();
        return ValueTask.FromResult(spells);
    }

    public ValueTask<IReadOnlyDictionary<int, int>> GetActionButtonsAsync(
        ulong guid,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyDictionary<int, int> buttons = Read(CharacterButtonsFile, static () => new List<CharacterButtonTableRow>())
            .Where(row => row.Owner == guid)
            .ToDictionary(row => row.Index, row => row.Action);
        return ValueTask.FromResult(buttons);
    }

    public ValueTask<IReadOnlyDictionary<uint, int>> GetSpellButtonsAsync(
        ulong guid,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyDictionary<uint, int> buttons = Read(CharacterSpellBookFile, static () => new List<CharacterSpellBookTableRow>())
            .Where(row => row.Owner == guid)
            .ToDictionary(row => row.Spell, row => row.Index);
        return ValueTask.FromResult(buttons);
    }

    public ValueTask<IReadOnlyList<CharacterInventoryRecord>> GetCharacterInventoryAsync(
        ulong guid,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyList<CharacterInventoryRecord> inventory = Read(CharacterInventoryFile, static () => new List<CharacterInventoryTableRow>())
            .Where(row => row.Owner == guid)
            .OrderBy(row => row.Bag)
            .ThenBy(row => row.Slot)
            .ThenBy(row => row.Guid)
            .Select(row => row.ToRecord())
            .ToArray();
        return ValueTask.FromResult(inventory);
    }

    public ValueTask InitializeCharacterStartupStateAsync(
        ulong guid,
        CharacterStartupState state,
        CancellationToken cancellationToken = default)
    {
        lock (_gate)
        {
            var spells = ReadLocked(CharacterSpellsFile, static () => new List<CharacterSpellTableRow>());
            spells.RemoveAll(row => row.Guid == guid);
            spells.AddRange(state.SpellIds.Distinct().Select(spell => new CharacterSpellTableRow
            {
                Guid = guid,
                SpellId = spell,
                Active = true,
                Disabled = false
            }));
            WriteLocked(CharacterSpellsFile, spells);

            var buttons = ReadLocked(CharacterButtonsFile, static () => new List<CharacterButtonTableRow>());
            buttons.RemoveAll(row => row.Owner == guid);
            buttons.AddRange(state.ActionButtons.Select(button => new CharacterButtonTableRow
            {
                Owner = guid,
                Index = button.Key,
                Action = button.Value
            }));
            WriteLocked(CharacterButtonsFile, buttons);

            var inventory = ReadLocked(CharacterInventoryFile, static () => new List<CharacterInventoryTableRow>());
            inventory.RemoveAll(row => row.Owner == guid);
            var nextItemGuid = inventory.Count == 0 ? 1UL : inventory.Max(row => row.Guid) + 1;
            inventory.AddRange(state.Inventory.Select(item => new CharacterInventoryTableRow
            {
                Guid = item.Guid == 0 ? nextItemGuid++ : item.Guid,
                Owner = guid,
                Creator = item.Creator,
                Bag = item.Bag,
                Slot = item.Slot,
                ItemTemplate = item.ItemTemplate,
                StackCount = item.StackCount,
                SpellCharges1 = item.SpellCharges1,
                SpellCharges2 = item.SpellCharges2,
                SpellCharges3 = item.SpellCharges3,
                SpellCharges4 = item.SpellCharges4,
                SpellCharges5 = item.SpellCharges5,
                ItemFlags = item.ItemFlags,
                Duration = item.Duration,
                Enchantments = item.Enchantments
            }));
            WriteLocked(CharacterInventoryFile, inventory);
            return ValueTask.CompletedTask;
        }
    }

    public ValueTask<IReadOnlyList<CharacterQuestRecord>> GetCharacterQuestsAsync(
        ulong characterGuid,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyList<CharacterQuestRecord> quests = Read(CharacterQuestsFile, static () => new List<CharacterQuestTableRow>())
            .Where(row => row.CharacterGuid == characterGuid)
            .OrderBy(row => row.QuestId)
            .Select(row => new CharacterQuestRecord(row.CharacterGuid, row.QuestId, row.State, row.Rewarded, row.ObjectiveCounts))
            .ToArray();
        return ValueTask.FromResult(quests);
    }

    public ValueTask<bool> AcceptCharacterQuestAsync(
        CharacterQuestRecord quest,
        CancellationToken cancellationToken = default)
    {
        lock (_gate)
        {
            var quests = ReadLocked(CharacterQuestsFile, static () => new List<CharacterQuestTableRow>());
            if (quests.Any(row => row.CharacterGuid == quest.CharacterGuid && row.QuestId == quest.QuestId))
                return ValueTask.FromResult(false);

            quests.Add(new CharacterQuestTableRow
            {
                CharacterGuid = quest.CharacterGuid,
                QuestId = quest.QuestId,
                State = quest.State,
                Rewarded = quest.Rewarded,
                ObjectiveCounts = quest.Progress.ToList()
            });
            WriteLocked(CharacterQuestsFile, quests);
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
            var quests = ReadLocked(CharacterQuestsFile, static () => new List<CharacterQuestTableRow>());
            var removed = quests.RemoveAll(row => row.CharacterGuid == characterGuid && row.QuestId == questId) > 0;
            if (removed)
                WriteLocked(CharacterQuestsFile, quests);
            return ValueTask.FromResult(removed);
        }
    }

    public ValueTask<bool> UpdateCharacterQuestAsync(
        CharacterQuestRecord quest,
        CancellationToken cancellationToken = default)
    {
        lock (_gate)
        {
            var quests = ReadLocked(CharacterQuestsFile, static () => new List<CharacterQuestTableRow>());
            var row = quests.FirstOrDefault(candidate =>
                candidate.CharacterGuid == quest.CharacterGuid && candidate.QuestId == quest.QuestId);
            if (row is null)
                return ValueTask.FromResult(false);

            row.State = quest.State;
            row.Rewarded = quest.Rewarded;
            row.ObjectiveCounts = quest.Progress.ToList();
            WriteLocked(CharacterQuestsFile, quests);
            return ValueTask.FromResult(true);
        }
    }

    public ValueTask<CharacterInventoryMutationResult?> ApplyInventoryMutationAsync(
        ulong characterGuid,
        IReadOnlyList<InventoryItemDelta> additions,
        IReadOnlyList<InventoryItemDelta> removals,
        long moneyDelta = 0,
        CancellationToken cancellationToken = default)
    {
        lock (_gate)
        {
            var characters = ReadLocked(CharactersFile, static () => new List<CharacterTableRow>());
            var characterRow = characters.FirstOrDefault(row => row.Character.Guid == characterGuid);
            if (characterRow is null)
                return ValueTask.FromResult<CharacterInventoryMutationResult?>(null);

            var inventory = ReadLocked(CharacterInventoryFile, static () => new List<CharacterInventoryTableRow>());
            var ownedRows = inventory.Where(row => row.Owner == characterGuid).ToList();
            var additionTotals = additions
                .Where(delta => delta.ItemTemplate > 0 && delta.Count > 0)
                .GroupBy(delta => delta.ItemTemplate)
                .ToDictionary(group => group.Key, group => group.Aggregate(0UL, (total, delta) => total + delta.Count));
            var removalTotals = removals
                .Where(delta => delta.ItemTemplate > 0 && delta.Count > 0)
                .GroupBy(delta => delta.ItemTemplate)
                .ToDictionary(group => group.Key, group => group.Aggregate(0UL, (total, delta) => total + delta.Count));

            foreach (var removal in removalTotals)
            {
                var owned = ownedRows
                    .Where(row => row.ItemTemplate == removal.Key)
                    .Aggregate(0UL, (total, row) => total + row.StackCount);
                if (owned < removal.Value)
                    return ValueTask.FromResult<CharacterInventoryMutationResult?>(null);
            }

            var currentMoney = characterRow.Character.Money;
            var nextMoney = (long)currentMoney + moneyDelta;
            if (nextMoney is < 0 or > uint.MaxValue)
                return ValueTask.FromResult<CharacterInventoryMutationResult?>(null);

            foreach (var itemEntry in additionTotals.Keys)
            {
                if (FindItemTemplateLocked(itemEntry) is null)
                    return ValueTask.FromResult<CharacterInventoryMutationResult?>(null);
            }

            var workingRows = ownedRows
                .Select(CloneInventoryRow)
                .ToList();
            var updatedItems = new List<CharacterInventoryRecord>();
            var removedGuids = new List<ulong>();

            foreach (var removal in removalTotals)
            {
                var remaining = removal.Value;
                foreach (var row in workingRows.Where(row => row.ItemTemplate == removal.Key).OrderBy(row => row.Guid).ToArray())
                {
                    if (remaining == 0)
                        break;
                    if ((ulong)row.StackCount <= remaining)
                    {
                        remaining -= row.StackCount;
                        workingRows.Remove(row);
                        removedGuids.Add(row.Guid);
                    }
                    else
                    {
                        row.StackCount = checked(row.StackCount - (uint)remaining);
                        remaining = 0;
                        updatedItems.Add(row.ToRecord());
                    }
                }
            }

            var usedSlots = workingRows
                .Where(row => row.Bag == 23 && row.Slot is >= 23 and < 39)
                .Select(row => row.Slot)
                .ToHashSet();
            var nextItemGuid = inventory.Count == 0 ? 1UL : inventory.Max(row => row.Guid) + 1;
            foreach (var addition in additionTotals)
            {
                var template = FindItemTemplateLocked(addition.Key)!;
                var maxStack = Math.Max(1u, template.Stackable);
                var remaining = addition.Value;
                foreach (var row in workingRows.Where(row => row.ItemTemplate == addition.Key).OrderBy(row => row.Guid).ToArray())
                {
                    if (remaining == 0)
                        break;
                    if (row.StackCount >= maxStack)
                        continue;
                    var amount = Math.Min(maxStack - row.StackCount, checked((uint)Math.Min((ulong)uint.MaxValue, remaining)));
                    row.StackCount += amount;
                    remaining -= amount;
                    updatedItems.Add(row.ToRecord());
                }

                while (remaining > 0)
                {
                    var slot = Enumerable.Range(23, 16)
                        .Select(value => checked((byte)value))
                        .FirstOrDefault(candidate => !usedSlots.Contains(candidate));
                    if (slot == 0)
                        return ValueTask.FromResult<CharacterInventoryMutationResult?>(null);

                    var amount = Math.Min(maxStack, checked((uint)Math.Min((ulong)uint.MaxValue, remaining)));
                    var row = new CharacterInventoryTableRow
                    {
                        Guid = nextItemGuid++, Owner = characterGuid, Bag = 23, Slot = slot,
                        ItemTemplate = addition.Key, StackCount = amount,
                        SpellCharges1 = template.SpellCharges1, SpellCharges2 = template.SpellCharges2,
                        SpellCharges3 = template.SpellCharges3, SpellCharges4 = template.SpellCharges4,
                        SpellCharges5 = template.SpellCharges5, Duration = template.Duration
                    };
                    workingRows.Add(row);
                    usedSlots.Add(slot);
                    remaining -= amount;
                }
            }

            inventory.RemoveAll(row => row.Owner == characterGuid);
            inventory.AddRange(workingRows);
            WriteLocked(CharacterInventoryFile, inventory);
            characterRow.Character = characterRow.Character with { Money = checked((uint)nextMoney) };
            WriteLocked(CharactersFile, characters);

            var addedGuids = workingRows
                .Where(row => !ownedRows.Any(old => old.Guid == row.Guid))
                .Select(row => row.ToRecord())
                .ToArray();
            var result = new CharacterInventoryMutationResult(
                addedGuids,
                updatedItems.DistinctBy(item => item.Guid).ToArray(),
                removedGuids,
                checked((uint)nextMoney));
            return ValueTask.FromResult<CharacterInventoryMutationResult?>(result);
        }
    }

    public ValueTask<StartingLocation?> GetStartingLocationAsync(
        byte race,
        byte classId,
        CancellationToken cancellationToken = default)
    {
        var location = Read(StartingLocationsFile, static () => new List<StartingLocationTableRow>())
            .FirstOrDefault(row => row.Race == race && row.Class == classId)
            ?.ToLocation() ?? StartingLocation.Default;
        return ValueTask.FromResult<StartingLocation?>(location);
    }

    public ValueTask<CharacterSummary?> CreateCharacterAsync(
        CharacterCreateRequest request,
        CancellationToken cancellationToken = default)
    {
        lock (_gate)
        {
            var rows = ReadLocked(CharactersFile, static () => new List<CharacterTableRow>());
            if (rows.Count(row => row.AccountId == request.AccountId && row.RealmId == request.RealmId) >= 10
                || rows.Any(row => row.RealmId == request.RealmId
                    && string.Equals(row.Character.Name, request.Name, StringComparison.OrdinalIgnoreCase)))
                return ValueTask.FromResult<CharacterSummary?>(null);

            var location = request.StartingLocation;
            var guid = rows.Count == 0 ? 1UL : rows.Max(row => row.Character.Guid) + 1;
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
            rows.Add(new CharacterTableRow { AccountId = request.AccountId, RealmId = request.RealmId, Character = character });
            WriteLocked(CharactersFile, rows);
            return ValueTask.FromResult<CharacterSummary?>(character);
        }
    }

    public ValueTask<bool> DeleteCharacterAsync(
        int accountId,
        int realmId,
        ulong guid,
        CancellationToken cancellationToken = default)
    {
        lock (_gate)
        {
            var rows = ReadLocked(CharactersFile, static () => new List<CharacterTableRow>());
            var removed = rows.RemoveAll(row => row.AccountId == accountId && row.RealmId == realmId && row.Character.Guid == guid) > 0;
            if (!removed)
                return ValueTask.FromResult(false);

            WriteLocked(CharactersFile, rows);
            RemoveStartupRowsLocked(guid);
            return ValueTask.FromResult(true);
        }
    }

    public ValueTask<IReadOnlyList<uint>> GetStartingSpellIdsAsync(
        byte race,
        byte classId,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyList<uint> spells = Read(StartingSpellsFile, static () => new List<StartingSpellTableRow>())
            .Where(row => row.Race == race && row.Class == classId)
            .OrderBy(row => row.Spell)
            .Select(row => row.Spell)
            .ToArray();
        return ValueTask.FromResult(spells);
    }

    public ValueTask<IReadOnlyDictionary<int, int>> GetStartingActionButtonsAsync(
        byte race,
        byte classId,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyDictionary<int, int> buttons = Read(StartingActionsFile, static () => new List<StartingActionTableRow>())
            .Where(row => row.Race == race && row.Class == classId)
            .ToDictionary(row => row.Button, row => row.Action);
        return ValueTask.FromResult(buttons);
    }

    public ValueTask<IReadOnlyList<StartingItemRecord>> GetStartingItemRecordsAsync(
        byte race,
        byte classId,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyList<StartingItemRecord> items = Read(StartingItemsFile, static () => new List<StartingItemTableRow>())
            .Where(row => row.Race == race && row.Class == classId)
            .OrderBy(row => row.Id)
            .Select(row => new StartingItemRecord(row.ItemId, row.Amount))
            .ToArray();
        return ValueTask.FromResult(items);
    }

    public ValueTask<ItemTemplateRecord?> GetItemTemplateAsync(
        uint entry,
        CancellationToken cancellationToken = default)
    {
        ItemTemplateRecord? template;
        lock (_gate)
        {
            _itemTemplateCache ??= ReadLocked(ItemTemplatesFile, static () => new List<ItemTemplateRecord>())
                .GroupBy(item => item.Entry)
                .ToDictionary(group => group.Key, group => group.First());
            _itemTemplateCache.TryGetValue(entry, out template);
        }
        return ValueTask.FromResult(template);
    }

    public ValueTask<PlayerRaceDefinition?> GetRaceDefinitionAsync(
        byte race,
        CancellationToken cancellationToken = default)
    {
        var definition = Read(RacesFile, DefaultRaces)
            .FirstOrDefault(item => item.Race == race);
        return ValueTask.FromResult(definition);
    }

    public ValueTask<IReadOnlyList<uint>> GetStartingOutfitItemIdsAsync(
        byte race,
        byte classId,
        byte gender,
        CancellationToken cancellationToken = default)
    {
        IReadOnlyList<uint> items = Read(OutfitsFile, static () => new List<OutfitTableRow>())
            .FirstOrDefault(row => row.Race == race && row.Class == classId && row.Gender == gender)
            ?.ItemIds.Where(item => item > 0).ToArray() ?? Array.Empty<uint>();
        return ValueTask.FromResult(items);
    }

    private void InitializeTables()
    {
        lock (_gate)
        {
            var accounts = ReadLocked(AccountsFile, () => new List<AccountRecord> { CreateDefaultAccount() });
            var username = Environment.GetEnvironmentVariable("BATTLE_NET_TEST_USERNAME");
            var password = Environment.GetEnvironmentVariable("BATTLE_NET_TEST_PASSWORD");
            if (!string.IsNullOrWhiteSpace(username) && password is not null
                && accounts.All(account => !string.Equals(account.Name, username, StringComparison.OrdinalIgnoreCase)))
            {
                accounts.Add(CreatePasswordAccount(username, password, accounts.Count + 1));
                WriteLocked(AccountsFile, accounts);
            }

            ReadLocked(RealmsFile, () => new List<RealmInfo>
            {
                new(
                    _options.LocalRealmId,
                    _options.RealmName,
                    _options.WorldAdvertisedAddress,
                    _options.WorldPort,
                    _options.RealmAdvertisedAddress,
                    _options.ProxyPort,
                    0)
            });
            ReadLocked(CharactersFile, static () => new List<CharacterTableRow>());
            ReadLocked(CharacterSpellsFile, static () => new List<CharacterSpellTableRow>());
            ReadLocked(CharacterButtonsFile, static () => new List<CharacterButtonTableRow>());
            ReadLocked(CharacterSpellBookFile, static () => new List<CharacterSpellBookTableRow>());
            ReadLocked(CharacterInventoryFile, static () => new List<CharacterInventoryTableRow>());
            ReadLocked(CharacterQuestsFile, static () => new List<CharacterQuestTableRow>());
            ReadLocked(StartingLocationsFile, static () => new List<StartingLocationTableRow>());
            ReadLocked(StartingSpellsFile, static () => new List<StartingSpellTableRow>());
            ReadLocked(StartingActionsFile, static () => new List<StartingActionTableRow>());
            ReadLocked(StartingItemsFile, static () => new List<StartingItemTableRow>());
            ReadLocked(RacesFile, DefaultRaces);
            ReadLocked(OutfitsFile, static () => new List<OutfitTableRow>());
            ReadLocked(ItemTemplatesFile, static () => new List<ItemTemplateRecord>());
        }
    }

    private void RemoveStartupRowsLocked(ulong guid)
    {
        var spells = ReadLocked(CharacterSpellsFile, static () => new List<CharacterSpellTableRow>());
        spells.RemoveAll(row => row.Guid == guid);
        WriteLocked(CharacterSpellsFile, spells);

        var buttons = ReadLocked(CharacterButtonsFile, static () => new List<CharacterButtonTableRow>());
        buttons.RemoveAll(row => row.Owner == guid);
        WriteLocked(CharacterButtonsFile, buttons);

        var spellBook = ReadLocked(CharacterSpellBookFile, static () => new List<CharacterSpellBookTableRow>());
        spellBook.RemoveAll(row => row.Owner == guid);
        WriteLocked(CharacterSpellBookFile, spellBook);

        var inventory = ReadLocked(CharacterInventoryFile, static () => new List<CharacterInventoryTableRow>());
        inventory.RemoveAll(row => row.Owner == guid);
        WriteLocked(CharacterInventoryFile, inventory);

        var quests = ReadLocked(CharacterQuestsFile, static () => new List<CharacterQuestTableRow>());
        quests.RemoveAll(row => row.CharacterGuid == guid);
        WriteLocked(CharacterQuestsFile, quests);
    }

    private T Read<T>(string fileName, Func<T> seed)
    {
        lock (_gate)
            return ReadLocked(fileName, seed);
    }

    private ItemTemplateRecord? FindItemTemplateLocked(uint entry)
    {
        _itemTemplateCache ??= ReadLocked(ItemTemplatesFile, static () => new List<ItemTemplateRecord>())
            .GroupBy(item => item.Entry)
            .ToDictionary(group => group.Key, group => group.First());
        return _itemTemplateCache.TryGetValue(entry, out var template) ? template : null;
    }

    private static CharacterInventoryTableRow CloneInventoryRow(CharacterInventoryTableRow row) => new()
    {
        Guid = row.Guid,
        Owner = row.Owner,
        Creator = row.Creator,
        Bag = row.Bag,
        Slot = row.Slot,
        ItemTemplate = row.ItemTemplate,
        StackCount = row.StackCount,
        SpellCharges1 = row.SpellCharges1,
        SpellCharges2 = row.SpellCharges2,
        SpellCharges3 = row.SpellCharges3,
        SpellCharges4 = row.SpellCharges4,
        SpellCharges5 = row.SpellCharges5,
        ItemFlags = row.ItemFlags,
        Duration = row.Duration,
        Enchantments = row.Enchantments
    };

    private T ReadLocked<T>(string fileName, Func<T> seed)
    {
        var path = Path.Combine(_directory, fileName);
        if (!File.Exists(path))
        {
            var created = seed();
            WriteLocked(fileName, created);
            return created;
        }

        var json = File.ReadAllText(path);
        if (string.IsNullOrWhiteSpace(json))
            return seed();
        return JsonSerializer.Deserialize<T>(json, JsonOptions) ?? seed();
    }

    private void WriteLocked<T>(string fileName, T value)
    {
        var path = Path.Combine(_directory, fileName);
        var temporaryPath = path + ".tmp-" + Guid.NewGuid().ToString("N");
        File.WriteAllText(temporaryPath, JsonSerializer.Serialize(value, JsonOptions), Encoding.UTF8);
        File.Move(temporaryPath, path, true);
    }

    private static AccountRecord CreateDefaultAccount() => CreatePasswordAccount("TEST", "PASSWORD", 1);

    private static AccountRecord CreatePasswordAccount(string username, string password, int id) =>
        AccountRegistration.CreateRecord(id, username, password);

    private static List<PlayerRaceDefinition> DefaultRaces() =>
    [
        new(1, 1, 49, 50, 1.0f, 7, 7),
        new(2, 2, 51, 52, 1.1f, 1, 7),
        new(3, 3, 53, 54, 1.0f, 7, 7),
        new(4, 4, 55, 56, 1.0f, 7, 7),
        new(5, 5, 57, 58, 1.0f, 6, 6),
        new(6, 6, 59, 60, 1.0f, 7, 7),
        new(7, 115, 1563, 1564, 0.8f, 7, 7),
        new(8, 116, 1478, 1479, 1.0f, 7, 7)
    ];

    private sealed class CharacterTableRow
    {
        public int AccountId { get; set; }
        public int RealmId { get; set; }
        public CharacterSummary Character { get; set; } = new(0, string.Empty, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0);
    }

    private sealed class CharacterSpellTableRow
    {
        public ulong Guid { get; set; }
        public uint SpellId { get; set; }
        public bool Active { get; set; } = true;
        public bool Disabled { get; set; }
    }

    private sealed class CharacterButtonTableRow
    {
        public ulong Owner { get; set; }
        public int Index { get; set; }
        public int Action { get; set; }
    }

    private sealed class CharacterSpellBookTableRow
    {
        public ulong Owner { get; set; }
        public int Index { get; set; }
        public uint Spell { get; set; }
    }

    private sealed class CharacterInventoryTableRow
    {
        public ulong Guid { get; set; }
        public ulong Owner { get; set; }
        public ulong Creator { get; set; }
        public uint Bag { get; set; }
        public byte Slot { get; set; }
        public uint ItemTemplate { get; set; }
        public uint StackCount { get; set; }
        public int SpellCharges1 { get; set; } = -1;
        public int SpellCharges2 { get; set; } = -1;
        public int SpellCharges3 { get; set; } = -1;
        public int SpellCharges4 { get; set; } = -1;
        public int SpellCharges5 { get; set; } = -1;
        public uint ItemFlags { get; set; }
        public uint Duration { get; set; }
        public string Enchantments { get; set; } = "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0";

        public CharacterInventoryRecord ToRecord() => new(
            Guid, Owner, Creator, Bag, Slot, ItemTemplate, StackCount,
            SpellCharges1, SpellCharges2, SpellCharges3, SpellCharges4, SpellCharges5,
            ItemFlags, Duration, Enchantments);
    }

    private sealed class CharacterQuestTableRow
    {
        public ulong CharacterGuid { get; set; }
        public uint QuestId { get; set; }
        public int State { get; set; } = 2;
        public bool Rewarded { get; set; }
        public List<uint> ObjectiveCounts { get; set; } = [];
    }

    private sealed class StartingLocationTableRow
    {
        public byte Race { get; set; }
        public byte Class { get; set; }
        public uint Map { get; set; }
        public uint Zone { get; set; }
        public float PositionX { get; set; }
        public float PositionY { get; set; }
        public float PositionZ { get; set; }
        public float Orientation { get; set; }

        public StartingLocation ToLocation() => new(Map, Zone, PositionX, PositionY, PositionZ, Orientation);
    }

    private sealed class StartingSpellTableRow
    {
        public byte Race { get; set; }
        public byte Class { get; set; }
        public uint Spell { get; set; }
    }

    private sealed class StartingActionTableRow
    {
        public byte Race { get; set; }
        public byte Class { get; set; }
        public int Button { get; set; }
        public int Action { get; set; }
    }

    private sealed class StartingItemTableRow
    {
        public ulong Id { get; set; }
        public byte Race { get; set; }
        public byte Class { get; set; }
        public uint ItemId { get; set; }
        public uint Amount { get; set; } = 1;
    }

    private sealed class OutfitTableRow
    {
        public byte Race { get; set; }
        public byte Class { get; set; }
        public byte Gender { get; set; }
        public List<uint> ItemIds { get; set; } = [];
    }
}

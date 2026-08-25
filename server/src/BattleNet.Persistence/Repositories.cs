using BattleNet.Persistence.Models;

namespace BattleNet.Persistence;

public interface IAuthRepository
{
    ValueTask<AccountRecord?> FindAccountAsync(string username, CancellationToken cancellationToken = default);
    ValueTask<AccountCreateResult> CreateAccountAsync(string username, string password, CancellationToken cancellationToken = default);
    ValueTask<bool> UpdateSessionKeyAsync(string username, string sessionKeyHex, string ipAddress, CancellationToken cancellationToken = default);
    ValueTask<IReadOnlyList<RealmInfo>> GetRealmsAsync(CancellationToken cancellationToken = default);
    ValueTask SetOnlinePlayerCountAsync(int realmId, int count, CancellationToken cancellationToken = default);
}

public interface IRealmRepository
{
    ValueTask<IReadOnlyList<CharacterSummary>> GetCharactersAsync(int accountId, int realmId, CancellationToken cancellationToken = default);
    ValueTask<CharacterSummary?> FindCharacterAsync(ulong guid, int realmId, CancellationToken cancellationToken = default);
    ValueTask<IReadOnlyList<CharacterSpellRecord>> GetCharacterSpellsAsync(ulong guid, CancellationToken cancellationToken = default);
    ValueTask<IReadOnlyDictionary<int, int>> GetActionButtonsAsync(ulong guid, CancellationToken cancellationToken = default);
    ValueTask<IReadOnlyDictionary<uint, int>> GetSpellButtonsAsync(ulong guid, CancellationToken cancellationToken = default);
    ValueTask<IReadOnlyList<CharacterInventoryRecord>> GetCharacterInventoryAsync(ulong guid, CancellationToken cancellationToken = default);
    ValueTask InitializeCharacterStartupStateAsync(ulong guid, CharacterStartupState state, CancellationToken cancellationToken = default);
    ValueTask<StartingLocation?> GetStartingLocationAsync(byte race, byte classId, CancellationToken cancellationToken = default);
    ValueTask<CharacterSummary?> CreateCharacterAsync(CharacterCreateRequest request, CancellationToken cancellationToken = default);
    ValueTask<bool> DeleteCharacterAsync(int accountId, int realmId, ulong guid, CancellationToken cancellationToken = default);
}

public interface IQuestRepository
{
    ValueTask<IReadOnlyList<CharacterQuestRecord>> GetCharacterQuestsAsync(
        ulong characterGuid,
        CancellationToken cancellationToken = default);

    ValueTask<bool> AcceptCharacterQuestAsync(
        CharacterQuestRecord quest,
        CancellationToken cancellationToken = default);

    ValueTask<bool> RemoveCharacterQuestAsync(
        ulong characterGuid,
        uint questId,
        CancellationToken cancellationToken = default);

    ValueTask<bool> UpdateCharacterQuestAsync(
        CharacterQuestRecord quest,
        CancellationToken cancellationToken = default);
}

public interface ICharacterMutationRepository
{
    ValueTask<CharacterInventoryMutationResult?> ApplyInventoryMutationAsync(
        ulong characterGuid,
        IReadOnlyList<InventoryItemDelta> additions,
        IReadOnlyList<InventoryItemDelta> removals,
        long moneyDelta = 0,
        CancellationToken cancellationToken = default);
}

public interface ICharacterStateRepository
{
    ValueTask<bool> SaveCharacterStateAsync(
        CharacterSummary character,
        CancellationToken cancellationToken = default);
}

public interface IDbcRepository
{
    ValueTask<PlayerRaceDefinition?> GetRaceDefinitionAsync(byte race, CancellationToken cancellationToken = default);
    ValueTask<IReadOnlyList<uint>> GetStartingOutfitItemIdsAsync(byte race, byte classId, byte gender, CancellationToken cancellationToken = default);
}

public interface IWorldRepository
{
    ValueTask<IReadOnlyList<uint>> GetStartingSpellIdsAsync(byte race, byte classId, CancellationToken cancellationToken = default);
    ValueTask<IReadOnlyDictionary<int, int>> GetStartingActionButtonsAsync(byte race, byte classId, CancellationToken cancellationToken = default);
    ValueTask<IReadOnlyList<StartingItemRecord>> GetStartingItemRecordsAsync(byte race, byte classId, CancellationToken cancellationToken = default);
    ValueTask<ItemTemplateRecord?> GetItemTemplateAsync(uint entry, CancellationToken cancellationToken = default);
}

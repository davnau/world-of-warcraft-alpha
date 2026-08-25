namespace BattleNet.Persistence.Models;

public sealed record InventoryItemDelta(uint ItemTemplate, uint Count);

public sealed record CharacterInventoryMutationResult(
    IReadOnlyList<CharacterInventoryRecord> AddedItems,
    IReadOnlyList<CharacterInventoryRecord> UpdatedItems,
    IReadOnlyList<ulong> RemovedItemGuids,
    uint Money);

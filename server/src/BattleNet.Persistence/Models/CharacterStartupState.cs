namespace BattleNet.Persistence.Models;

/// <summary>
/// A spell row as persisted by Alpha Core's character_spells table.
/// </summary>
public sealed record CharacterSpellRecord(
    uint SpellId,
    bool Active,
    bool Disabled);

/// <summary>
/// An item instance as persisted by Alpha Core's character_inventory table.
/// The item template row is intentionally kept separate; the object update can
/// be sent from this state while a later DBC/world repository supplies query data.
/// </summary>
public sealed record CharacterInventoryRecord(
    ulong Guid,
    ulong Owner,
    ulong Creator,
    uint Bag,
    byte Slot,
    uint ItemTemplate,
    uint StackCount,
    int SpellCharges1 = -1,
    int SpellCharges2 = -1,
    int SpellCharges3 = -1,
    int SpellCharges4 = -1,
    int SpellCharges5 = -1,
    uint ItemFlags = 0,
    uint Duration = 0,
    string Enchantments = "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0");

public sealed record CharacterStartupState(
    IReadOnlyList<uint> SpellIds,
    IReadOnlyDictionary<int, int> ActionButtons,
    IReadOnlyList<CharacterInventoryRecord> Inventory);

public sealed record StartingItemRecord(uint ItemId, uint Amount = 1);

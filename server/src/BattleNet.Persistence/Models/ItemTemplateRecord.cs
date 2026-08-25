namespace BattleNet.Persistence.Models;

public sealed record ItemStatRecord(int Type, int Value);

public sealed record ItemDamageRecord(float Minimum, float Maximum, int Type);

public sealed record ItemSpellRecord(
    int SpellId,
    int Trigger,
    int Charges,
    int Cooldown,
    int Category,
    int CategoryCooldown);

/// <summary>
/// The Alpha item_template fields used by starter-item construction and the
/// SMSG_ITEM_QUERY_SINGLE/MULTIPLE_RESPONSE wire layouts.
/// </summary>
public sealed record ItemTemplateRecord(
    uint Entry,
    int Class,
    int Subclass,
    string Name,
    string Description,
    uint DisplayId,
    uint Quality,
    uint Flags,
    uint BuyPrice,
    uint SellPrice,
    uint InventoryType,
    int AllowableClass,
    int AllowableRace,
    uint ItemLevel,
    uint RequiredLevel,
    uint RequiredSkill,
    uint RequiredSkillRank,
    uint MaxCount,
    uint Stackable,
    uint ContainerSlots,
    IReadOnlyList<ItemStatRecord> Stats,
    IReadOnlyList<ItemDamageRecord> Damages,
    int Armor,
    int HolyResistance,
    int FireResistance,
    int NatureResistance,
    int FrostResistance,
    int ShadowResistance,
    int Delay,
    uint AmmoType,
    IReadOnlyList<ItemSpellRecord> Spells,
    uint Bonding,
    uint PageText,
    uint PageLanguage,
    uint PageMaterial,
    uint StartQuest,
    uint LockId,
    int Material,
    uint Sheath,
    uint Duration,
    int SpellCharges1 = 0,
    int SpellCharges2 = 0,
    int SpellCharges3 = 0,
    int SpellCharges4 = 0,
    int SpellCharges5 = 0);

using System.Buffers.Binary;
using System.IO;
using BattleNet.Persistence.Models;

namespace BattleNet.Server;

/// <summary>
/// Builds the Alpha Core player CREATE_OBJECT update sent after CMSG_PLAYER_LOGIN.
///
/// The field indexes intentionally mirror utils/constants/UpdateFields.py. The
/// writer consumes persisted character state plus ChrRaces data; skills, auras,
/// inventory objects, and later partial updates remain separate port boundaries.
/// </summary>
internal static class PlayerUpdatePacketWriter
{
    private const int PlayerFieldCount = 634; // PlayerFields.END in Alpha Core.
    private const int UpdateMaskBlockCount = (PlayerFieldCount + 31) / 32;
    private const uint PlayerTypeMask = 25; // TYPE_OBJECT | TYPE_UNIT | TYPE_PLAYER.
    private const byte PlayerObjectTypeId = 4;

    private const int ObjectFieldGuid = 0x00;
    private const int ObjectFieldType = 0x02;
    private const int ObjectFieldScale = 0x04;

    private const int UnitFieldHealth = 0x16;
    private const int UnitFieldPower1 = 0x17;
    private const int UnitFieldPower2 = 0x18;
    private const int UnitFieldPower3 = 0x19;
    private const int UnitFieldPower4 = 0x1A;
    private const int UnitFieldMaxHealth = 0x1B;
    private const int UnitFieldMaxPower1 = 0x1C;
    private const int UnitFieldMaxPower2 = 0x1D;
    private const int UnitFieldMaxPower3 = 0x1E;
    private const int UnitFieldMaxPower4 = 0x1F;
    private const int UnitFieldLevel = 0x20;
    private const int UnitFieldFactionTemplate = 0x21;
    private const int UnitFieldBytes0 = 0x22;
    private const int UnitFieldFlags = 0x36;
    private const int UnitFieldCoinage = 0x37;
    private const int UnitFieldBaseAttackTime = 0x8C;
    private const int UnitFieldBoundingRadius = 0x94;
    private const int UnitFieldCombatReach = 0x95;
    private const int UnitFieldWeaponReach = 0x96;
    private const int UnitFieldDisplayId = 0x97;
    private const int UnitFieldMountDisplayId = 0x98;
    private const int UnitFieldDamage = 0x99;
    private const int UnitFieldBytes1 = 0xAC;
    private const int UnitFieldBytes2 = 0xB6;

    private const int PlayerFieldsStart = 0xB8; // UnitFields.END.
    private const int PlayerFieldInventorySlots = PlayerFieldsStart + 0x00;
    private const int PlayerFieldPackSlots = PlayerFieldsStart + 0x2E;
    private const int PlayerFieldBankSlots = PlayerFieldsStart + 0x4E;
    private const int PlayerFieldBankBagSlots = PlayerFieldsStart + 0x7E;
    private const int PlayerFieldNumInventorySlots = PlayerFieldsStart + 0x90;
    private const int PlayerFieldBytes = PlayerFieldsStart + 0x93;
    private const int PlayerFieldXp = PlayerFieldsStart + 0x94;
    private const int PlayerFieldNextLevelXp = PlayerFieldsStart + 0x95;
    private const int PlayerFieldBytes2 = PlayerFieldsStart + 0x156;
    private const int PlayerFieldCharacterPoints1 = PlayerFieldsStart + 0x1B7;
    private const int PlayerFieldCharacterPoints2 = PlayerFieldsStart + 0x1B8;

    public static byte[] Build(
        CharacterSummary character,
        PlayerRaceDefinition? raceDefinition = null,
        IReadOnlyList<CharacterInventoryRecord>? inventory = null)
    {
        var fields = BuildFields(
            character,
            raceDefinition ?? GetFallbackRaceDefinition(character.Race),
            inventory);

        using var stream = new MemoryStream(capacity: 4 + 10 + 68 + 20 + 1 + (UpdateMaskBlockCount * 4) + (fields.Length * 4));
        using var writer = new BinaryWriter(stream);

        // One update transaction containing one CREATE_OBJECT block.
        writer.Write(1u);
        writer.Write((byte)2); // UpdateTypes.CREATE_OBJECT.
        writer.Write(character.Guid);
        writer.Write(PlayerObjectTypeId);
        WriteMovement(writer, character);

        // CREATE_OBJECT flags, attack cycle, timer id, and current victim.
        writer.Write(1u); // UPDATEFLAG_SELF.
        writer.Write(1u); // Attack cycle applies to players/items.
        writer.Write(0u);
        writer.Write(0UL);

        writer.Write((byte)UpdateMaskBlockCount);
        for (var block = 0; block < UpdateMaskBlockCount; block++)
        {
            var mask = uint.MaxValue;
            if (block == UpdateMaskBlockCount - 1)
                mask = (1u << (PlayerFieldCount % 32)) - 1;
            writer.Write(mask);
        }

        foreach (var field in fields)
            writer.Write(field);

        return stream.ToArray();
    }

    public static byte[] BuildMoneyUpdate(ulong playerGuid, uint money)
    {
        return BuildFieldUpdate(playerGuid, (UnitFieldCoinage, money));
    }

    public static byte[] BuildHealthUpdate(ulong playerGuid, uint health, uint maxHealth)
    {
        return BuildFieldUpdate(
            playerGuid,
            (UnitFieldHealth, health),
            (UnitFieldMaxHealth, maxHealth));
    }

    public static byte[] BuildPowerUpdate(ulong playerGuid, uint power, uint maxPower)
    {
        return BuildFieldUpdate(
            playerGuid,
            (UnitFieldPower1, power),
            (UnitFieldMaxPower1, maxPower));
    }

    private static byte[] BuildFieldUpdate(ulong playerGuid, params (int Field, uint Value)[] updates)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(1u);
        writer.Write((byte)0); // UpdateTypes.PARTIAL.
        writer.Write(playerGuid);
        writer.Write((byte)UpdateMaskBlockCount);
        var masks = new uint[UpdateMaskBlockCount];
        foreach (var update in updates)
            masks[update.Field / 32] |= 1u << (update.Field % 32);
        foreach (var mask in masks)
            writer.Write(mask);
        foreach (var update in updates.OrderBy(update => update.Field))
            writer.Write(update.Value);
        return stream.ToArray();
    }

    private static uint[] BuildFields(
        CharacterSummary character,
        PlayerRaceDefinition? raceDefinition,
        IReadOnlyList<CharacterInventoryRecord>? inventory)
    {
        var fields = new uint[PlayerFieldCount];
        var nativeScale = GetNativeScale(character.Race, character.Gender);
        var combatReach = GetCombatReach(character.Race, character.Gender);
        fields[ObjectFieldGuid] = unchecked((uint)character.Guid);
        fields[ObjectFieldGuid + 1] = checked((uint)(character.Guid >> 32));
        fields[ObjectFieldType] = PlayerTypeMask;
        fields[ObjectFieldScale] = SingleBits(nativeScale);

        fields[UnitFieldHealth] = character.Health;
        fields[UnitFieldMaxHealth] = character.Health;
        fields[UnitFieldPower1] = character.Power1;
        fields[UnitFieldPower2] = character.Power2;
        fields[UnitFieldPower3] = character.Power3;
        fields[UnitFieldPower4] = character.Power4;
        fields[UnitFieldMaxPower1] = character.Power1;
        fields[UnitFieldMaxPower2] = 1000;
        fields[UnitFieldMaxPower3] = 100;
        fields[UnitFieldMaxPower4] = 100;
        fields[UnitFieldLevel] = character.Level;
        fields[UnitFieldFactionTemplate] = raceDefinition?.FactionId ?? GetFactionTemplate(character.Race);
        fields[UnitFieldBytes0] = PackBytes(character.Race, character.Class, character.Gender, GetPowerType(character.Class));
        fields[UnitFieldFlags] = 0x00000008; // UNIT_FLAG_PLAYER_CONTROLLED.
        fields[UnitFieldCoinage] = character.Money;
        fields[UnitFieldBaseAttackTime] = 2000;
        fields[UnitFieldBaseAttackTime + 1] = 2000;
        fields[UnitFieldBoundingRadius] = SingleBits(0.306f);
        fields[UnitFieldCombatReach] = SingleBits(combatReach);
        fields[UnitFieldWeaponReach] = SingleBits(combatReach);
        fields[UnitFieldDisplayId] = raceDefinition is null
            ? 0
            : character.Gender == 0 ? raceDefinition.MaleDisplayId : raceDefinition.FemaleDisplayId;
        fields[UnitFieldMountDisplayId] = 0;
        fields[UnitFieldDamage] = 1;
        fields[UnitFieldBytes1] = 0;
        fields[UnitFieldBytes2] = 0;

        fields[PlayerFieldNumInventorySlots] = 0x89;
        fields[PlayerFieldBytes] = PackBytes(character.HairColor, character.HairStyle, character.Face, character.Skin);
        fields[PlayerFieldXp] = character.Experience;
        fields[PlayerFieldNextLevelXp] = GetExperienceToNextLevel(character.Level);
        fields[PlayerFieldBytes2] = PackBytes(0, character.BankSlots, character.FacialHair, 0);
        fields[PlayerFieldCharacterPoints1] = character.TalentPoints;
        fields[PlayerFieldCharacterPoints2] = character.SkillPoints;
        SetInventoryFieldGuids(fields, inventory);
        return fields;
    }

    private static uint GetExperienceToNextLevel(byte level)
    {
        if (level == 0)
            return 0;

        var difference = level > 31 ? 5u * ((uint)level - 30u)
            : level == 31 ? 6u
            : level == 30 ? 3u
            : level == 29 ? 1u
            : 0u;
        var raw = (8u * level + difference) * (45u + 5u * level) + 1u;
        return ((raw + 50u) / 100u) * 100u;
    }

    private static void SetInventoryFieldGuids(
        uint[] fields,
        IReadOnlyList<CharacterInventoryRecord>? inventory)
    {
        if (inventory is null)
            return;

        foreach (var item in inventory)
        {
            var itemGuid = item.Guid | (0x4000UL << 48); // HighGuid.HIGHGUID_ITEM.
            var fieldIndex = item.Bag switch
            {
                23 when item.Slot < 23 => PlayerFieldInventorySlots + (item.Slot * 2),
                23 when item.Slot is >= 23 and < 39 => PlayerFieldPackSlots + ((item.Slot - 23) * 2),
                23 when item.Slot is >= 39 and < 63 => PlayerFieldBankSlots + ((item.Slot - 39) * 2),
                23 when item.Slot is >= 63 and <= 68 => PlayerFieldBankBagSlots + ((item.Slot - 63) * 2),
                _ => -1
            };
            if (fieldIndex < 0 || fieldIndex + 1 >= fields.Length)
                continue;

            fields[fieldIndex] = unchecked((uint)itemGuid);
            fields[fieldIndex + 1] = checked((uint)(itemGuid >> 32));
        }
    }

    private static void WriteMovement(BinaryWriter writer, CharacterSummary character)
    {
        // Alpha Core ObjectManager._get_movement_fields(): <Q9fI followed by <I4f.
        writer.Write(0UL); // Transport guid.
        writer.Write(0.0f);
        writer.Write(0.0f);
        writer.Write(0.0f);
        writer.Write(0.0f);
        writer.Write(character.PositionX);
        writer.Write(character.PositionY);
        writer.Write(character.PositionZ);
        writer.Write(character.Orientation);
        writer.Write(0.0f); // Pitch.
        writer.Write(0u); // Movement flags.
        writer.Write(0u); // Fall time.
        writer.Write(2.5f); // Walk speed.
        writer.Write(7.0f); // Run speed.
        writer.Write(4.72f); // Swim speed.
        writer.Write(MathF.PI); // Turn rate.
    }

    internal static float GetNativeScale(byte race, byte gender) => race switch
    {
        6 => gender == 0 ? 1.35f : 1.25f, // Tauren.
        7 => 1.15f, // Gnome.
        _ => 1.0f
    };

    private static float GetCombatReach(byte race, byte gender) => race switch
    {
        6 => gender == 0 ? 4.05f : 3.75f, // Tauren.
        7 => 1.725f, // Gnome.
        _ => 1.5f
    };

    private static uint GetFactionTemplate(byte race) => race switch
    {
        1 => 1,
        2 => 2,
        3 => 3,
        4 => 4,
        5 => 5,
        6 => 6,
        7 => 115,
        8 => 116,
        _ => 0
    };

    private static PlayerRaceDefinition? GetFallbackRaceDefinition(byte race) => race switch
    {
        1 => new(1, 1, 49, 50, 1.0f, 7, 7),
        2 => new(2, 2, 51, 52, 1.1f, 1, 7),
        3 => new(3, 3, 53, 54, 1.0f, 7, 7),
        4 => new(4, 4, 55, 56, 1.0f, 7, 7),
        5 => new(5, 5, 57, 58, 1.0f, 6, 6),
        6 => new(6, 6, 59, 60, 1.0f, 7, 7),
        7 => new(7, 115, 1563, 1564, 0.8f, 7, 7),
        8 => new(8, 116, 1478, 1479, 1.0f, 7, 7),
        _ => null
    };

    private static byte GetPowerType(byte classId) => classId switch
    {
        1 => 1, // Warrior rage.
        3 => 2, // Hunter focus.
        4 => 3, // Rogue energy.
        _ => 0  // Mana.
    };

    private static uint PackBytes(byte first, byte second, byte third, byte fourth) =>
        (uint)(first | (second << 8) | (third << 16) | (fourth << 24));

    private static uint SingleBits(float value) => unchecked((uint)BitConverter.SingleToInt32Bits(value));
}

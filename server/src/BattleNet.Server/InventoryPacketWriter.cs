using System.Buffers.Binary;
using System.IO;
using BattleNet.Persistence.Models;

namespace BattleNet.Server;

/// <summary>
/// Builds the Alpha item CREATE_OBJECT update used for a character's loaded
/// inventory. Item query/name data is a separate world-DBC boundary; this
/// writer preserves the persistent instance fields and object layout first.
/// </summary>
internal static class InventoryPacketWriter
{
    private const int ItemFieldCount = 36; // ItemFields.END in Alpha Core.
    private const int ItemUpdateMaskBlockCount = (ItemFieldCount + 31) / 32;
    private const ulong ItemHighGuid = 0x4000UL << 48;

    public static byte[] Build(CharacterInventoryRecord item, CharacterSummary owner)
    {
        var fields = BuildFields(item);
        using var stream = new MemoryStream(capacity: 4 + 10 + 68 + 20 + 1 + (ItemUpdateMaskBlockCount * 4) + (fields.Length * 4));
        using var writer = new BinaryWriter(stream);

        writer.Write(1u); // One update transaction containing one item.
        writer.Write((byte)2); // UpdateTypes.CREATE_OBJECT.
        writer.Write(item.Guid | ItemHighGuid);
        writer.Write((byte)1); // ObjectTypeIds.ID_ITEM.
        WriteMovement(writer, owner);
        writer.Write(0u); // Not the current player.
        writer.Write(1u); // Items have an attack cycle field in Alpha.
        writer.Write(0u); // Timer id.
        writer.Write(0UL); // Victim guid.
        writer.Write((byte)ItemUpdateMaskBlockCount);
        writer.Write(uint.MaxValue);
        writer.Write((1u << (ItemFieldCount - 32)) - 1);
        foreach (var field in fields)
            writer.Write(field);

        return stream.ToArray();
    }

    public static byte[] BuildStackCountUpdate(CharacterInventoryRecord item)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(1u);
        writer.Write((byte)0); // UpdateTypes.PARTIAL.
        writer.Write(item.Guid | ItemHighGuid);
        writer.Write((byte)ItemUpdateMaskBlockCount);
        writer.Write(1u << 12); // ITEM_FIELD_STACK_COUNT.
        writer.Write(0u);
        writer.Write(item.StackCount);
        return stream.ToArray();
    }

    public static byte[] BuildDestroy(ulong itemGuid)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(itemGuid | ItemHighGuid);
        return stream.ToArray();
    }

    private static uint[] BuildFields(CharacterInventoryRecord item)
    {
        var fields = new uint[ItemFieldCount];
        var itemGuid = item.Guid | ItemHighGuid;
        var containedGuid = item.Bag == 23 ? item.Owner : 0UL; // SLOT_INBACKPACK.

        SetGuid(fields, 0, itemGuid); // OBJECT_FIELD_GUID.
        fields[2] = 3; // TYPE_OBJECT | TYPE_ITEM.
        fields[3] = item.ItemTemplate;
        fields[4] = SingleBits(1.0f);
        fields[5] = 0;
        SetGuid(fields, 6, item.Owner);
        SetGuid(fields, 8, containedGuid);
        SetGuid(fields, 10, item.Creator);
        fields[12] = item.StackCount;
        fields[13] = item.Duration;
        fields[14] = unchecked((uint)item.SpellCharges1);
        fields[15] = unchecked((uint)item.SpellCharges2);
        fields[16] = unchecked((uint)item.SpellCharges3);
        fields[17] = unchecked((uint)item.SpellCharges4);
        fields[18] = unchecked((uint)item.SpellCharges5);
        fields[19] = item.ItemFlags;

        var enchantments = ParseEnchantments(item.Enchantments);
        for (var index = 0; index < enchantments.Length; index++)
            fields[20 + index] = unchecked((uint)enchantments[index]);
        fields[35] = 0;
        return fields;
    }

    private static int[] ParseEnchantments(string value)
    {
        var result = new int[15];
        var values = value.Split(',', StringSplitOptions.TrimEntries);
        for (var index = 0; index < result.Length && index < values.Length; index++)
            _ = int.TryParse(values[index], out result[index]);
        return result;
    }

    private static void SetGuid(uint[] fields, int index, ulong guid)
    {
        fields[index] = unchecked((uint)guid);
        fields[index + 1] = checked((uint)(guid >> 32));
    }

    private static void WriteMovement(BinaryWriter writer, CharacterSummary owner)
    {
        // Alpha Core ObjectManager._get_movement_fields(). Items use the
        // owner's stationary location and non-unit movement speeds of 1.0.
        writer.Write(0UL);
        writer.Write(0.0f);
        writer.Write(0.0f);
        writer.Write(0.0f);
        writer.Write(0.0f);
        writer.Write(owner.PositionX);
        writer.Write(owner.PositionY);
        writer.Write(owner.PositionZ);
        writer.Write(owner.Orientation);
        writer.Write(0.0f);
        writer.Write(0u);
        writer.Write(0u);
        writer.Write(1.0f);
        writer.Write(1.0f);
        writer.Write(1.0f);
        writer.Write(1.0f);
    }

    private static uint SingleBits(float value) => unchecked((uint)BitConverter.SingleToInt32Bits(value));
}

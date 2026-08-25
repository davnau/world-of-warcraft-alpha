using System.IO;

namespace BattleNet.Server;

/// <summary>
/// Builds the Alpha CREATE_OBJECT update for a player corpse.  The field
/// indexes match CorpseFields in the recovered 0.5.3 client/server sources.
/// </summary>
internal static class CorpseUpdatePacketWriter
{
    private const int CorpseFieldCount = 0x24;
    private const int UpdateMaskBlockCount = (CorpseFieldCount + 31) / 32;
    private const uint CorpseTypeMask = 0x81; // TYPE_OBJECT | TYPE_CORPSE.
    private const byte CorpseObjectTypeId = 7;

    public static byte[] Build(PlayerCorpseRuntime corpse)
    {
        var fields = new uint[CorpseFieldCount];
        SetGuid(fields, 0, corpse.Guid);
        fields[2] = CorpseTypeMask;
        fields[4] = SingleBits(corpse.Scale);
        SetGuid(fields, 6, corpse.OwnerGuid);
        fields[8] = SingleBits(corpse.Orientation);
        fields[9] = SingleBits(corpse.PositionX);
        fields[10] = SingleBits(corpse.PositionY);
        fields[11] = SingleBits(corpse.PositionZ);
        fields[12] = corpse.DisplayId;
        for (var slot = 0; slot < Math.Min(19, corpse.ItemFields.Count); slot++)
            fields[13 + slot] = corpse.ItemFields[slot];
        fields[32] = corpse.Bytes1;
        fields[33] = corpse.Bytes2;
        fields[34] = corpse.GuildId;
        fields[35] = corpse.Level;

        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(1u);
        writer.Write((byte)2); // UpdateTypes.CREATE_OBJECT.
        writer.Write(corpse.Guid);
        writer.Write(CorpseObjectTypeId);
        WriteMovement(writer, corpse);
        writer.Write(0u);
        writer.Write(0u);
        writer.Write(0u);
        writer.Write(0UL);
        writer.Write((byte)UpdateMaskBlockCount);
        writer.Write(uint.MaxValue);
        writer.Write((1u << (CorpseFieldCount - 32)) - 1u);
        foreach (var field in fields)
            writer.Write(field);
        return stream.ToArray();
    }

    public static byte[] BuildDestroy(ulong corpseGuid)
    {
        using var stream = new MemoryStream(sizeof(ulong));
        using var writer = new BinaryWriter(stream);
        writer.Write(corpseGuid);
        return stream.ToArray();
    }

    private static void WriteMovement(BinaryWriter writer, PlayerCorpseRuntime corpse)
    {
        writer.Write(0UL);
        writer.Write(0.0f);
        writer.Write(0.0f);
        writer.Write(0.0f);
        writer.Write(0.0f);
        writer.Write(corpse.PositionX);
        writer.Write(corpse.PositionY);
        writer.Write(corpse.PositionZ);
        writer.Write(corpse.Orientation);
        writer.Write(0.0f);
        writer.Write(0u);
        writer.Write(0u);
        writer.Write(2.5f);
        writer.Write(7.0f);
        writer.Write(4.72f);
        writer.Write(MathF.PI);
    }

    private static void SetGuid(uint[] fields, int field, ulong guid)
    {
        fields[field] = unchecked((uint)guid);
        fields[field + 1] = unchecked((uint)(guid >> 32));
    }

    private static uint SingleBits(float value) =>
        unchecked((uint)BitConverter.SingleToInt32Bits(value));
}

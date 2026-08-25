using System.Buffers;
using System.Buffers.Binary;
using System.Text;
using BattleNet.Persistence.Models;

namespace BattleNet.Server;

internal static class CharacterEnumPacketWriter
{
    private const int EquipmentSlotCount = 20;

    public static byte[] Build(IReadOnlyList<CharacterSummary> characters)
    {
        if (characters.Count > byte.MaxValue)
            throw new InvalidOperationException("The character enumeration packet cannot contain more than 255 characters.");

        var writer = new ArrayBufferWriter<byte>();
        WriteByte(writer, (byte)characters.Count);
        foreach (var character in characters)
            WriteCharacter(writer, character);
        return writer.WrittenSpan.ToArray();
    }

    private static void WriteCharacter(IBufferWriter<byte> writer, CharacterSummary character)
    {
        WriteUInt64(writer, character.Guid);
        WriteCString(writer, character.Name);
        WriteByte(writer, character.Race);
        WriteByte(writer, character.Class);
        WriteByte(writer, character.Gender);
        WriteByte(writer, character.Skin);
        WriteByte(writer, character.Face);
        WriteByte(writer, character.HairStyle);
        WriteByte(writer, character.HairColor);
        WriteByte(writer, character.FacialHair);
        WriteByte(writer, character.Level);
        WriteUInt32(writer, character.Zone);
        WriteUInt32(writer, character.Map);
        WriteSingle(writer, character.PositionX);
        WriteSingle(writer, character.PositionY);
        WriteSingle(writer, character.PositionZ);
        WriteUInt32(writer, character.GuildId);
        WriteUInt32(writer, character.PetDisplayId);
        WriteUInt32(writer, character.PetLevel);
        WriteUInt32(writer, character.PetFamily);

        // The Alpha Core packet contains display-id/inventory-type pairs for
        // equipment slots 0 through 19. Item-template/DBC lookup is a separate
        // world-data port; zeroes are the correct empty-slot representation.
        for (var slot = 0; slot < EquipmentSlotCount; slot++)
        {
            WriteUInt32(writer, 0);
            WriteByte(writer, 0);
        }
    }

    private static void WriteCString(IBufferWriter<byte> writer, string value)
    {
        var bytes = Encoding.UTF8.GetBytes(value);
        writer.Write(bytes);
        WriteByte(writer, 0);
    }

    private static void WriteByte(IBufferWriter<byte> writer, byte value)
    {
        writer.GetSpan(1)[0] = value;
        writer.Advance(1);
    }

    private static void WriteUInt32(IBufferWriter<byte> writer, uint value)
    {
        BinaryPrimitives.WriteUInt32LittleEndian(writer.GetSpan(sizeof(uint)), value);
        writer.Advance(sizeof(uint));
    }

    private static void WriteUInt64(IBufferWriter<byte> writer, ulong value)
    {
        BinaryPrimitives.WriteUInt64LittleEndian(writer.GetSpan(sizeof(ulong)), value);
        writer.Advance(sizeof(ulong));
    }

    private static void WriteSingle(IBufferWriter<byte> writer, float value) =>
        WriteUInt32(writer, unchecked((uint)BitConverter.SingleToInt32Bits(value)));
}

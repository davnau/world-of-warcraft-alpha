using System.Buffers.Binary;
using BattleNet.Persistence.Models;
using BattleNet.Protocol;

namespace BattleNet.Server;

internal static class NameQueryPacketWriter
{
    public static byte[] Build(CharacterSummary character)
    {
        var name = PacketWriter.StringToBytes(character.Name);
        var payload = new byte[sizeof(ulong) + name.Length + (sizeof(uint) * 3)];
        BinaryPrimitives.WriteUInt64LittleEndian(payload.AsSpan(0, sizeof(ulong)), character.Guid);
        name.CopyTo(payload.AsSpan(sizeof(ulong)));
        var offset = sizeof(ulong) + name.Length;
        BinaryPrimitives.WriteUInt32LittleEndian(payload.AsSpan(offset, sizeof(uint)), character.Race);
        BinaryPrimitives.WriteUInt32LittleEndian(payload.AsSpan(offset + sizeof(uint), sizeof(uint)), character.Gender);
        BinaryPrimitives.WriteUInt32LittleEndian(payload.AsSpan(offset + (sizeof(uint) * 2), sizeof(uint)), character.Class);
        return payload;
    }
}

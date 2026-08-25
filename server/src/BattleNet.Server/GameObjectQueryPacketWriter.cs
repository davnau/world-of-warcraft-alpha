using System.Buffers.Binary;
using BattleNet.Persistence.Models;
using BattleNet.Protocol;

namespace BattleNet.Server;

internal static class GameObjectQueryPacketWriter
{
    public static byte[] Build(WorldGameObjectTemplate template)
    {
        var name = PacketWriter.StringToBytes(template.Name);
        var payload = new byte[sizeof(uint) * 3 + name.Length + 3 + sizeof(uint) * 10];
        var offset = 0;
        WriteUInt(payload, ref offset, template.Entry);
        WriteUInt(payload, ref offset, template.Type);
        WriteUInt(payload, ref offset, template.DisplayId);
        name.CopyTo(payload, offset);
        offset += name.Length + 3;
        foreach (var value in template.Data)
            WriteUInt(payload, ref offset, value);
        return payload;
    }

    private static void WriteUInt(byte[] payload, ref int offset, uint value)
    {
        BinaryPrimitives.WriteUInt32LittleEndian(payload.AsSpan(offset, sizeof(uint)), value);
        offset += sizeof(uint);
    }
}

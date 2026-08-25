using System.Buffers.Binary;
using System.Text;
using BattleNet.Persistence.Models;
using BattleNet.Protocol;

namespace BattleNet.Server;

internal static class CreatureQueryPacketWriter
{
    public static byte[] Build(WorldCreatureTemplate template)
    {
        var name = PacketWriter.StringToBytes(template.Name);
        var subName = PacketWriter.StringToBytes(template.SubName);
        var payload = new byte[sizeof(uint) + name.Length + 3 + subName.Length + sizeof(uint) * 3];
        var offset = 0;
        BinaryPrimitives.WriteUInt32LittleEndian(payload.AsSpan(offset, sizeof(uint)), template.Entry);
        offset += sizeof(uint);
        name.CopyTo(payload, offset);
        offset += name.Length;
        offset += 3;
        subName.CopyTo(payload, offset);
        offset += subName.Length;
        BinaryPrimitives.WriteUInt32LittleEndian(payload.AsSpan(offset, sizeof(uint)), template.StaticFlags);
        offset += sizeof(uint);
        BinaryPrimitives.WriteUInt32LittleEndian(payload.AsSpan(offset, sizeof(uint)), template.Type);
        offset += sizeof(uint);
        BinaryPrimitives.WriteUInt32LittleEndian(payload.AsSpan(offset, sizeof(uint)), template.BeastFamily);
        return payload;
    }
}

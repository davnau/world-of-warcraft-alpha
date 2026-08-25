using System.Buffers.Binary;
using System.IO.Compression;
using System.Text;

namespace BattleNet.Protocol;

public static class PacketWriter
{
    public const int HeaderSize = 6;
    public const int MaximumPacketSize = 0x8000;

    public static byte[] BuildPacket(Opcode opcode, ReadOnlySpan<byte> payload = default)
    {
        var packetLength = 4 + payload.Length;
        if (packetLength > ushort.MaxValue)
            throw new ArgumentOutOfRangeException(nameof(payload), "BattleNet packet exceeds the two-byte header limit.");

        var packet = new byte[2 + packetLength];
        BinaryPrimitives.WriteUInt16BigEndian(packet.AsSpan(0, 2), checked((ushort)packetLength));
        BinaryPrimitives.WriteUInt32LittleEndian(packet.AsSpan(2, 4), (uint)opcode);
        payload.CopyTo(packet.AsSpan(HeaderSize));

        if (opcode == Opcode.SMSG_UPDATE_OBJECT && packet.Length > 100)
        {
            var compressedPayload = CompressZlib(packet.AsSpan(HeaderSize));
            var compressedData = new byte[4 + compressedPayload.Length];
            BinaryPrimitives.WriteUInt32LittleEndian(compressedData.AsSpan(0, 4), checked((uint)(packet.Length - HeaderSize)));
            compressedPayload.CopyTo(compressedData.AsSpan(4));
            return BuildPacket(Opcode.SMSG_COMPRESSED_UPDATE_OBJECT, compressedData);
        }

        return packet;
    }

    public static byte[] BuildSrpPacket(ReadOnlySpan<byte> payload)
    {
        if (payload.Length > ushort.MaxValue)
            throw new ArgumentOutOfRangeException(nameof(payload));

        var packet = new byte[2 + payload.Length];
        BinaryPrimitives.WriteUInt16BigEndian(packet.AsSpan(0, 2), checked((ushort)payload.Length));
        payload.CopyTo(packet.AsSpan(2));
        return packet;
    }

    public static byte[] StringToBytes(string? value)
    {
        var text = value ?? string.Empty;
        var encoded = Encoding.Latin1.GetBytes(text);
        var result = new byte[encoded.Length + 1];
        encoded.CopyTo(result, 0);
        return result;
    }

    private static byte[] CompressZlib(ReadOnlySpan<byte> data)
    {
        using var output = new MemoryStream();
        using (var zlib = new ZLibStream(output, CompressionLevel.SmallestSize, leaveOpen: true))
            zlib.Write(data);
        return output.ToArray();
    }
}

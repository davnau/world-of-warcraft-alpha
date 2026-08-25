using System.Buffers.Binary;
using System.Text;

namespace BattleNet.Protocol;

public static class PacketReader
{
    public const int WorldHeaderSize = 6;
    public const int MaximumWorldPayload = 0x4000;

    public static async ValueTask<PacketFrame?> ReadWorldAsync(
        Stream stream,
        int maximumPayload = MaximumWorldPayload,
        CancellationToken cancellationToken = default)
    {
        var header = new byte[WorldHeaderSize];
        if (!await ReadExactlyAsync(stream, header, cancellationToken).ConfigureAwait(false))
            return null;

        var declaredSize = BinaryPrimitives.ReadUInt16BigEndian(header.AsSpan(0, 2));
        var payloadLength = declaredSize - 4;
        if (payloadLength < 0 || payloadLength > maximumPayload)
            throw new InvalidDataException($"Invalid BattleNet packet size {payloadLength}.");

        var opcode = (Opcode)BinaryPrimitives.ReadUInt32LittleEndian(header.AsSpan(2, 4));
        var payload = new byte[payloadLength];
        if (payloadLength > 0 && !await ReadExactlyAsync(stream, payload, cancellationToken).ConfigureAwait(false))
            return null;

        return new PacketFrame(opcode, payload);
    }

    public static async ValueTask<byte[]?> ReadSrpFrameAsync(
        Stream stream,
        int maximumPayload = 4096,
        CancellationToken cancellationToken = default)
    {
        var header = new byte[2];
        if (!await ReadExactlyAsync(stream, header, cancellationToken).ConfigureAwait(false))
            return null;

        var length = BinaryPrimitives.ReadUInt16BigEndian(header);
        if (length > maximumPayload)
            throw new InvalidDataException($"Invalid SRP6 packet size {length}.");

        var payload = new byte[length];
        return length == 0 || await ReadExactlyAsync(stream, payload, cancellationToken).ConfigureAwait(false)
            ? payload
            : null;
    }

    public static string ReadCString(ReadOnlySpan<byte> payload, int start, Encoding? encoding = null)
    {
        if ((uint)start >= (uint)payload.Length)
            return string.Empty;

        var end = payload[start..].IndexOf((byte)0);
        if (end < 0)
            end = payload.Length - start;

        return (encoding ?? Encoding.Latin1).GetString(payload.Slice(start, end));
    }

    public static int CStringEnd(ReadOnlySpan<byte> payload, int start)
    {
        if ((uint)start >= (uint)payload.Length)
            return -1;
        var end = payload[start..].IndexOf((byte)0);
        return end < 0 ? -1 : start + end + 1;
    }

    private static async ValueTask<bool> ReadExactlyAsync(
        Stream stream,
        Memory<byte> buffer,
        CancellationToken cancellationToken)
    {
        var offset = 0;
        while (offset < buffer.Length)
        {
            var read = await stream.ReadAsync(buffer[offset..], cancellationToken).ConfigureAwait(false);
            if (read == 0)
                return false;
            offset += read;
        }
        return true;
    }
}

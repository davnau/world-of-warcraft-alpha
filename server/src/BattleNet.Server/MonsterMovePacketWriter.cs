using System.IO;

namespace BattleNet.Server;

internal static class MonsterMovePacketWriter
{
    public static byte[] Build(
        ulong guid,
        float startX,
        float startY,
        float startZ,
        float destinationX,
        float destinationY,
        float destinationZ,
        uint durationMs)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(guid);
        writer.Write(startX);
        writer.Write(startY);
        writer.Write(startZ);
        writer.Write(0u); // Spline timestamp.
        writer.Write((byte)0); // Spline type: normal.
        writer.Write(0u); // Spline flags.
        writer.Write(durationMs);
        writer.Write(1u); // One destination point.
        writer.Write(destinationX);
        writer.Write(destinationY);
        writer.Write(destinationZ);
        return stream.ToArray();
    }
}

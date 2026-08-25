using System.IO;

namespace BattleNet.Server;

internal static class MovementPacketWriter
{
    public static byte[] Build(
        ulong guid,
        float positionX,
        float positionY,
        float positionZ,
        float orientation,
        uint movementFlags = 0)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(guid);
        writer.Write(0UL); // Transport guid.
        writer.Write(0.0f);
        writer.Write(0.0f);
        writer.Write(0.0f);
        writer.Write(0.0f);
        writer.Write(positionX);
        writer.Write(positionY);
        writer.Write(positionZ);
        writer.Write(orientation);
        writer.Write(0.0f); // Pitch.
        writer.Write(movementFlags);
        return stream.ToArray();
    }
}

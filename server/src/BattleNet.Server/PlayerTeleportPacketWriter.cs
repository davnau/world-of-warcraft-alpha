using System.IO;

namespace BattleNet.Server;

internal static class PlayerTeleportPacketWriter
{
    // Recovered MSG_MOVE_TELEPORT_ACK payload: <Q9fI>.  It intentionally has
    // no mover GUID; the client replies with the bare opcode after placement.
    public static byte[] Build(
        float positionX,
        float positionY,
        float positionZ,
        float orientation,
        uint movementFlags = 0)
    {
        using var stream = new MemoryStream(48);
        using var writer = new BinaryWriter(stream);
        writer.Write(0UL); // Transport GUID.
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

using System.IO;

namespace BattleNet.Server;

internal static class SpellPacketWriter
{
    public static byte[] BuildCastResult(uint spellId, bool success, byte reason = 0)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(spellId);
        writer.Write((byte)(success ? 0 : 2)); // SpellCastStatus.CAST_SUCCESS / CAST_FAILED.
        if (!success)
            writer.Write(reason);
        return stream.ToArray();
    }

    public static byte[] BuildFailure(ulong casterGuid, uint spellId, byte reason)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(casterGuid);
        writer.Write(spellId);
        writer.Write(reason);
        return stream.ToArray();
    }

    public static byte[] BuildAuraDuration(byte auraIndex, int durationMs)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(auraIndex);
        writer.Write(durationMs);
        return stream.ToArray();
    }

    public static byte[] BuildCooldown(uint spellId, ulong casterGuid, int cooldownMs)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(spellId);
        writer.Write(casterGuid);
        writer.Write((ushort)Math.Clamp(cooldownMs, 0, ushort.MaxValue));
        return stream.ToArray();
    }

    public static byte[] BuildStart(
        ulong sourceGuid,
        ulong casterGuid,
        uint spellId,
        ushort castFlags,
        int castTimeMs,
        ushort targetMask,
        ulong targetGuid,
        float sourceX,
        float sourceY,
        float sourceZ,
        float destinationX,
        float destinationY,
        float destinationZ)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(sourceGuid);
        writer.Write(casterGuid);
        writer.Write(spellId);
        writer.Write(castFlags);
        writer.Write(castTimeMs);
        writer.Write(targetMask);
        WriteTargetInfo(writer, targetMask, targetGuid, sourceX, sourceY, sourceZ, destinationX, destinationY, destinationZ);
        return stream.ToArray();
    }

    public static byte[] BuildGo(
        ulong sourceGuid,
        ulong casterGuid,
        uint spellId,
        ushort castFlags,
        ushort targetMask,
        ulong targetGuid,
        float sourceX,
        float sourceY,
        float sourceZ,
        float destinationX,
        float destinationY,
        float destinationZ,
        bool hit = true)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(sourceGuid);
        writer.Write(casterGuid);
        writer.Write(spellId);
        writer.Write(castFlags);
        writer.Write((byte)(hit ? 1 : 0));
        if (hit)
            writer.Write(targetGuid);
        writer.Write((byte)(hit ? 0 : 1));
        if (!hit)
        {
            writer.Write((byte)1); // SPELL_MISS_REASON_MISS.
            writer.Write(targetGuid);
        }
        writer.Write(targetMask);
        WriteTargetInfo(writer, targetMask, targetGuid, sourceX, sourceY, sourceZ, destinationX, destinationY, destinationZ);
        return stream.ToArray();
    }

    private static void WriteTargetInfo(
        BinaryWriter writer,
        ushort targetMask,
        ulong targetGuid,
        float sourceX,
        float sourceY,
        float sourceZ,
        float destinationX,
        float destinationY,
        float destinationZ)
    {
        if ((targetMask & 0x0802) != 0)
            writer.Write(targetGuid);
        if ((targetMask & 0x1010) != 0)
            writer.Write(0UL);
        if ((targetMask & 0x0020) != 0)
        {
            writer.Write(sourceX);
            writer.Write(sourceY);
            writer.Write(sourceZ);
        }
        if ((targetMask & 0x0040) != 0)
        {
            writer.Write(destinationX);
            writer.Write(destinationY);
            writer.Write(destinationZ);
        }
    }
}

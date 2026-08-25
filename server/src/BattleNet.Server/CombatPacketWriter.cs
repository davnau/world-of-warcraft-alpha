using System.Buffers.Binary;

namespace BattleNet.Server;

internal static class CombatPacketWriter
{
    public static byte[] BuildAttackStart(ulong attackerGuid, ulong victimGuid)
    {
        var payload = new byte[sizeof(ulong) * 2];
        BinaryPrimitives.WriteUInt64LittleEndian(payload, attackerGuid);
        BinaryPrimitives.WriteUInt64LittleEndian(payload.AsSpan(sizeof(ulong)), victimGuid);
        return payload;
    }

    public static byte[] BuildAttackStop(ulong attackerGuid, ulong victimGuid, bool attackerDead = false)
    {
        var payload = new byte[(sizeof(ulong) * 2) + sizeof(uint)];
        BinaryPrimitives.WriteUInt64LittleEndian(payload, attackerGuid);
        BinaryPrimitives.WriteUInt64LittleEndian(payload.AsSpan(sizeof(ulong)), victimGuid);
        BinaryPrimitives.WriteUInt32LittleEndian(payload.AsSpan(sizeof(ulong) * 2), attackerDead ? 1u : 0u);
        return payload;
    }

    public static byte[] BuildAttackerStateUpdate(
        ulong attackerGuid,
        ulong victimGuid,
        uint damage)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(0u); // HitInfo.DAMAGE.
        writer.Write(attackerGuid);
        writer.Write(victimGuid);
        writer.Write(damage);
        writer.Write((byte)1); // Sub damage count.
        writer.Write(0u); // Physical school mask.
        writer.Write((float)damage);
        writer.Write(damage);
        writer.Write(0u); // Absorb.
        writer.Write(0u); // Victim state.
        writer.Write(0u); // Victim round duration.
        writer.Write(0u); // Spell damage added.
        writer.Write(0u); // Spell added damage.
        writer.Write(0); // Proc victim spell.
        return stream.ToArray();
    }

    public static byte[] BuildDamageDone(
        ulong targetGuid,
        uint damage,
        ulong attackerGuid,
        uint spellId = 0)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(targetGuid);
        writer.Write(damage);
        writer.Write(spellId == 0 ? damage : 0u); // White versus yellow combat text.
        writer.Write(0); // WorldTextFlags.NORMAL_DAMAGE.
        writer.Write(spellId);
        writer.Write(attackerGuid);
        return stream.ToArray();
    }

    public static byte[] BuildAiReaction(ulong creatureGuid, uint reaction)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(creatureGuid);
        writer.Write(reaction);
        return stream.ToArray();
    }
}

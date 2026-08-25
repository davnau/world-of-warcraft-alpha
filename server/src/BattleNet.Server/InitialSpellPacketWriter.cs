using System.Buffers.Binary;
using BattleNet.Persistence.Models;

namespace BattleNet.Server;

/// <summary>
/// Builds Alpha Core's SMSG_INITIAL_SPELLS payload.
/// </summary>
internal static class InitialSpellPacketWriter
{
    public static byte[] Build(
        IReadOnlyList<CharacterSpellRecord> spells,
        IReadOnlyDictionary<uint, int> spellButtons)
    {
        if (spells.Count > ushort.MaxValue)
            throw new ArgumentOutOfRangeException(nameof(spells), "Alpha spell count exceeds the packet field.");

        var activeCount = spells.Count(spell => spell.Active);
        var payload = new byte[1 + sizeof(ushort) + (activeCount * (sizeof(short) * 2)) + sizeof(ushort)];
        payload[0] = 0; // Cooldown/global spell state.
        BinaryPrimitives.WriteUInt16LittleEndian(payload.AsSpan(1, sizeof(ushort)), checked((ushort)spells.Count));

        var offset = 1 + sizeof(ushort);
        foreach (var spell in spells)
        {
            // Alpha serializes these as signed 16-bit values, despite the DB
            // columns being integer-sized. Alpha spell IDs fit this wire field.
            if (!spell.Active)
                continue;

            BinaryPrimitives.WriteInt16LittleEndian(
                payload.AsSpan(offset, sizeof(short)),
                unchecked((short)spell.SpellId));
            var button = spellButtons.TryGetValue(spell.SpellId, out var index) ? index : 0;
            BinaryPrimitives.WriteInt16LittleEndian(
                payload.AsSpan(offset + sizeof(short), sizeof(short)),
                unchecked((short)button));
            offset += sizeof(short) * 2;
        }

        BinaryPrimitives.WriteUInt16LittleEndian(payload.AsSpan(offset, sizeof(ushort)), 0);
        return payload;
    }
}

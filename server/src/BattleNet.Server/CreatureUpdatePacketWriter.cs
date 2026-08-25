using System.Buffers.Binary;
using BattleNet.Persistence.Models;

namespace BattleNet.Server;

/// <summary>
/// Sends a basic Alpha CREATE_OBJECT update for a database creature spawn.
/// Combat AI and dynamic field updates are separate systems; this establishes
/// the visible NPC/object foundation used by queries and quest givers.
/// </summary>
internal static class CreatureUpdatePacketWriter
{
    private const int UnitFieldCount = 184; // UnitFields.END in Alpha Core.
    private const int UpdateMaskBlockCount = (UnitFieldCount + 31) / 32;
    private const uint CreatureTypeMask = 9; // TYPE_OBJECT | TYPE_UNIT.
    private const byte CreatureObjectTypeId = 3;
    private const ulong HighGuidUnit = 0xF130000000000000UL;

    public static byte[] Build(WorldCreatureSpawn spawn, WorldCreatureTemplate template)
    {
        var fields = new uint[UnitFieldCount];
        var guid = HighGuidUnit | spawn.SpawnId;
        var level = template.LevelMax >= template.LevelMin
            ? Math.Max(1u, template.LevelMin)
            : Math.Max(1u, template.LevelMax);
        if (template.LevelMax > template.LevelMin)
            level = template.LevelMin + ((spawn.SpawnId % (template.LevelMax - template.LevelMin + 1)));
        var health = 100u;

        fields[0] = unchecked((uint)guid);
        fields[1] = unchecked((uint)(guid >> 32));
        fields[2] = CreatureTypeMask;
        fields[3] = template.Entry;
        fields[4] = SingleBits(1.0f);
        fields[0x16] = health;
        fields[0x1B] = health;
        fields[0x20] = level;
        fields[0x21] = template.Faction;
        fields[0x22] = 0;
        fields[0x36] = 0;
        fields[0x8C] = 2000;
        fields[0x8D] = 2000;
        fields[0x94] = SingleBits(0.306f);
        fields[0x95] = SingleBits(1.5f);
        fields[0x96] = SingleBits(1.5f);
        fields[0x97] = template.DisplayId;
        fields[0x99] = 1;
        // UNIT_FIELD_BYTES_1 stores NPC flags in its third byte in Alpha:
        // sheath state, shapeshift form, NPC flags, stand state.
        fields[0xAC] = template.NpcFlags << 8;
        fields[0xB1] = 0;
        fields[0xB2] = 0;
        fields[0xB3] = 0;
        fields[0xB4] = 0;
        fields[0xB6] = 0;

        using var stream = new MemoryStream(capacity: 4 + 10 + 68 + 20 + 1 + (UpdateMaskBlockCount * 4) + (fields.Length * 4));
        using var writer = new BinaryWriter(stream);
        writer.Write(1u);
        writer.Write((byte)2); // UpdateTypes.CREATE_OBJECT.
        writer.Write(guid);
        writer.Write(CreatureObjectTypeId);
        WriteMovement(writer, spawn);
        writer.Write(0u); // CREATE_OBJECT flags.
        writer.Write(0u); // Attack cycle.
        writer.Write(0u); // Timer id.
        writer.Write(0UL); // Victim guid.
        writer.Write((byte)UpdateMaskBlockCount);
        for (var block = 0; block < UpdateMaskBlockCount; block++)
        {
            var mask = uint.MaxValue;
            if (block == UpdateMaskBlockCount - 1)
                mask = (1u << (UnitFieldCount % 32)) - 1;
            writer.Write(mask);
        }
        foreach (var field in fields)
            writer.Write(field);
        return stream.ToArray();
    }

    public static byte[] BuildHealthUpdate(ulong guid, uint health, uint maxHealth)
    {
        const uint mask = (1u << 0x16) | (1u << 0x1B); // UNIT_FIELD_HEALTH and MAXHEALTH.
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(1u);
        writer.Write((byte)0); // UpdateTypes.PARTIAL.
        writer.Write(guid);
        writer.Write((byte)UpdateMaskBlockCount);
        writer.Write(mask);
        for (var block = 1; block < UpdateMaskBlockCount; block++)
            writer.Write(0u);
        writer.Write(health);
        writer.Write(maxHealth);
        return stream.ToArray();
    }

    private static void WriteMovement(BinaryWriter writer, WorldCreatureSpawn spawn)
    {
        writer.Write(0UL);
        writer.Write(0.0f);
        writer.Write(0.0f);
        writer.Write(0.0f);
        writer.Write(0.0f);
        writer.Write(spawn.PositionX);
        writer.Write(spawn.PositionY);
        writer.Write(spawn.PositionZ);
        writer.Write(spawn.Orientation);
        writer.Write(0.0f);
        writer.Write(0u);
        writer.Write(0u);
        writer.Write(2.5f);
        writer.Write(7.0f);
        writer.Write(4.72f);
        writer.Write(MathF.PI);
    }

    private static uint SingleBits(float value) => unchecked((uint)BitConverter.SingleToInt32Bits(value));
}

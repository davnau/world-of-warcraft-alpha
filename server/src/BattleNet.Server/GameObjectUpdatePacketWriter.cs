using BattleNet.Persistence.Models;

namespace BattleNet.Server;

internal static class GameObjectUpdatePacketWriter
{
    private const int GameObjectFieldCount = 20; // ObjectFields.END + GameObjectFields.END.
    private const int UpdateMaskBlockCount = (GameObjectFieldCount + 31) / 32;
    private const uint GameObjectTypeMask = 33; // TYPE_OBJECT | TYPE_GAMEOBJECT.
    private const byte GameObjectTypeId = 5;
    private const ulong HighGuidGameObject = 0xF110000000000000UL;

    public static byte[] Build(WorldGameObjectSpawn spawn, WorldGameObjectTemplate template)
    {
        var fields = new uint[GameObjectFieldCount];
        var guid = HighGuidGameObject | spawn.SpawnId;
        var rotation2 = spawn.Rotation2;
        var rotation3 = spawn.Rotation3;
        if (rotation2 == 0 && rotation3 == 0)
        {
            rotation2 = MathF.Sin(spawn.Orientation / 2.0f);
            rotation3 = MathF.Cos(spawn.Orientation / 2.0f);
        }

        fields[0] = unchecked((uint)guid);
        fields[1] = unchecked((uint)(guid >> 32));
        fields[2] = GameObjectTypeMask;
        fields[3] = template.Entry;
        fields[4] = SingleBits(template.Scale > 0 ? template.Scale : 1.0f);
        fields[6] = template.DisplayId;
        fields[7] = template.Flags;
        fields[8] = SingleBits(spawn.Rotation0);
        fields[9] = SingleBits(spawn.Rotation1);
        fields[10] = SingleBits(rotation2);
        fields[11] = SingleBits(rotation3);
        fields[12] = spawn.State;
        fields[14] = SingleBits(spawn.PositionX);
        fields[15] = SingleBits(spawn.PositionY);
        fields[16] = SingleBits(spawn.PositionZ);
        fields[17] = SingleBits(spawn.Orientation);
        fields[19] = template.Faction;

        using var stream = new MemoryStream(capacity: 4 + 1 + 8 + 1 + 64 + 20 + 4 + (UpdateMaskBlockCount * 4) + fields.Length * 4);
        using var writer = new BinaryWriter(stream);
        writer.Write(1u);
        writer.Write((byte)2); // UpdateTypes.CREATE_OBJECT.
        writer.Write(guid);
        writer.Write(GameObjectTypeId);
        WriteMovement(writer, spawn);
        writer.Write(0u); // CREATE_OBJECT flags.
        writer.Write(0u); // Attack cycle.
        writer.Write(0u); // Timer id.
        writer.Write(0UL); // Victim guid.
        writer.Write((byte)UpdateMaskBlockCount);
        writer.Write(uint.MaxValue & ((1u << GameObjectFieldCount) - 1));
        foreach (var field in fields)
            writer.Write(field);
        return stream.ToArray();
    }

    private static void WriteMovement(BinaryWriter writer, WorldGameObjectSpawn spawn)
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
        writer.Write(1.0f);
        writer.Write(1.0f);
        writer.Write(1.0f);
        writer.Write(1.0f);
    }

    private static uint SingleBits(float value) => unchecked((uint)BitConverter.SingleToInt32Bits(value));
}

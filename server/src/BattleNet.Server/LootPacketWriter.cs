using System.IO;

namespace BattleNet.Server;

internal static class LootPacketWriter
{
    public static byte[] BuildResponse(
        ulong objectGuid,
        uint money,
        IReadOnlyList<LootRuntimeItem> items)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(objectGuid);
        writer.Write((byte)1); // LOOT_TYPE_CORPSE.
        writer.Write(money);
        var visible = items.Where(item => !item.Claimed).ToArray();
        writer.Write((byte)Math.Min(byte.MaxValue, visible.Length));
        for (var index = 0; index < visible.Length && index < byte.MaxValue; index++)
        {
            var item = visible[index];
            writer.Write((byte)index);
            writer.Write(item.ItemEntry);
            writer.Write(item.Count);
            writer.Write(item.DisplayId);
        }
        return stream.ToArray();
    }

    public static byte[] BuildRelease(ulong objectGuid)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(objectGuid);
        writer.Write((byte)1);
        return stream.ToArray();
    }

    public static byte[] BuildError(ulong objectGuid, byte error)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(objectGuid);
        writer.Write((byte)0); // LOOT_TYPE_NOTALLOWED.
        writer.Write(error);
        return stream.ToArray();
    }

    public static byte[] BuildRemoved(byte slot) => new[] { slot };

    public static byte[] BuildMoneyNotify(uint money) => BitConverter.GetBytes(money);

    public static byte[] BuildItemPush(
        ulong itemGuid,
        uint itemEntry,
        byte bagSlot,
        bool created = true,
        bool showInChat = true)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(itemGuid);
        writer.Write(created ? 0u : 1u);
        writer.Write(showInChat ? 1u : 0u);
        writer.Write(bagSlot == 23 ? (byte)0xFF : bagSlot);
        writer.Write(itemEntry);
        return stream.ToArray();
    }
}

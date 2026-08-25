using System.Buffers.Binary;
using BattleNet.Persistence.Models;
using BattleNet.Protocol;

namespace BattleNet.Server;

internal static class QuestGiverPacketWriters
{
    public const uint None = 0;
    public const uint Trivial = 1;
    public const uint Future = 2;
    public const uint Reward = 3;
    public const uint Quest = 4;

    public static byte[] BuildStatus(ulong guid, uint status)
    {
        var payload = new byte[sizeof(ulong) + sizeof(uint)];
        BinaryPrimitives.WriteUInt64LittleEndian(payload, guid);
        BinaryPrimitives.WriteUInt32LittleEndian(payload.AsSpan(sizeof(ulong)), status);
        return payload;
    }

    public static byte[] BuildList(
        ulong guid,
        string message,
        uint emote,
        IReadOnlyList<WorldQuestGiverQuest> quests)
    {
        var payload = new List<byte>(64 + quests.Count * 32);
        WriteUInt64(payload, guid);
        WriteString(payload, message, maxBytes: 256);
        WriteUInt(payload, 0); // Emote delay.
        WriteUInt(payload, emote);
        payload.Add(checked((byte)Math.Min(byte.MaxValue, quests.Count)));

        foreach (var quest in quests.Take(byte.MaxValue))
        {
            WriteUInt(payload, quest.Entry);
            WriteUInt(payload, quest.QuestGiverState);
            WriteUInt(payload, quest.QuestLevel);
            WriteString(payload, quest.Title);
        }

        return payload.ToArray();
    }

    public static byte[] BuildDetails(
        ulong guid,
        WorldQuestTemplate quest,
        bool activateAccept = true)
    {
        var payload = new List<byte>(512);
        WriteUInt64(payload, guid);
        WriteUInt(payload, quest.Entry);
        WriteString(payload, quest.Title);
        WriteString(payload, quest.Details);
        WriteString(payload, quest.Objectives);
        WriteUInt(payload, activateAccept ? 1u : 0u);

        var choiceItems = PairNonZero(quest.RewardChoiceItemIds, quest.RewardChoiceItemCounts);
        WriteUInt(payload, checked((uint)choiceItems.Count));
        foreach (var item in choiceItems)
            WriteItemStruct(payload, item.ItemId, item.Count, 0);

        var rewardItems = PairNonZero(quest.RewardItemIds, quest.RewardItemCounts);
        WriteUInt(payload, checked((uint)rewardItems.Count));
        foreach (var item in rewardItems)
            WriteItemStruct(payload, item.ItemId, item.Count, 0);

        WriteUInt(payload, unchecked((uint)quest.RewardOrRequiredMoney));
        WriteUInt(payload, 4); // Details emote count in Alpha.
        for (var index = 0; index < 4; index++)
        {
            WriteUInt(payload, 0); // Emote.
            WriteUInt(payload, 0); // Delay.
        }

        return payload.ToArray();
    }

    public static byte[] BuildRequestItems(
        ulong guid,
        WorldQuestTemplate quest,
        bool complete,
        bool closeOnCancel = false)
    {
        var payload = new List<byte>(256);
        WriteUInt64(payload, guid);
        WriteUInt(payload, quest.Entry);
        WriteString(payload, quest.Title);
        WriteString(payload, quest.RequestItemsText);
        WriteUInt(payload, 0); // Emote delay.
        WriteUInt(payload, complete ? quest.CompleteEmote : quest.IncompleteEmote);
        WriteUInt(payload, closeOnCancel ? 1u : 0u);

        var requirements = quest.Requirements
            .Where(requirement => requirement.ItemId > 0)
            .ToArray();
        WriteUInt(payload, checked((uint)requirements.Length));
        foreach (var requirement in requirements)
            WriteItemStruct(payload, requirement.ItemId, requirement.ItemCount, 0);

        WriteUInt(payload, 1); // General requirements met.
        WriteUInt(payload, complete ? 1u : 0u);
        WriteUInt(payload, 1); // Reputation requirements met.
        return payload.ToArray();
    }

    public static byte[] BuildOfferReward(ulong guid, WorldQuestTemplate quest, bool enableNext = true)
    {
        var payload = new List<byte>(256);
        WriteUInt64(payload, guid);
        WriteUInt(payload, quest.Entry);
        WriteString(payload, quest.Title);
        WriteString(payload, quest.OfferRewardText);
        WriteUInt(payload, enableNext ? 1u : 0u);
        WriteUInt(payload, 4); // Emote count.
        for (var index = 0; index < 4; index++)
        {
            WriteUInt(payload, 0); // Delay.
            WriteUInt(payload, 0); // Emote.
        }
        return payload.ToArray();
    }

    public static byte[] BuildCompleteNoRewards(uint questId)
    {
        var payload = new byte[sizeof(uint) * 5];
        BinaryPrimitives.WriteUInt32LittleEndian(payload, questId);
        // status/flags, XP, and money are zero; the fifth uint is reward item count.
        return payload;
    }

    public static byte[] BuildComplete(
        uint questId,
        IReadOnlyList<(uint ItemId, uint Count)> rewards,
        uint money = 0)
    {
        var payload = new List<byte>(20 + rewards.Count * 8);
        WriteUInt(payload, questId);
        WriteUInt(payload, 0); // Status/flags.
        WriteUInt(payload, 0); // XP; experience rewards are added with the level system.
        WriteUInt(payload, money);
        WriteUInt(payload, checked((uint)rewards.Count));
        foreach (var reward in rewards)
        {
            WriteUInt(payload, reward.ItemId);
            WriteUInt(payload, reward.Count);
        }
        return payload.ToArray();
    }

    private static List<(uint ItemId, uint Count)> PairNonZero(
        IReadOnlyList<uint> ids,
        IReadOnlyList<uint> counts)
    {
        var pairs = new List<(uint, uint)>();
        for (var index = 0; index < ids.Count; index++)
        {
            if (ids[index] == 0)
                continue;
            pairs.Add((ids[index], index < counts.Count ? counts[index] : 0));
        }
        return pairs;
    }

    private static void WriteItemStruct(List<byte> payload, uint itemId, uint count, uint displayId)
    {
        WriteUInt(payload, itemId);
        WriteUInt(payload, count);
        WriteUInt(payload, displayId);
    }

    private static void WriteString(List<byte> payload, string value, int? maxBytes = null)
    {
        var bytes = PacketWriter.StringToBytes(value);
        if (maxBytes is int limit && bytes.Length > limit)
        {
            bytes = bytes[..Math.Max(1, limit)];
            bytes[^1] = 0;
        }
        payload.AddRange(bytes);
    }

    private static void WriteUInt64(List<byte> payload, ulong value)
    {
        Span<byte> bytes = stackalloc byte[sizeof(ulong)];
        BinaryPrimitives.WriteUInt64LittleEndian(bytes, value);
        payload.AddRange(bytes.ToArray());
    }

    private static void WriteUInt(List<byte> payload, uint value)
    {
        Span<byte> bytes = stackalloc byte[sizeof(uint)];
        BinaryPrimitives.WriteUInt32LittleEndian(bytes, value);
        payload.AddRange(bytes.ToArray());
    }
}

internal static class VendorPacketWriter
{
    public static byte[] BuildInventory(ulong guid, IReadOnlyList<WorldVendorItem> items)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(guid);
        writer.Write(checked((byte)Math.Min(byte.MaxValue, items.Count)));

        if (items.Count == 0)
        {
            writer.Write((byte)0);
            return stream.ToArray();
        }

        var slot = 1;
        foreach (var item in items.Take(byte.MaxValue))
        {
            writer.Write(slot++);
            writer.Write(checked((int)item.ItemEntry));
            writer.Write(checked((int)item.DisplayId));
            writer.Write(item.MaxCount);
            writer.Write(item.BuyPrice);
            writer.Write(item.MaxDurability);
            writer.Write(item.BuyCount);
        }

        return stream.ToArray();
    }
}

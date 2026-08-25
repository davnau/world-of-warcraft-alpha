using System.Buffers.Binary;
using BattleNet.Persistence.Models;
using BattleNet.Protocol;

namespace BattleNet.Server;

internal static class QuestQueryPacketWriter
{
    public static byte[] Build(WorldQuestTemplate quest)
    {
        var payload = new List<byte>(1024);
        WriteUInt(payload, quest.Entry);
        WriteUInt(payload, quest.Method);
        WriteUInt(payload, quest.QuestLevel);
        WriteInt(payload, quest.ZoneOrSort);
        WriteUInt(payload, quest.Type);
        WriteUInt(payload, quest.NextQuestInChain);
        WriteUInt(payload, unchecked((uint)quest.RewardOrRequiredMoney));
        WriteUInt(payload, quest.SourceItemId);

        WritePairs(payload, quest.RewardItemIds, quest.RewardItemCounts, 4);
        WritePairs(payload, quest.RewardChoiceItemIds, quest.RewardChoiceItemCounts, 6);

        WriteUInt(payload, quest.PointMapId);
        WriteFloat(payload, quest.PointX);
        WriteFloat(payload, quest.PointY);
        WriteUInt(payload, quest.PointOption);
        WriteString(payload, quest.Title);
        WriteString(payload, quest.Details);
        WriteString(payload, quest.Objectives);
        WriteString(payload, quest.EndText);

        foreach (var requirement in quest.Requirements.Take(4))
        {
            var target = requirement.CreatureOrGameObjectId < 0
                ? unchecked((uint)(-requirement.CreatureOrGameObjectId)) | 0x80000000u
                : unchecked((uint)requirement.CreatureOrGameObjectId);
            WriteUInt(payload, target);
            WriteUInt(payload, requirement.CreatureOrGameObjectCount);
            WriteUInt(payload, requirement.ItemId);
            WriteUInt(payload, requirement.ItemCount);
            WriteString(payload, requirement.Name);
        }

        for (var index = quest.Requirements.Count; index < 4; index++)
        {
            WriteUInt(payload, 0);
            WriteUInt(payload, 0);
            WriteUInt(payload, 0);
            WriteUInt(payload, 0);
            WriteString(payload, string.Empty);
        }

        return payload.ToArray();
    }

    private static void WritePairs(List<byte> payload, IReadOnlyList<uint> ids, IReadOnlyList<uint> counts, int count)
    {
        for (var index = 0; index < count; index++)
        {
            WriteUInt(payload, index < ids.Count ? ids[index] : 0);
            WriteUInt(payload, index < counts.Count ? counts[index] : 0);
        }
    }

    private static void WriteString(List<byte> payload, string value) => payload.AddRange(PacketWriter.StringToBytes(value));

    private static void WriteUInt(List<byte> payload, uint value)
    {
        Span<byte> bytes = stackalloc byte[sizeof(uint)];
        BinaryPrimitives.WriteUInt32LittleEndian(bytes, value);
        payload.AddRange(bytes.ToArray());
    }

    private static void WriteInt(List<byte> payload, int value) => WriteUInt(payload, unchecked((uint)value));

    private static void WriteFloat(List<byte> payload, float value)
    {
        Span<byte> bytes = stackalloc byte[sizeof(float)];
        BinaryPrimitives.WriteInt32LittleEndian(bytes, BitConverter.SingleToInt32Bits(value));
        payload.AddRange(bytes.ToArray());
    }
}

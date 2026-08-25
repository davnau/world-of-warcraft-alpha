using System.Buffers.Binary;

namespace BattleNet.Server;

/// <summary>
/// Builds Alpha Core's fixed 120-slot SMSG_ACTION_BUTTONS payload.
/// </summary>
internal static class ActionButtonPacketWriter
{
    private const int MaxActionButtons = 120;

    public static byte[] Build(IReadOnlyDictionary<int, int> actionButtons)
    {
        var payload = new byte[MaxActionButtons * sizeof(int)];
        foreach (var (index, action) in actionButtons)
        {
            if (index is < 0 or >= MaxActionButtons)
                continue;

            BinaryPrimitives.WriteInt32LittleEndian(
                payload.AsSpan(index * sizeof(int), sizeof(int)),
                action);
        }

        return payload;
    }
}

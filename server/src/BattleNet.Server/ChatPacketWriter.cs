using BattleNet.Protocol;

namespace BattleNet.Server;

internal static class ChatPacketWriter
{
    public static byte[] BuildMonsterMessage(
        string name,
        ulong targetGuid,
        string message,
        byte chatType = 0,
        uint language = 0)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(chatType);
        writer.Write(language);
        writer.Write(PacketWriter.StringToBytes(name));
        writer.Write(targetGuid);
        writer.Write(PacketWriter.StringToBytes(message));
        writer.Write((byte)0); // Chat flags.
        return stream.ToArray();
    }

    public static byte[] BuildPlayerMessage(
        byte chatType,
        uint language,
        ulong senderGuid,
        string message,
        byte chatFlags = 0)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(chatType);
        writer.Write(language);
        writer.Write(senderGuid);
        writer.Write(PacketWriter.StringToBytes(message));
        writer.Write(chatFlags);
        return stream.ToArray();
    }
}

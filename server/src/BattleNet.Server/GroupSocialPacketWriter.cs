using BattleNet.Protocol;

namespace BattleNet.Server;

internal static class GroupSocialPacketWriter
{
    public static byte[] BuildPartyResult(uint operation, string name, uint result)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(operation);
        writer.Write(PacketWriter.StringToBytes(name));
        writer.Write(result);
        return stream.ToArray();
    }

    public static byte[] BuildGroupInvite(string inviterName) => PacketWriter.StringToBytes(inviterName);

    public static byte[] BuildGroupDecline(string name) => PacketWriter.StringToBytes(name);

    public static byte[] BuildGroupSetLeader(string name) => PacketWriter.StringToBytes(name);

    public static byte[] BuildGroupList(
        WorldSessionPeer requester,
        WorldPartyState party,
        IReadOnlyList<WorldSessionPeer> members)
    {
        var leader = members.FirstOrDefault(member => member.Character.Guid == party.LeaderGuid);
        var leaderName = leader?.Character.Name ?? string.Empty;
        var visible = members
            .Where(member => member.Character.Guid != requester.Character.Guid && member.Character.Guid != party.LeaderGuid)
            .ToArray();
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write((uint)(visible.Length + (requester.Character.Guid == party.LeaderGuid ? 0 : 1)));
        writer.Write(PacketWriter.StringToBytes(leaderName));
        writer.Write(party.LeaderGuid);
        writer.Write((byte)(leader is not null ? 1 : 0));
        if (requester.Character.Guid != party.LeaderGuid)
        {
            writer.Write(PacketWriter.StringToBytes(requester.Character.Name));
            writer.Write(requester.Character.Guid);
            writer.Write((byte)1);
        }
        foreach (var member in visible)
        {
            writer.Write(PacketWriter.StringToBytes(member.Character.Name));
            writer.Write(member.Character.Guid);
            writer.Write((byte)1);
        }
        writer.Write(party.LootMethod);
        writer.Write(party.MasterLootGuid);
        return stream.ToArray();
    }

    public static byte[] BuildPartyMemberStats(WorldSessionPeer member)
    {
        var character = member.Character;
        var runtime = member.Runtime;
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write(character.Guid);
        writer.Write(runtime?.Health ?? character.Health);
        writer.Write(runtime?.MaxHealth ?? Math.Max(100u, 80u + character.Level * 25u));
        writer.Write((byte)0); // Mana power type in the Alpha character slice.
        writer.Write(runtime?.Power ?? character.Power1);
        writer.Write(runtime?.MaxPower ?? 1000u);
        writer.Write((uint)character.Level);
        writer.Write(character.Map);
        writer.Write(character.Zone);
        writer.Write((uint)character.Class);
        writer.Write(character.PositionX);
        writer.Write(character.PositionY);
        writer.Write(character.PositionZ);
        return stream.ToArray();
    }

    public static byte[] BuildFriendStatus(byte status, ulong guid) =>
        [status, .. BitConverter.GetBytes(guid)];

    public static byte[] BuildFriendList(
        IReadOnlyCollection<ulong> friendGuids,
        IReadOnlyDictionary<ulong, WorldSessionPeer> online)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write((byte)Math.Min(byte.MaxValue, friendGuids.Count));
        foreach (var guid in friendGuids.Take(byte.MaxValue))
        {
            if (!online.TryGetValue(guid, out var peer))
            {
                writer.Write(guid);
                writer.Write((byte)0);
                continue;
            }
            writer.Write(guid);
            writer.Write((byte)1);
            writer.Write(peer.Character.Zone);
            writer.Write((uint)peer.Character.Level);
            writer.Write((uint)peer.Character.Class);
        }
        return stream.ToArray();
    }

    public static byte[] BuildIgnoreList(IReadOnlyCollection<ulong> ignoreGuids)
    {
        using var stream = new MemoryStream();
        using var writer = new BinaryWriter(stream);
        writer.Write((byte)Math.Min(byte.MaxValue, ignoreGuids.Count));
        foreach (var guid in ignoreGuids.Take(byte.MaxValue))
            writer.Write(guid);
        return stream.ToArray();
    }
}

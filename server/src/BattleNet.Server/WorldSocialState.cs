using BattleNet.Core.Networking;
using BattleNet.Persistence.Models;

namespace BattleNet.Server;

internal sealed class WorldSocialState
{
    public const int PartyInvite = 0;
    public const int PartyLeave = 2;

    private readonly object _gate = new();
    private readonly Dictionary<ulong, WorldSessionPeer> _players = new();
    private readonly Dictionary<string, WorldSessionPeer> _playersByName = new(StringComparer.OrdinalIgnoreCase);
    private readonly HashSet<(ulong ObjectGuid, int Slot)> _claimedSharedLoot = new();
    private readonly Dictionary<(uint PartyId, int MapId), uint> _instanceTokens = new();
    private uint _nextPartyId = 1;
    private uint _nextInstanceToken = 1;

    public WorldSessionPeer Register(CharacterSummary character, SessionConnection session)
    {
        lock (_gate)
        {
            if (_players.TryGetValue(character.Guid, out var existing))
                _playersByName.Remove(existing.Character.Name);

            var peer = new WorldSessionPeer(character, session);
            _players[character.Guid] = peer;
            _playersByName[character.Name] = peer;
            return peer;
        }
    }

    public void Unregister(ulong guid)
    {
        lock (_gate)
        {
            if (!_players.Remove(guid, out var peer))
                return;
            _playersByName.Remove(peer.Character.Name);
            if (peer.Party is not null)
            {
                peer.Party.Members.Remove(guid);
                if (peer.Party.LeaderGuid == guid)
                    peer.Party.LeaderGuid = peer.Party.Members.FirstOrDefault();
                if (peer.Party.Members.Count == 0)
                    peer.Party = null;
            }
        }
    }

    public void UpdateCharacter(CharacterSummary character)
    {
        lock (_gate)
        {
            if (!_players.TryGetValue(character.Guid, out var peer))
                return;
            _playersByName.Remove(peer.Character.Name);
            peer.Character = character;
            _playersByName[character.Name] = peer;
        }
    }

    public WorldSessionPeer? FindByGuid(ulong guid)
    {
        lock (_gate)
            return _players.GetValueOrDefault(guid);
    }

    public WorldSessionPeer? FindByName(string name)
    {
        lock (_gate)
            return _playersByName.GetValueOrDefault(name.Trim());
    }

    public IReadOnlyList<WorldSessionPeer> GetOnlinePlayers()
    {
        lock (_gate)
            return _players.Values.ToArray();
    }

    public IReadOnlyList<WorldSessionPeer> GetNearby(CharacterSummary source, float radius)
    {
        lock (_gate)
        {
            var radiusSquared = radius * radius;
            return _players.Values
                .Where(peer => peer.Character.Map == source.Map
                    && DistanceSquared(peer.Character.PositionX, peer.Character.PositionY, peer.Character.PositionZ,
                        source.PositionX, source.PositionY, source.PositionZ) <= radiusSquared)
                .ToArray();
        }
    }

    public bool TryInvite(ulong inviterGuid, string targetName, out WorldSessionPeer? target, out int result)
    {
        lock (_gate)
        {
            target = _playersByName.GetValueOrDefault(targetName.Trim());
            result = 1;
            if (!_players.TryGetValue(inviterGuid, out var inviter) || target is null)
                return false;
            if (target.Character.Guid == inviterGuid || target.Party is not null || target.PendingParty is not null)
            {
                result = target.Character.Guid == inviterGuid ? 9 : 4;
                return false;
            }
            if (target.Ignores.Contains(inviterGuid))
            {
                result = 8;
                return false;
            }

            var party = inviter.Party;
            if (party is not null && party.LeaderGuid != inviterGuid)
            {
                result = 6;
                return false;
            }
            if (party is not null && party.Members.Count >= 5)
            {
                result = 3;
                return false;
            }
            if (party is null)
            {
                party = new WorldPartyState(_nextPartyId++, inviterGuid);
                party.Members.Add(inviterGuid);
                inviter.Party = party;
            }

            target.PendingParty = party;
            target.PendingInviterGuid = inviterGuid;
            result = 0;
            return true;
        }
    }

    public bool TryAcceptInvite(ulong targetGuid, out WorldPartyState? party)
    {
        lock (_gate)
        {
            party = null;
            if (!_players.TryGetValue(targetGuid, out var target) || target.PendingParty is null)
                return false;
            party = target.PendingParty;
            target.PendingParty = null;
            target.PendingInviterGuid = 0;
            if (party.Members.Count >= 5 || !party.Members.Contains(party.LeaderGuid))
                return false;
            party.Members.Add(targetGuid);
            target.Party = party;
            return true;
        }
    }

    public WorldPartyState? DeclineInvite(ulong targetGuid)
    {
        lock (_gate)
        {
            if (!_players.TryGetValue(targetGuid, out var target))
                return null;
            var party = target.PendingParty;
            target.PendingParty = null;
            target.PendingInviterGuid = 0;
            return party;
        }
    }

    public bool TrySetLeader(ulong requesterGuid, ulong targetGuid)
    {
        lock (_gate)
        {
            var requester = _players.GetValueOrDefault(requesterGuid);
            if (requester?.Party is null || requester.Party.LeaderGuid != requesterGuid || !requester.Party.Members.Contains(targetGuid))
                return false;
            requester.Party.LeaderGuid = targetGuid;
            return true;
        }
    }

    public bool TrySetLootMethod(ulong requesterGuid, byte method, ulong masterLootGuid)
    {
        lock (_gate)
        {
            var requester = _players.GetValueOrDefault(requesterGuid);
            if (requester?.Party is null || requester.Party.LeaderGuid != requesterGuid || method > 2)
                return false;
            requester.Party.LootMethod = method;
            requester.Party.MasterLootGuid = requester.Party.Members.Contains(masterLootGuid) ? masterLootGuid : 0;
            return true;
        }
    }

    public WorldPartyState? LeaveParty(ulong guid, ulong? targetGuid = null)
    {
        lock (_gate)
        {
            var peer = _players.GetValueOrDefault(guid);
            var party = peer?.Party;
            if (party is null)
                return null;
            if (targetGuid.HasValue && (party.LeaderGuid != guid || !party.Members.Contains(targetGuid.Value)))
                return null;

            var removedGuid = targetGuid ?? guid;
            party.Members.Remove(removedGuid);
            if (_players.TryGetValue(removedGuid, out var removed))
            {
                removed.Party = null;
                removed.PendingParty = null;
            }
            if (party.LeaderGuid == removedGuid)
                party.LeaderGuid = party.Members.FirstOrDefault();
            if (party.Members.Count <= 1)
            {
                foreach (var memberGuid in party.Members.ToArray())
                    if (_players.TryGetValue(memberGuid, out var member))
                        member.Party = null;
                party.Members.Clear();
            }
            return party;
        }
    }

    public WorldPartyState? DisbandParty(ulong guid)
    {
        lock (_gate)
        {
            var requester = _players.GetValueOrDefault(guid);
            var party = requester?.Party;
            if (party is null || party.LeaderGuid != guid)
                return null;
            foreach (var memberGuid in party.Members.ToArray())
            {
                if (_players.TryGetValue(memberGuid, out var member))
                {
                    member.Party = null;
                    member.PendingParty = null;
                }
            }
            party.Members.Clear();
            return party;
        }
    }

    public IReadOnlyList<WorldSessionPeer> GetPartyMembers(ulong guid, bool nearbyOnly = false)
    {
        lock (_gate)
        {
            var requester = _players.GetValueOrDefault(guid);
            if (requester?.Party is null)
                return requester is null ? Array.Empty<WorldSessionPeer>() : [requester];

            var members = requester.Party.Members
                .Select(memberGuid => _players.GetValueOrDefault(memberGuid))
                .Where(member => member is not null)
                .Cast<WorldSessionPeer>()
                .ToArray();
            if (!nearbyOnly)
                return members;
            return members.Where(member => member.Character.Map == requester.Character.Map
                && DistanceSquared(member.Character.PositionX, member.Character.PositionY, member.Character.PositionZ,
                    requester.Character.PositionX, requester.Character.PositionY, requester.Character.PositionZ) <= 100 * 100).ToArray();
        }
    }

    public uint GetInstanceToken(ulong guid, int mapId)
    {
        lock (_gate)
        {
            var peer = _players.GetValueOrDefault(guid);
            var partyId = peer?.Party?.Id ?? 0;
            if (partyId == 0)
                return 0;
            var key = (partyId, mapId);
            if (!_instanceTokens.TryGetValue(key, out var token))
            {
                token = _nextInstanceToken++;
                _instanceTokens[key] = token;
            }
            return token;
        }
    }

    public bool TryClaimSharedLoot(ulong objectGuid, int slot, ulong claimantGuid)
    {
        lock (_gate)
        {
            var key = (objectGuid, slot);
            if (_claimedSharedLoot.Contains(key))
                return false;
            _claimedSharedLoot.Add(key);
            return true;
        }
    }

    public (byte Status, ulong Guid) AddFriend(ulong ownerGuid, string name)
    {
        lock (_gate)
        {
            var owner = _players.GetValueOrDefault(ownerGuid);
            var target = _playersByName.GetValueOrDefault(name.Trim());
            if (owner is null || target is null)
                return (4, 0);
            if (ownerGuid == target.Character.Guid)
                return (9, target.Character.Guid);
            if (owner.Friends.Contains(target.Character.Guid))
                return (8, target.Character.Guid);
            if (owner.Friends.Count >= 50)
                return (1, target.Character.Guid);
            owner.Friends.Add(target.Character.Guid);
            return (6, target.Character.Guid);
        }
    }

    public (byte Status, ulong Guid) RemoveFriend(ulong ownerGuid, ulong targetGuid)
    {
        lock (_gate)
        {
            var owner = _players.GetValueOrDefault(ownerGuid);
            if (owner is null || !owner.Friends.Remove(targetGuid))
                return (4, targetGuid);
            return (5, targetGuid);
        }
    }

    public (byte Status, ulong Guid) AddIgnore(ulong ownerGuid, string name)
    {
        lock (_gate)
        {
            var owner = _players.GetValueOrDefault(ownerGuid);
            var target = _playersByName.GetValueOrDefault(name.Trim());
            if (owner is null || target is null)
                return (13, 0);
            if (ownerGuid == target.Character.Guid)
                return (12, target.Character.Guid);
            if (owner.Ignores.Contains(target.Character.Guid))
                return (14, target.Character.Guid);
            if (owner.Ignores.Count >= 25)
                return (11, target.Character.Guid);
            owner.Ignores.Add(target.Character.Guid);
            return (15, target.Character.Guid);
        }
    }

    public (byte Status, ulong Guid) RemoveIgnore(ulong ownerGuid, ulong targetGuid)
    {
        lock (_gate)
        {
            var owner = _players.GetValueOrDefault(ownerGuid);
            if (owner is null || !owner.Ignores.Remove(targetGuid))
                return (13, targetGuid);
            return (16, targetGuid);
        }
    }

    private static float DistanceSquared(float x1, float y1, float z1, float x2, float y2, float z2)
    {
        var x = x1 - x2;
        var y = y1 - y2;
        var z = z1 - z2;
        return x * x + y * y + z * z;
    }
}

internal sealed class WorldSessionPeer(CharacterSummary character, SessionConnection session)
{
    public CharacterSummary Character { get; set; } = character;
    public SessionConnection Session { get; } = session;
    public PlayerRuntimeState? Runtime { get; set; }
    public Dictionary<ulong, LootRuntimeState> LootByCreature { get; } = new();
    public HashSet<ulong> Friends { get; } = new();
    public HashSet<ulong> Ignores { get; } = new();
    public WorldPartyState? Party { get; set; }
    public WorldPartyState? PendingParty { get; set; }
    public ulong PendingInviterGuid { get; set; }
    public uint InstanceToken { get; set; }
}

internal sealed class WorldPartyState(uint id, ulong leaderGuid)
{
    public uint Id { get; } = id;
    public ulong LeaderGuid { get; set; } = leaderGuid;
    public byte LootMethod { get; set; }
    public ulong MasterLootGuid { get; set; }
    public HashSet<ulong> Members { get; } = new();
}

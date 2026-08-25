using System.Buffers.Binary;
using System.Net.Sockets;
using System.Security.Cryptography;
using System.Text;
using BattleNet.Core.Configuration;
using BattleNet.Core.Networking;
using BattleNet.Persistence;
using BattleNet.Persistence.Models;
using BattleNet.Protocol;
using Microsoft.Extensions.Logging;
using BattleNet.Server;

namespace BattleNet.Server.Services;

internal sealed class WorldServerService(
    ILogger<WorldServerService> logger,
    ServerOptions options,
    IAuthRepository authRepository,
    IRealmRepository realmRepository,
    IDbcRepository dbcRepository,
    IWorldRepository worldRepository,
    IWorldContentRepository worldContentRepository,
    IQuestRepository questRepository,
    ICharacterMutationRepository characterMutationRepository,
    ICharacterStateRepository characterStateRepository,
    WorldSocialState socialState)
    : TcpListenerService(logger, "WorldServer", options.BindAddress, options.WorldPort)
{
    private readonly byte[] _serverSeed = RandomNumberGenerator.GetBytes(4);
    private static readonly SemaphoreSlim CharacterMutationLock = new(1, 1);
    private static long _nextCorpseLowGuid;

    protected override async Task HandleClientAsync(TcpClient client, CancellationToken stoppingToken)
    {
        await using var session = new SessionConnection(client);
        await session.SendPacketAsync(Opcode.SMSG_AUTH_CHALLENGE, _serverSeed, stoppingToken).ConfigureAwait(false);
        var authenticatedAccount = await AuthenticateAsync(session, client, stoppingToken).ConfigureAwait(false);
        if (authenticatedAccount is null)
            return;

        await session.SendPacketAsync(Opcode.SMSG_AUTH_RESPONSE, new byte[] { (byte)AuthCode.AuthOk }, stoppingToken).ConfigureAwait(false);
        var activeCharacterGuid = 0UL;
        var selectedTargetGuid = 0UL;
        var creatures = new Dictionary<ulong, CreatureRuntimeState>();
        var lootByCreature = new Dictionary<ulong, LootRuntimeState>();
        PlayerRuntimeState? playerRuntime = null;
        WorldSessionPeer? sessionPeer = null;
        var pendingRead = session.ReadWorldPacketAsync(stoppingToken).AsTask();
        var nextSimulationTick = DateTimeOffset.UtcNow.AddMilliseconds(250);

        while (!stoppingToken.IsCancellationRequested)
        {
            var tickDelay = nextSimulationTick - DateTimeOffset.UtcNow;
            var tick = tickDelay <= TimeSpan.Zero
                ? Task.CompletedTask
                : Task.Delay(tickDelay, stoppingToken);
            var completed = await Task.WhenAny(pendingRead, tick).ConfigureAwait(false);
            if (completed != pendingRead)
            {
                if (playerRuntime is not null)
                {
                    await UpdateWorldSimulationAsync(
                        session,
                        playerRuntime,
                        creatures,
                        lootByCreature,
                        stoppingToken).ConfigureAwait(false);
                }
                nextSimulationTick = DateTimeOffset.UtcNow.AddMilliseconds(250);
                continue;
            }

            var packet = await pendingRead.ConfigureAwait(false);
            if (packet is null)
            {
                if (activeCharacterGuid != 0)
                    socialState.Unregister(activeCharacterGuid);
                return;
            }
            pendingRead = session.ReadWorldPacketAsync(stoppingToken).AsTask();

            switch (packet.Value.Opcode)
            {
                case Opcode.CMSG_PING when packet.Value.Payload.Length >= 4:
                    await session.SendPacketAsync(Opcode.SMSG_PONG, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_CHAR_ENUM:
                    await SendCharacterListAsync(session, authenticatedAccount.Id, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_CHAR_CREATE:
                    await CreateCharacterAsync(session, authenticatedAccount.Id, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_CHAR_DELETE when packet.Value.Payload.Length == sizeof(ulong):
                    await DeleteCharacterAsync(session, authenticatedAccount.Id, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_NAME_QUERY when packet.Value.Payload.Length == sizeof(ulong):
                    await SendNameQueryAsync(session, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_MESSAGECHAT when packet.Value.Payload.Length >= sizeof(uint) * 2:
                    await HandleChatMessageAsync(sessionPeer, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_FRIEND_LIST:
                    await HandleFriendListAsync(sessionPeer, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_ADD_FRIEND:
                    await HandleAddFriendAsync(sessionPeer, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_DEL_FRIEND when packet.Value.Payload.Length >= sizeof(ulong):
                    await HandleDeleteFriendAsync(sessionPeer, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_ADD_IGNORE:
                    await HandleAddIgnoreAsync(sessionPeer, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_DEL_IGNORE when packet.Value.Payload.Length >= sizeof(ulong):
                    await HandleDeleteIgnoreAsync(sessionPeer, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_GROUP_INVITE:
                    await HandleGroupInviteAsync(sessionPeer, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_GROUP_ACCEPT:
                    await HandleGroupAcceptAsync(sessionPeer, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_GROUP_DECLINE:
                    await HandleGroupDeclineAsync(sessionPeer, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_GROUP_UNINVITE:
                    await HandleGroupUninviteAsync(sessionPeer, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_GROUP_UNINVITE_GUID when packet.Value.Payload.Length >= sizeof(ulong):
                    await HandleGroupUninviteGuidAsync(sessionPeer, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_GROUP_SET_LEADER:
                    await HandleGroupSetLeaderAsync(sessionPeer, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_LOOT_METHOD when packet.Value.Payload.Length >= 1 + sizeof(ulong):
                    await HandleLootMethodAsync(sessionPeer, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_GROUP_DISBAND:
                    await HandleGroupDisbandAsync(sessionPeer, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.MSG_MINIMAP_PING when packet.Value.Payload.Length >= sizeof(float) * 2:
                    await HandleMinimapPingAsync(sessionPeer, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_ZONEUPDATE when packet.Value.Payload.Length >= sizeof(uint):
                    if (playerRuntime is not null)
                    {
                        playerRuntime.Character = playerRuntime.Character with
                        {
                            Zone = BinaryPrimitives.ReadUInt32LittleEndian(packet.Value.Payload.AsSpan())
                        };
                        socialState.UpdateCharacter(playerRuntime.Character);
                        if (sessionPeer is not null)
                            sessionPeer.InstanceToken = socialState.GetInstanceToken(playerRuntime.Character.Guid, checked((int)playerRuntime.Character.Map));
                    }
                    break;
                case Opcode.CMSG_SET_SELECTION when packet.Value.Payload.Length >= sizeof(ulong):
                case Opcode.CMSG_SET_TARGET when packet.Value.Payload.Length >= sizeof(ulong):
                    selectedTargetGuid = BinaryPrimitives.ReadUInt64LittleEndian(packet.Value.Payload.AsSpan());
                    break;
                case Opcode.CMSG_ATTACKSWING when packet.Value.Payload.Length >= sizeof(ulong):
                    await HandleAttackSwingAsync(session, activeCharacterGuid, playerRuntime, packet.Value.Payload, creatures, lootByCreature, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_ATTACKSTOP:
                    await HandleAttackStopAsync(session, activeCharacterGuid, selectedTargetGuid, stoppingToken).ConfigureAwait(false);
                    selectedTargetGuid = 0;
                    break;
                case Opcode.CMSG_CAST_SPELL when packet.Value.Payload.Length >= sizeof(uint) + sizeof(ushort):
                    await HandleCastSpellAsync(
                        session,
                        playerRuntime,
                        selectedTargetGuid,
                        packet.Value.Payload,
                        creatures,
                        lootByCreature,
                        stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_USE_ITEM when packet.Value.Payload.Length >= 5:
                    await HandleUseItemAsync(
                        session,
                        playerRuntime,
                        selectedTargetGuid,
                        packet.Value.Payload,
                        creatures,
                        lootByCreature,
                        stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_CANCEL_AURA when packet.Value.Payload.Length >= sizeof(uint):
                    if (playerRuntime is not null)
                    {
                        var auraId = BinaryPrimitives.ReadUInt32LittleEndian(packet.Value.Payload.AsSpan());
                        playerRuntime.Auras.Remove(auraId);
                    }
                    break;
                case Opcode.CMSG_LOOT when packet.Value.Payload.Length >= sizeof(ulong):
                    await HandleLootRequestAsync(session, playerRuntime, packet.Value.Payload, lootByCreature, creatures, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_AUTOSTORE_LOOT_ITEM when packet.Value.Payload.Length >= 1:
                    await HandleLootItemAsync(session, playerRuntime, packet.Value.Payload[0], lootByCreature, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_STORE_LOOT_IN_SLOT when packet.Value.Payload.Length >= 2:
                    await HandleLootItemAsync(session, playerRuntime, packet.Value.Payload[1], lootByCreature, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_LOOT_MONEY:
                    await HandleLootMoneyAsync(session, playerRuntime, lootByCreature, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_LOOT_RELEASE when packet.Value.Payload.Length >= sizeof(ulong):
                    await HandleLootReleaseAsync(session, playerRuntime, packet.Value.Payload, lootByCreature, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_REPOP_REQUEST:
                    if (playerRuntime is not null)
                    {
                        await HandleRepopRequestAsync(
                            session,
                            playerRuntime,
                            creatures,
                            stoppingToken).ConfigureAwait(false);
                        selectedTargetGuid = 0;
                    }
                    break;
                case Opcode.MSG_MOVE_TELEPORT_ACK:
                    if (playerRuntime is not null)
                    {
                        await CompletePlayerTeleportAsync(
                            session,
                            playerRuntime,
                            creatures,
                            longTeleportAcknowledgement: false,
                            stoppingToken).ConfigureAwait(false);
                    }
                    break;
                case Opcode.MSG_MOVE_WORLDPORT_ACK:
                    if (playerRuntime is not null)
                    {
                        await CompletePlayerTeleportAsync(
                            session,
                            playerRuntime,
                            creatures,
                            longTeleportAcknowledgement: true,
                            stoppingToken).ConfigureAwait(false);
                    }
                    break;
                case Opcode.CMSG_RECLAIM_CORPSE when packet.Value.Payload.Length >= sizeof(ulong):
                    if (playerRuntime is not null)
                    {
                        await HandleReclaimCorpseAsync(
                            session,
                            playerRuntime,
                            packet.Value.Payload,
                            stoppingToken).ConfigureAwait(false);
                    }
                    break;
                case Opcode.CMSG_RESURRECT_RESPONSE when packet.Value.Payload.Length >= sizeof(ulong) + 1:
                    // Player-cast resurrection requests use this opcode too.
                    // No request is accepted unless a matching offer has been
                    // installed; release-spirit resurrection uses REPOP above.
                    break;
                case Opcode.MSG_MOVE_START_FORWARD:
                case Opcode.MSG_MOVE_START_BACKWARD:
                case Opcode.MSG_MOVE_STOP:
                case Opcode.MSG_MOVE_START_STRAFE_LEFT:
                case Opcode.MSG_MOVE_START_STRAFE_RIGHT:
                case Opcode.MSG_MOVE_STOP_STRAFE:
                case Opcode.MSG_MOVE_JUMP:
                case Opcode.MSG_MOVE_START_TURN_LEFT:
                case Opcode.MSG_MOVE_START_TURN_RIGHT:
                case Opcode.MSG_MOVE_STOP_TURN:
                case Opcode.MSG_MOVE_START_PITCH_UP:
                case Opcode.MSG_MOVE_START_PITCH_DOWN:
                case Opcode.MSG_MOVE_STOP_PITCH:
                case Opcode.MSG_MOVE_SET_RUN_MODE:
                case Opcode.MSG_MOVE_SET_WALK_MODE:
                case Opcode.MSG_MOVE_HEARTBEAT:
                case Opcode.MSG_MOVE_SET_FACING:
                    if (playerRuntime is not null)
                    {
                        await HandlePlayerMovementAsync(
                            session,
                            playerRuntime,
                            packet.Value.Payload,
                            packet.Value.Opcode,
                            stoppingToken).ConfigureAwait(false);
                    }
                    break;
                case Opcode.CMSG_ITEM_QUERY_SINGLE when packet.Value.Payload.Length >= sizeof(uint):
                    await SendItemQuerySingleAsync(session, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_ITEM_QUERY_MULTIPLE when packet.Value.Payload.Length >= sizeof(uint):
                    await SendItemQueryMultipleAsync(session, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_CREATURE_QUERY when packet.Value.Payload.Length >= sizeof(uint):
                    await SendCreatureQueryAsync(session, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_GAMEOBJECT_QUERY when packet.Value.Payload.Length >= sizeof(uint):
                    await SendGameObjectQueryAsync(session, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_QUEST_QUERY when packet.Value.Payload.Length >= sizeof(uint):
                    await SendQuestQueryAsync(session, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_QUESTGIVER_STATUS_QUERY when packet.Value.Payload.Length >= sizeof(ulong):
                    await SendQuestGiverStatusAsync(session, packet.Value.Payload, activeCharacterGuid, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_QUESTGIVER_HELLO when packet.Value.Payload.Length >= sizeof(ulong):
                    await SendQuestGiverHelloAsync(session, packet.Value.Payload, activeCharacterGuid, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_QUESTGIVER_QUERY_QUEST when packet.Value.Payload.Length >= sizeof(ulong) + sizeof(uint):
                    await SendQuestGiverDetailsAsync(session, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_QUESTGIVER_ACCEPT_QUEST when packet.Value.Payload.Length >= sizeof(ulong) + sizeof(uint):
                    await AcceptQuestAsync(session, packet.Value.Payload, activeCharacterGuid, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_QUESTGIVER_COMPLETE_QUEST when packet.Value.Payload.Length >= sizeof(ulong) + sizeof(uint):
                    await CompleteQuestAsync(session, packet.Value.Payload, activeCharacterGuid, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_QUESTGIVER_REQUEST_REWARD when packet.Value.Payload.Length >= sizeof(ulong) + sizeof(uint):
                    await RequestQuestRewardAsync(session, packet.Value.Payload, activeCharacterGuid, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_QUESTGIVER_CHOOSE_REWARD when packet.Value.Payload.Length >= sizeof(ulong) + (sizeof(uint) * 2):
                    await ChooseQuestRewardAsync(session, packet.Value.Payload, activeCharacterGuid, playerRuntime, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_QUESTLOG_REMOVE_QUEST when packet.Value.Payload.Length >= sizeof(uint):
                    await RemoveQuestAsync(session, packet.Value.Payload, activeCharacterGuid, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_LIST_INVENTORY when packet.Value.Payload.Length >= sizeof(ulong):
                    await SendVendorInventoryAsync(session, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_QUERY_TIME:
                    await SendQueryTimeAsync(session, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_PLAYER_LOGIN when packet.Value.Payload.Length == sizeof(ulong):
                    creatures.Clear();
                    lootByCreature.Clear();
                    activeCharacterGuid = await LoginCharacterAsync(session, authenticatedAccount.Id, packet.Value.Payload, stoppingToken).ConfigureAwait(false) ?? 0;
                    if (activeCharacterGuid != 0)
                    {
                        var character = await realmRepository.FindCharacterAsync(
                            activeCharacterGuid,
                            options.LocalRealmId,
                            stoppingToken).ConfigureAwait(false);
                        var spells = await realmRepository.GetCharacterSpellsAsync(activeCharacterGuid, stoppingToken).ConfigureAwait(false);
                        if (character is not null)
                        {
                            playerRuntime = new PlayerRuntimeState(character, spells);
                            sessionPeer = socialState.Register(character, session);
                            sessionPeer.Runtime = playerRuntime;
                            lootByCreature = sessionPeer.LootByCreature;
                            sessionPeer.InstanceToken = socialState.GetInstanceToken(character.Guid, checked((int)character.Map));
                            await LoadCreatureRuntimeAsync(playerRuntime, creatures, stoppingToken).ConfigureAwait(false);
                            await session.SendPacketAsync(
                                Opcode.SMSG_UPDATE_OBJECT,
                                PlayerUpdatePacketWriter.BuildHealthUpdate(playerRuntime.Character.Guid, playerRuntime.Health, playerRuntime.MaxHealth),
                                stoppingToken).ConfigureAwait(false);
                            await session.SendPacketAsync(
                                Opcode.SMSG_UPDATE_OBJECT,
                                PlayerUpdatePacketWriter.BuildPowerUpdate(playerRuntime.Character.Guid, playerRuntime.Power, playerRuntime.MaxPower),
                                stoppingToken).ConfigureAwait(false);
                            foreach (var state in creatures.Values)
                            {
                                await session.SendPacketAsync(
                                    Opcode.SMSG_UPDATE_OBJECT,
                                    CreatureUpdatePacketWriter.BuildHealthUpdate(
                                        BuildCreatureGuid(state.Spawn.SpawnId),
                                        state.Health,
                                        state.MaxHealth),
                                    stoppingToken).ConfigureAwait(false);
                            }
                        }
                    }
                    break;
                case Opcode.CMSG_PLAYER_LOGOUT:
                case Opcode.CMSG_LOGOUT_REQUEST:
                    if (playerRuntime is not null)
                    {
                        playerRuntime.Character = playerRuntime.Character with
                        {
                            Health = playerRuntime.Health,
                            Power1 = playerRuntime.Power
                        };
                        await characterStateRepository.SaveCharacterStateAsync(playerRuntime.Character, stoppingToken).ConfigureAwait(false);
                    }
                    if (activeCharacterGuid != 0)
                        socialState.Unregister(activeCharacterGuid);
                    return;
            }

            if (playerRuntime is not null &&
                DateTimeOffset.UtcNow >= nextSimulationTick)
            {
                await UpdateWorldSimulationAsync(
                    session,
                    playerRuntime,
                    creatures,
                    lootByCreature,
                    stoppingToken).ConfigureAwait(false);
                nextSimulationTick = DateTimeOffset.UtcNow.AddMilliseconds(250);
            }
        }
    }

    private async Task HandleFriendListAsync(
        WorldSessionPeer? peer,
        CancellationToken cancellationToken)
    {
        if (peer is null)
            return;
        var online = socialState.GetOnlinePlayers().ToDictionary(item => item.Character.Guid);
        await peer.Session.SendPacketAsync(
            Opcode.SMSG_FRIEND_LIST,
            GroupSocialPacketWriter.BuildFriendList(peer.Friends, online),
            cancellationToken).ConfigureAwait(false);
        await peer.Session.SendPacketAsync(
            Opcode.SMSG_IGNORE_LIST,
            GroupSocialPacketWriter.BuildIgnoreList(peer.Ignores),
            cancellationToken).ConfigureAwait(false);
    }

    private async Task HandleChatMessageAsync(
        WorldSessionPeer? peer,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        if (peer is null || payload.Length < 8)
            return;
        var chatType = checked((byte)BinaryPrimitives.ReadUInt32LittleEndian(payload.Span));
        var language = BinaryPrimitives.ReadUInt32LittleEndian(payload.Span[4..]);
        var messageOffset = 8;
        WorldSessionPeer? whisperTarget = null;
        if (chatType == 5)
        {
            var targetName = PacketReader.ReadCString(payload.Span, messageOffset).Trim();
            var targetEnd = PacketReader.CStringEnd(payload.Span, messageOffset);
            if (targetEnd < 0)
                return;
            messageOffset = targetEnd + 1;
            whisperTarget = socialState.FindByName(targetName);
            if (whisperTarget is null)
                return;
        }
        var message = PacketReader.ReadCString(payload.Span, messageOffset);
        if (string.IsNullOrWhiteSpace(message))
            return;

        if (chatType == 5 && whisperTarget is not null)
        {
            await whisperTarget.Session.SendPacketAsync(
                Opcode.SMSG_MESSAGECHAT,
                ChatPacketWriter.BuildPlayerMessage(5, language, peer.Character.Guid, message),
                cancellationToken).ConfigureAwait(false);
            await peer.Session.SendPacketAsync(
                Opcode.SMSG_MESSAGECHAT,
                ChatPacketWriter.BuildPlayerMessage(6, language, whisperTarget.Character.Guid, message),
                cancellationToken).ConfigureAwait(false);
            return;
        }

        IReadOnlyList<WorldSessionPeer> recipients = chatType switch
        {
            1 => peer.Party is null ? [peer] : GetOnlinePartyMembers(peer.Party),
            4 => socialState.GetNearby(peer.Character, 300),
            0 or 7 => socialState.GetNearby(peer.Character, 25),
            _ => Array.Empty<WorldSessionPeer>()
        };
        foreach (var recipient in recipients)
            await recipient.Session.SendPacketAsync(
                Opcode.SMSG_MESSAGECHAT,
                ChatPacketWriter.BuildPlayerMessage(chatType, language, peer.Character.Guid, message),
                cancellationToken).ConfigureAwait(false);
    }

    private async Task HandleAddFriendAsync(
        WorldSessionPeer? peer,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        if (peer is null)
            return;
        var name = PacketReader.ReadCString(payload.Span, 0).Trim();
        var result = socialState.AddFriend(peer.Character.Guid, name);
        await peer.Session.SendPacketAsync(
            Opcode.SMSG_FRIEND_STATUS,
            GroupSocialPacketWriter.BuildFriendStatus(result.Status, result.Guid),
            cancellationToken).ConfigureAwait(false);
    }

    private async Task HandleDeleteFriendAsync(
        WorldSessionPeer? peer,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        if (peer is null)
            return;
        var guid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        var result = socialState.RemoveFriend(peer.Character.Guid, guid);
        await peer.Session.SendPacketAsync(
            Opcode.SMSG_FRIEND_STATUS,
            GroupSocialPacketWriter.BuildFriendStatus(result.Status, result.Guid),
            cancellationToken).ConfigureAwait(false);
    }

    private async Task HandleAddIgnoreAsync(
        WorldSessionPeer? peer,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        if (peer is null)
            return;
        var name = PacketReader.ReadCString(payload.Span, 0).Trim();
        var result = socialState.AddIgnore(peer.Character.Guid, name);
        await peer.Session.SendPacketAsync(
            Opcode.SMSG_FRIEND_STATUS,
            GroupSocialPacketWriter.BuildFriendStatus(result.Status, result.Guid),
            cancellationToken).ConfigureAwait(false);
    }

    private async Task HandleDeleteIgnoreAsync(
        WorldSessionPeer? peer,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        if (peer is null)
            return;
        var guid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        var result = socialState.RemoveIgnore(peer.Character.Guid, guid);
        await peer.Session.SendPacketAsync(
            Opcode.SMSG_FRIEND_STATUS,
            GroupSocialPacketWriter.BuildFriendStatus(result.Status, result.Guid),
            cancellationToken).ConfigureAwait(false);
    }

    private async Task HandleGroupInviteAsync(
        WorldSessionPeer? peer,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        if (peer is null)
            return;
        var targetName = PacketReader.ReadCString(payload.Span, 0).Trim();
        var invited = socialState.TryInvite(peer.Character.Guid, targetName, out var target, out var result);
        await peer.Session.SendPacketAsync(
            Opcode.SMSG_PARTY_COMMAND_RESULT,
            GroupSocialPacketWriter.BuildPartyResult(WorldSocialState.PartyInvite, targetName, checked((uint)result)),
            cancellationToken).ConfigureAwait(false);
        if (invited && target is not null)
        {
            await target.Session.SendPacketAsync(
                Opcode.SMSG_GROUP_INVITE,
                GroupSocialPacketWriter.BuildGroupInvite(peer.Character.Name),
                cancellationToken).ConfigureAwait(false);
        }
    }

    private async Task HandleGroupAcceptAsync(
        WorldSessionPeer? peer,
        CancellationToken cancellationToken)
    {
        if (peer is null || !socialState.TryAcceptInvite(peer.Character.Guid, out var party) || party is null)
            return;
        await SendGroupUpdateAsync(party, cancellationToken).ConfigureAwait(false);
    }

    private async Task HandleGroupDeclineAsync(
        WorldSessionPeer? peer,
        CancellationToken cancellationToken)
    {
        if (peer is null)
            return;
        var party = socialState.DeclineInvite(peer.Character.Guid);
        if (party is null)
            return;
        var leader = socialState.FindByGuid(party.LeaderGuid);
        if (leader is not null)
            await leader.Session.SendPacketAsync(
                Opcode.SMSG_GROUP_DECLINE,
                GroupSocialPacketWriter.BuildGroupDecline(peer.Character.Name),
                cancellationToken).ConfigureAwait(false);
    }

    private async Task HandleGroupUninviteAsync(
        WorldSessionPeer? peer,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        if (peer is null)
            return;
        var name = PacketReader.ReadCString(payload.Span, 0).Trim();
        var target = socialState.FindByName(name);
        if (target is null)
        {
            await SendPartyResultAsync(peer, WorldSocialState.PartyLeave, name, 1, cancellationToken).ConfigureAwait(false);
            return;
        }
        var party = socialState.LeaveParty(peer.Character.Guid, target.Character.Guid);
        if (party is null)
        {
            await SendPartyResultAsync(peer, WorldSocialState.PartyLeave, name, 6, cancellationToken).ConfigureAwait(false);
            return;
        }
        await target.Session.SendPacketAsync(Opcode.SMSG_GROUP_UNINVITE, ReadOnlyMemory<byte>.Empty, cancellationToken).ConfigureAwait(false);
        await SendGroupUpdateAsync(party, cancellationToken).ConfigureAwait(false);
    }

    private async Task HandleGroupUninviteGuidAsync(
        WorldSessionPeer? peer,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        if (peer is null)
            return;
        var targetGuid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        var target = socialState.FindByGuid(targetGuid);
        if (target is null)
        {
            await SendPartyResultAsync(peer, WorldSocialState.PartyLeave, string.Empty, 1, cancellationToken).ConfigureAwait(false);
            return;
        }
        var party = socialState.LeaveParty(peer.Character.Guid, targetGuid);
        if (party is null)
        {
            await SendPartyResultAsync(peer, WorldSocialState.PartyLeave, string.Empty, 2, cancellationToken).ConfigureAwait(false);
            return;
        }
        await target.Session.SendPacketAsync(Opcode.SMSG_GROUP_UNINVITE, ReadOnlyMemory<byte>.Empty, cancellationToken).ConfigureAwait(false);
        await SendGroupUpdateAsync(party, cancellationToken).ConfigureAwait(false);
    }

    private async Task HandleGroupSetLeaderAsync(
        WorldSessionPeer? peer,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        if (peer is null)
            return;
        var name = PacketReader.ReadCString(payload.Span, 0).Trim();
        var target = socialState.FindByName(name);
        if (target is null || !socialState.TrySetLeader(peer.Character.Guid, target.Character.Guid))
        {
            await SendPartyResultAsync(peer, WorldSocialState.PartyLeave, name, 6, cancellationToken).ConfigureAwait(false);
            return;
        }
        var party = peer.Party;
        if (party is not null)
        {
            foreach (var member in GetOnlinePartyMembers(party))
                await member.Session.SendPacketAsync(Opcode.SMSG_GROUP_SET_LEADER, GroupSocialPacketWriter.BuildGroupSetLeader(target.Character.Name), cancellationToken).ConfigureAwait(false);
            await SendGroupUpdateAsync(party, cancellationToken).ConfigureAwait(false);
        }
    }

    private async Task HandleLootMethodAsync(
        WorldSessionPeer? peer,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        if (peer is null)
            return;
        var method = payload.Span[0];
        var masterGuid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span[1..]);
        if (!socialState.TrySetLootMethod(peer.Character.Guid, method, masterGuid))
        {
            await SendPartyResultAsync(peer, WorldSocialState.PartyInvite, string.Empty, 6, cancellationToken).ConfigureAwait(false);
            return;
        }
        if (peer.Party is not null)
            await SendGroupUpdateAsync(peer.Party, cancellationToken).ConfigureAwait(false);
    }

    private async Task HandleGroupDisbandAsync(
        WorldSessionPeer? peer,
        CancellationToken cancellationToken)
    {
        if (peer?.Party is null)
            return;
        var members = GetOnlinePartyMembers(peer.Party);
        var party = socialState.DisbandParty(peer.Character.Guid);
        if (party is null)
            return;
        foreach (var member in members)
            await member.Session.SendPacketAsync(Opcode.SMSG_GROUP_DESTROYED, ReadOnlyMemory<byte>.Empty, cancellationToken).ConfigureAwait(false);
    }

    private async Task HandleMinimapPingAsync(
        WorldSessionPeer? peer,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        if (peer?.Party is null)
            return;
        var packet = new byte[sizeof(ulong) + sizeof(float) * 2];
        BinaryPrimitives.WriteUInt64LittleEndian(packet, peer.Character.Guid);
        payload[..(sizeof(float) * 2)].CopyTo(packet.AsMemory(sizeof(ulong)));
        foreach (var member in GetOnlinePartyMembers(peer.Party).Where(member => member.Character.Guid != peer.Character.Guid))
            await member.Session.SendPacketAsync(Opcode.MSG_MINIMAP_PING, packet, cancellationToken).ConfigureAwait(false);
    }

    private async Task SendPartyResultAsync(
        WorldSessionPeer peer,
        int operation,
        string name,
        int result,
        CancellationToken cancellationToken) =>
        await peer.Session.SendPacketAsync(
            Opcode.SMSG_PARTY_COMMAND_RESULT,
            GroupSocialPacketWriter.BuildPartyResult(checked((uint)operation), name, checked((uint)result)),
            cancellationToken).ConfigureAwait(false);

    private async Task SendGroupUpdateAsync(WorldPartyState party, CancellationToken cancellationToken)
    {
        var members = GetOnlinePartyMembers(party);
        foreach (var member in members)
        {
            await member.Session.SendPacketAsync(
                Opcode.SMSG_GROUP_LIST,
                GroupSocialPacketWriter.BuildGroupList(member, party, members),
                cancellationToken).ConfigureAwait(false);
            foreach (var other in members.Where(other => other.Character.Guid != member.Character.Guid))
                await member.Session.SendPacketAsync(
                    Opcode.SMSG_PARTY_MEMBER_STATS,
                    GroupSocialPacketWriter.BuildPartyMemberStats(other),
                    cancellationToken).ConfigureAwait(false);
        }
    }

    private IReadOnlyList<WorldSessionPeer> GetOnlinePartyMembers(WorldPartyState party) =>
        party.Members
            .Select(socialState.FindByGuid)
            .Where(member => member is not null)
            .Cast<WorldSessionPeer>()
            .ToArray();

    private async Task<BattleNet.Persistence.Models.AccountRecord?> AuthenticateAsync(
        SessionConnection session,
        TcpClient client,
        CancellationToken cancellationToken)
    {
        var packet = await session.ReadWorldPacketAsync(cancellationToken).ConfigureAwait(false);
        if (packet is null || packet.Value.Opcode != Opcode.CMSG_AUTH_SESSION || packet.Value.Payload.Length < 8)
        {
            await session.SendPacketAsync(Opcode.SMSG_AUTH_RESPONSE, new byte[] { (byte)AuthCode.AuthFailed }, cancellationToken).ConfigureAwait(false);
            return null;
        }

        var payload = packet.Value.Payload;
        var version = BinaryPrimitives.ReadUInt32LittleEndian(payload.AsSpan(0, 4));
        if (version != options.SupportedClient)
        {
            await session.SendPacketAsync(Opcode.SMSG_AUTH_RESPONSE, new byte[] { (byte)AuthCode.AuthVersionMismatch }, cancellationToken).ConfigureAwait(false);
            return null;
        }

        var username = PacketReader.ReadCString(payload, 8);
        var stringEnd = PacketReader.CStringEnd(payload, 8);
        var account = await authRepository.FindAccountAsync(username, cancellationToken).ConfigureAwait(false);
        if (account is null || stringEnd < 0 || payload.Length < stringEnd + 24 || string.IsNullOrWhiteSpace(account.SessionKeyHex))
        {
            await session.SendPacketAsync(Opcode.SMSG_AUTH_RESPONSE, new byte[] { (byte)AuthCode.AuthUnknownAccount }, cancellationToken).ConfigureAwait(false);
            return null;
        }

        var clientSeed = payload.AsSpan(stringEnd, 4);
        var clientDigest = payload.AsSpan(stringEnd + 4, 20);
        var expected = Srp6.CalculateWorldServerProof(
            username,
            clientSeed,
            _serverSeed,
            Convert.FromHexString(account.SessionKeyHex));
        if (!CryptographicOperations.FixedTimeEquals(expected, clientDigest))
        {
            await session.SendPacketAsync(Opcode.SMSG_AUTH_RESPONSE, new byte[] { (byte)AuthCode.AuthIncorrectPassword }, cancellationToken).ConfigureAwait(false);
            return null;
        }

        logger.LogInformation("World session authenticated for {Username} from {Address}", username, client.Client.RemoteEndPoint);
        return account;
    }

    private async Task SendCharacterListAsync(SessionConnection session, int accountId, CancellationToken cancellationToken)
    {
        var characters = await realmRepository.GetCharactersAsync(accountId, options.LocalRealmId, cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(Opcode.SMSG_CHAR_ENUM, CharacterEnumPacketWriter.Build(characters), cancellationToken).ConfigureAwait(false);
    }

    private async Task CreateCharacterAsync(
        SessionConnection session,
        int accountId,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        var result = await CreateCharacterCoreAsync(accountId, payload, cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(Opcode.SMSG_CHAR_CREATE, new[] { (byte)result }, cancellationToken).ConfigureAwait(false);
    }

    private async Task<CharacterCreateCode> CreateCharacterCoreAsync(
        int accountId,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        var nameEnd = PacketReader.CStringEnd(payload.Span, 0);
        if (nameEnd < 0 || payload.Length < nameEnd + 9)
            return CharacterCreateCode.Error;

        var name = PacketReader.ReadCString(payload.Span, 0);
        if (!IsValidCharacterName(name))
            return CharacterCreateCode.Error;

        var race = payload.Span[nameEnd];
        var classId = payload.Span[nameEnd + 1];
        var gender = payload.Span[nameEnd + 2];
        var skin = payload.Span[nameEnd + 3];
        var face = payload.Span[nameEnd + 4];
        var hairStyle = payload.Span[nameEnd + 5];
        var hairColor = payload.Span[nameEnd + 6];
        var facialHair = payload.Span[nameEnd + 7];
        if (!IsSupportedRace(race) || !IsSupportedClass(classId))
            return CharacterCreateCode.Disabled;

        await CharacterMutationLock.WaitAsync(cancellationToken).ConfigureAwait(false);
        try
        {
            var characters = await realmRepository.GetCharactersAsync(accountId, options.LocalRealmId, cancellationToken).ConfigureAwait(false);
            if (characters.Count >= 10)
                return CharacterCreateCode.Failed;
            if (characters.Any(character => string.Equals(character.Name, name, StringComparison.OrdinalIgnoreCase)))
                return CharacterCreateCode.NameInUse;

            var startingLocation = await realmRepository.GetStartingLocationAsync(race, classId, cancellationToken).ConfigureAwait(false);
            if (startingLocation is null)
                return CharacterCreateCode.Failed;

            var request = new BattleNet.Persistence.Models.CharacterCreateRequest(
                accountId,
                options.LocalRealmId,
                name,
                race,
                classId,
                gender,
                skin,
                face,
                hairStyle,
                hairColor,
                facialHair,
                1,
                startingLocation);
            var created = await realmRepository.CreateCharacterAsync(request, cancellationToken).ConfigureAwait(false);
            if (created is null)
                return CharacterCreateCode.Failed;

            var startupState = await BuildStartingStateAsync(race, classId, gender, cancellationToken).ConfigureAwait(false);
            await realmRepository.InitializeCharacterStartupStateAsync(
                created.Guid,
                startupState,
                cancellationToken).ConfigureAwait(false);
            return CharacterCreateCode.Success;
        }
        finally
        {
            CharacterMutationLock.Release();
        }
    }

    private async Task DeleteCharacterAsync(
        SessionConnection session,
        int accountId,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        var guid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        var deleted = await realmRepository.DeleteCharacterAsync(
            accountId,
            options.LocalRealmId,
            guid,
            cancellationToken).ConfigureAwait(false);
        var result = deleted ? CharacterDeleteCode.Success : CharacterDeleteCode.Failed;
        await session.SendPacketAsync(Opcode.SMSG_CHAR_DELETE, new[] { (byte)result }, cancellationToken).ConfigureAwait(false);
    }

    private async Task SendNameQueryAsync(
        SessionConnection session,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        var guid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        var character = await realmRepository.FindCharacterAsync(guid, options.LocalRealmId, cancellationToken).ConfigureAwait(false);
        if (character is not null)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_NAME_QUERY_RESPONSE,
                NameQueryPacketWriter.Build(character),
                cancellationToken).ConfigureAwait(false);
        }
    }

    private async Task SendItemQuerySingleAsync(
        SessionConnection session,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        var entry = BinaryPrimitives.ReadUInt32LittleEndian(payload.Span);
        if (entry == 0)
            return;

        var template = await worldRepository.GetItemTemplateAsync(entry, cancellationToken).ConfigureAwait(false);
        if (template is not null)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_ITEM_QUERY_SINGLE_RESPONSE,
                ItemQueryPacketWriter.BuildSinglePayload(template),
                cancellationToken).ConfigureAwait(false);
        }
    }

    private async Task SendItemQueryMultipleAsync(
        SessionConnection session,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        var requestedCount = BinaryPrimitives.ReadUInt32LittleEndian(payload.Span);
        if (requestedCount == 0 || requestedCount > (payload.Length - sizeof(uint)) / sizeof(uint))
            return;

        var templates = new List<BattleNet.Persistence.Models.ItemTemplateRecord>();
        for (var index = 0; index < requestedCount; index++)
        {
            var entry = BinaryPrimitives.ReadUInt32LittleEndian(
                payload.Span.Slice(sizeof(uint) + (checked((int)index) * sizeof(uint)), sizeof(uint)));
            if (entry == 0)
                continue;

            var template = await worldRepository.GetItemTemplateAsync(entry, cancellationToken).ConfigureAwait(false);
            if (template is not null)
                templates.Add(template);
        }

        foreach (var responsePayload in ItemQueryPacketWriter.BuildMultiplePayloads(templates))
        {
            await session.SendPacketAsync(
                Opcode.SMSG_ITEM_QUERY_MULTIPLE_RESPONSE,
                responsePayload,
                cancellationToken).ConfigureAwait(false);
        }
    }

    private async Task SendCreatureQueryAsync(
        SessionConnection session,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        var entry = BinaryPrimitives.ReadUInt32LittleEndian(payload.Span);
        var template = await worldContentRepository.GetCreatureTemplateAsync(entry, cancellationToken).ConfigureAwait(false);
        if (template is not null)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_CREATURE_QUERY_RESPONSE,
                CreatureQueryPacketWriter.Build(template),
                cancellationToken).ConfigureAwait(false);
        }
    }

    private async Task SendGameObjectQueryAsync(
        SessionConnection session,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        var entry = BinaryPrimitives.ReadUInt32LittleEndian(payload.Span);
        var template = await worldContentRepository.GetGameObjectTemplateAsync(entry, cancellationToken).ConfigureAwait(false);
        if (template is not null)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_GAMEOBJECT_QUERY_RESPONSE,
                GameObjectQueryPacketWriter.Build(template),
                cancellationToken).ConfigureAwait(false);
        }
    }

    private async Task SendQuestQueryAsync(
        SessionConnection session,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        var entry = BinaryPrimitives.ReadUInt32LittleEndian(payload.Span);
        var quest = await worldContentRepository.GetQuestTemplateAsync(entry, cancellationToken).ConfigureAwait(false);
        if (quest is not null)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_QUEST_QUERY_RESPONSE,
                QuestQueryPacketWriter.Build(quest),
                cancellationToken).ConfigureAwait(false);
        }
    }

    private async Task SendQuestGiverStatusAsync(
        SessionConnection session,
        ReadOnlyMemory<byte> payload,
        ulong activeCharacterGuid,
        CancellationToken cancellationToken)
    {
        var guid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        var spawn = await worldContentRepository.GetCreatureSpawnAsync(guid, cancellationToken).ConfigureAwait(false);
        var creature = spawn is null
            ? null
            : await worldContentRepository.GetCreatureTemplateAsync(spawn.Entry, cancellationToken).ConfigureAwait(false);
        if (spawn is null || creature is null)
            return;

        var starterQuests = await worldContentRepository.GetCreatureQuestIdsAsync(
            creature.Entry,
            starter: true,
            cancellationToken).ConfigureAwait(false);
        var finisherQuests = await worldContentRepository.GetCreatureQuestIdsAsync(
            creature.Entry,
            starter: false,
            cancellationToken).ConfigureAwait(false);
        var activeQuests = activeCharacterGuid == 0
            ? Array.Empty<CharacterQuestRecord>()
            : await questRepository.GetCharacterQuestsAsync(activeCharacterGuid, cancellationToken).ConfigureAwait(false);
        var activeQuestIds = activeQuests.Select(quest => quest.QuestId).ToHashSet();
        var status = QuestGiverPacketWriters.None;
        var activeFinisher = finisherQuests.FirstOrDefault(activeQuestIds.Contains);
        if (activeFinisher > 0)
        {
            var activeQuest = await worldContentRepository.GetQuestTemplateAsync(activeFinisher, cancellationToken).ConfigureAwait(false);
            status = activeQuest is not null && await IsQuestCompleteAsync(
                activeCharacterGuid,
                activeQuest,
                cancellationToken).ConfigureAwait(false)
                ? QuestGiverPacketWriters.Reward
                : QuestGiverPacketWriters.Quest;
        }
        else if (starterQuests.Any(quest => !activeQuestIds.Contains(quest)))
        {
            status = QuestGiverPacketWriters.Quest;
        }
        await session.SendPacketAsync(
            Opcode.SMSG_QUESTGIVER_STATUS,
            QuestGiverPacketWriters.BuildStatus(guid, status),
            cancellationToken).ConfigureAwait(false);
    }

    private async Task SendQuestGiverHelloAsync(
        SessionConnection session,
        ReadOnlyMemory<byte> payload,
        ulong activeCharacterGuid,
        CancellationToken cancellationToken)
    {
        var guid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        var spawn = await worldContentRepository.GetCreatureSpawnAsync(guid, cancellationToken).ConfigureAwait(false);
        var creature = spawn is null
            ? null
            : await worldContentRepository.GetCreatureTemplateAsync(spawn.Entry, cancellationToken).ConfigureAwait(false);
        if (spawn is null || creature is null)
            return;

        var starterQuests = await worldContentRepository.GetCreatureQuestIdsAsync(
            creature.Entry,
            starter: true,
            cancellationToken).ConfigureAwait(false);
        var finisherQuests = await worldContentRepository.GetCreatureQuestIdsAsync(
            creature.Entry,
            starter: false,
            cancellationToken).ConfigureAwait(false);
        var finishers = finisherQuests.ToHashSet();
        var activeQuests = activeCharacterGuid == 0
            ? Array.Empty<CharacterQuestRecord>()
            : await questRepository.GetCharacterQuestsAsync(activeCharacterGuid, cancellationToken).ConfigureAwait(false);
        var activeQuestIds = activeQuests.Select(quest => quest.QuestId).ToHashSet();
        var questIds = starterQuests
            .Concat(finisherQuests)
            .Distinct()
            .OrderBy(entry => entry)
            .ToArray();
        var quests = new List<WorldQuestGiverQuest>(questIds.Length);
        foreach (var questId in questIds)
        {
            var quest = await worldContentRepository.GetQuestTemplateAsync(questId, cancellationToken).ConfigureAwait(false);
            if (quest is null)
                continue;

            var questState = QuestGiverPacketWriters.None;
            if (finishers.Contains(questId) && activeQuestIds.Contains(questId))
            {
                questState = await IsQuestCompleteAsync(
                    activeCharacterGuid,
                    quest,
                    cancellationToken).ConfigureAwait(false)
                    ? QuestGiverPacketWriters.Reward
                    : QuestGiverPacketWriters.Quest;
            }
            else if (starterQuests.Contains(questId) && !activeQuestIds.Contains(questId))
            {
                questState = QuestGiverPacketWriters.Quest;
            }
            if (questState == QuestGiverPacketWriters.None)
                continue;

            quests.Add(new WorldQuestGiverQuest(
                quest.Entry,
                questState,
                quest.QuestLevel,
                quest.Title));
        }

        if (quests.Count == 0 && (creature.NpcFlags & 0x1) == 0)
            return;

        await session.SendPacketAsync(
            Opcode.SMSG_QUESTGIVER_QUEST_LIST,
            QuestGiverPacketWriters.BuildList(guid, string.Empty, 0, quests),
            cancellationToken).ConfigureAwait(false);
    }

    private async Task SendQuestGiverDetailsAsync(
        SessionConnection session,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        var guid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        var questEntry = BinaryPrimitives.ReadUInt32LittleEndian(payload.Span[sizeof(ulong)..]);
        var spawn = await worldContentRepository.GetCreatureSpawnAsync(guid, cancellationToken).ConfigureAwait(false);
        var creature = spawn is null
            ? null
            : await worldContentRepository.GetCreatureTemplateAsync(spawn.Entry, cancellationToken).ConfigureAwait(false);
        if (spawn is null || creature is null)
            return;

        var starterQuests = await worldContentRepository.GetCreatureQuestIdsAsync(
            creature.Entry,
            starter: true,
            cancellationToken).ConfigureAwait(false);
        var finisherQuests = await worldContentRepository.GetCreatureQuestIdsAsync(
            creature.Entry,
            starter: false,
            cancellationToken).ConfigureAwait(false);
        if (!starterQuests.Contains(questEntry) && !finisherQuests.Contains(questEntry))
            return;

        var quest = await worldContentRepository.GetQuestTemplateAsync(questEntry, cancellationToken).ConfigureAwait(false);
        if (quest is null)
            return;

        await session.SendPacketAsync(
            Opcode.SMSG_QUESTGIVER_QUEST_DETAILS,
            QuestGiverPacketWriters.BuildDetails(guid, quest),
            cancellationToken).ConfigureAwait(false);
    }

    private async Task<bool> IsQuestCompleteAsync(
        ulong characterGuid,
        WorldQuestTemplate quest,
        CancellationToken cancellationToken)
    {
        var activeQuest = (await questRepository.GetCharacterQuestsAsync(characterGuid, cancellationToken).ConfigureAwait(false))
            .FirstOrDefault(candidate => candidate.QuestId == quest.Entry);
        if (activeQuest is null)
            return false;

        var inventory = await realmRepository.GetCharacterInventoryAsync(characterGuid, cancellationToken).ConfigureAwait(false);
        for (var index = 0; index < quest.Requirements.Count; index++)
        {
            var requirement = quest.Requirements[index];
            if (requirement.CreatureOrGameObjectId > 0)
            {
                var progress = index < activeQuest.Progress.Count ? activeQuest.Progress[index] : 0;
                if (progress < requirement.CreatureOrGameObjectCount)
                    return false;
            }
            else if (requirement.CreatureOrGameObjectId < 0)
            {
                return false;
            }

            if (requirement.ItemId == 0)
                continue;
            var owned = inventory
                .Where(item => item.ItemTemplate == requirement.ItemId)
                .Aggregate(0UL, (total, item) => total + item.StackCount);
            if (owned < requirement.ItemCount)
                return false;
        }
        return true;
    }

    private async Task RecordCreatureKillAsync(
        SessionConnection session,
        ulong characterGuid,
        WorldCreatureSpawn spawn,
        CancellationToken cancellationToken)
    {
        var activeQuests = await questRepository.GetCharacterQuestsAsync(characterGuid, cancellationToken).ConfigureAwait(false);
        foreach (var activeQuest in activeQuests)
        {
            var quest = await worldContentRepository.GetQuestTemplateAsync(activeQuest.QuestId, cancellationToken).ConfigureAwait(false);
            if (quest is null)
                continue;

            var progress = activeQuest.Progress.ToList();
            while (progress.Count < quest.Requirements.Count)
                progress.Add(0);

            var changed = false;
            for (var index = 0; index < quest.Requirements.Count; index++)
            {
                var requirement = quest.Requirements[index];
                if (requirement.CreatureOrGameObjectId != (int)spawn.Entry
                    || progress[index] >= requirement.CreatureOrGameObjectCount)
                    continue;

                progress[index] = Math.Min(
                    requirement.CreatureOrGameObjectCount,
                    checked(progress[index] + 1));
                changed = true;
                var update = new byte[sizeof(uint) * 4 + sizeof(ulong)];
                BinaryPrimitives.WriteUInt32LittleEndian(update, activeQuest.QuestId);
                BinaryPrimitives.WriteUInt32LittleEndian(update.AsSpan(sizeof(uint)), spawn.Entry);
                BinaryPrimitives.WriteUInt32LittleEndian(update.AsSpan(sizeof(uint) * 2), progress[index]);
                BinaryPrimitives.WriteUInt32LittleEndian(update.AsSpan(sizeof(uint) * 3), requirement.CreatureOrGameObjectCount);
                BinaryPrimitives.WriteUInt64LittleEndian(update.AsSpan(sizeof(uint) * 4), 0xF130000000000000UL | spawn.SpawnId);
                await session.SendPacketAsync(
                    Opcode.SMSG_QUESTUPDATE_ADD_KILL,
                    update,
                    cancellationToken).ConfigureAwait(false);
            }

            if (changed)
            {
                await questRepository.UpdateCharacterQuestAsync(
                    activeQuest with { ObjectiveCounts = progress },
                    cancellationToken).ConfigureAwait(false);
                if (await IsQuestCompleteAsync(characterGuid, quest, cancellationToken).ConfigureAwait(false))
                {
                    var complete = new byte[sizeof(uint)];
                    BinaryPrimitives.WriteUInt32LittleEndian(complete, quest.Entry);
                    await session.SendPacketAsync(
                        Opcode.SMSG_QUESTUPDATE_COMPLETE,
                        complete,
                        cancellationToken).ConfigureAwait(false);
                }
            }
        }
    }

    private async Task HandleAttackSwingAsync(
        SessionConnection session,
        ulong activeCharacterGuid,
        PlayerRuntimeState? player,
        ReadOnlyMemory<byte> payload,
        Dictionary<ulong, CreatureRuntimeState> creatures,
        Dictionary<ulong, LootRuntimeState> lootByCreature,
        CancellationToken cancellationToken)
    {
        if (activeCharacterGuid == 0 || player?.Health == 0)
            return;

        var targetGuid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        var spawn = await worldContentRepository.GetCreatureSpawnAsync(targetGuid, cancellationToken).ConfigureAwait(false);
        var creature = spawn is null
            ? null
            : await worldContentRepository.GetCreatureTemplateAsync(spawn.Entry, cancellationToken).ConfigureAwait(false);
        if (spawn is null || creature is null)
            return;

        var character = player?.Character ?? await realmRepository.FindCharacterAsync(
            activeCharacterGuid,
            options.LocalRealmId,
            cancellationToken).ConfigureAwait(false);
        if (character is null)
            return;

        if (!creatures.TryGetValue(targetGuid, out var state))
        {
            state = await CreateCreatureRuntimeAsync(spawn, creature, cancellationToken).ConfigureAwait(false);
            creatures[targetGuid] = state;
        }

        var distanceSquared = DistanceSquared(
            character.PositionX,
            character.PositionY,
            character.PositionZ,
            state.PositionX,
            state.PositionY,
            state.PositionZ);
        if (distanceSquared > 64.0f)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_ATTACKSWING_NOTINRANGE,
                ReadOnlyMemory<byte>.Empty,
                cancellationToken).ConfigureAwait(false);
            return;
        }

        var health = state.Health;
        if (health == 0)
        {
            await HandleAttackStopAsync(session, activeCharacterGuid, targetGuid, cancellationToken).ConfigureAwait(false);
            return;
        }
        if (state.NextMeleeAttack > DateTimeOffset.UtcNow)
            return;

        await session.SendPacketAsync(
            Opcode.SMSG_ATTACKSTART,
            CombatPacketWriter.BuildAttackStart(activeCharacterGuid, targetGuid),
            cancellationToken).ConfigureAwait(false);
        var damage = Math.Max(1u, (uint)(character.Level * 5 + 5));
        var newHealth = damage >= health ? 0 : health - damage;
        state.Health = newHealth;
        state.ThreatTargetGuid = activeCharacterGuid;
        state.NextMeleeAttack = DateTimeOffset.UtcNow.AddMilliseconds(Math.Max(250, state.Template.BaseAttackTime));
        await session.SendPacketAsync(
            Opcode.SMSG_ATTACKERSTATEUPDATE,
            CombatPacketWriter.BuildAttackerStateUpdate(activeCharacterGuid, targetGuid, Math.Min(damage, health)),
            cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            Opcode.SMSG_UPDATE_OBJECT,
            CreatureUpdatePacketWriter.BuildHealthUpdate(targetGuid, newHealth, state.MaxHealth),
            cancellationToken).ConfigureAwait(false);

        if (newHealth == 0)
        {
            await HandleCreatureDefeatedAsync(session, activeCharacterGuid, state, lootByCreature, cancellationToken).ConfigureAwait(false);
            await session.SendPacketAsync(
                Opcode.SMSG_ATTACKSTOP,
                CombatPacketWriter.BuildAttackStop(activeCharacterGuid, targetGuid),
                cancellationToken).ConfigureAwait(false);
        }
    }

    private Task HandleAttackStopAsync(
        SessionConnection session,
        ulong activeCharacterGuid,
        ulong targetGuid,
        CancellationToken cancellationToken)
    {
        if (activeCharacterGuid == 0)
            return Task.CompletedTask;

        return session.SendPacketAsync(
            Opcode.SMSG_ATTACKSTOP,
            CombatPacketWriter.BuildAttackStop(activeCharacterGuid, targetGuid),
            cancellationToken);
    }

    private static float DistanceSquared(
        float x1,
        float y1,
        float z1,
        float x2,
        float y2,
        float z2)
    {
        var x = x1 - x2;
        var y = y1 - y2;
        var z = z1 - z2;
        return (x * x) + (y * y) + (z * z);
    }

    private async Task<CreatureRuntimeState> CreateCreatureRuntimeAsync(
        WorldCreatureSpawn spawn,
        WorldCreatureTemplate template,
        CancellationToken cancellationToken)
    {
        var waypoints = await worldContentRepository.GetCreatureWaypointsAsync(
            spawn.SpawnId,
            template.Entry,
            cancellationToken).ConfigureAwait(false);
        var spells = await worldContentRepository.GetCreatureSpellsAsync(
            template.Entry,
            cancellationToken).ConfigureAwait(false);
        var aiEvents = await worldContentRepository.GetCreatureAiEventsAsync(
            template.Entry,
            cancellationToken).ConfigureAwait(false);
        var scriptIds = aiEvents
            .SelectMany(eventRow => new[] { eventRow.Action1Script, eventRow.Action2Script, eventRow.Action3Script })
            .Where(scriptId => scriptId > 0)
            .Distinct()
            .ToArray();
        var aiScriptRows = await worldContentRepository.GetCreatureAiScriptsAsync(scriptIds, cancellationToken).ConfigureAwait(false);
        var aiScripts = aiScriptRows
            .GroupBy(script => script.Id)
            .ToDictionary(group => group.Key, group => (IReadOnlyList<WorldScriptCommand>)group.ToArray());
        return new CreatureRuntimeState(spawn, template, waypoints, spells, aiEvents, aiScripts);
    }

    private async Task LoadCreatureRuntimeAsync(
        PlayerRuntimeState player,
        Dictionary<ulong, CreatureRuntimeState> creatures,
        CancellationToken cancellationToken)
    {
        var spawns = await worldContentRepository.GetCreatureSpawnsNearAsync(
            checked((int)player.Character.Map),
            player.Character.PositionX,
            player.Character.PositionY,
            player.Character.PositionZ,
            radius: 250,
            limit: 128,
            cancellationToken).ConfigureAwait(false);
        foreach (var spawn in spawns)
        {
            var template = await worldContentRepository.GetCreatureTemplateAsync(spawn.Entry, cancellationToken).ConfigureAwait(false);
            if (template is not null)
                creatures[BuildCreatureGuid(spawn.SpawnId)] = await CreateCreatureRuntimeAsync(spawn, template, cancellationToken).ConfigureAwait(false);
        }
    }

    private async Task UpdateWorldSimulationAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        Dictionary<ulong, CreatureRuntimeState> creatures,
        Dictionary<ulong, LootRuntimeState> lootByCreature,
        CancellationToken cancellationToken)
    {
        var now = DateTimeOffset.UtcNow;
        if (player.Corpse is { } corpse && now >= corpse.ExpiresAt)
        {
            player.Corpse = null;
            player.HasResurrectionSickness = false;
            player.Auras.Remove(2146);
            await session.SendPacketAsync(
                Opcode.SMSG_DESTROY_OBJECT,
                CorpseUpdatePacketWriter.BuildDestroy(corpse.Guid),
                cancellationToken).ConfigureAwait(false);
            await session.SendPacketAsync(
                Opcode.SMSG_UPDATE_AURA_DURATION,
                SpellPacketWriter.BuildAuraDuration(0, 0),
                cancellationToken).ConfigureAwait(false);
        }
        await CompletePendingPlayerSpellCastAsync(
            session,
            player,
            creatures,
            lootByCreature,
            now,
            cancellationToken).ConfigureAwait(false);
        await UpdatePlayerAurasAsync(session, player, now, cancellationToken).ConfigureAwait(false);

        if (player.Health > 0)
        {
            foreach (var state in creatures.Values.ToArray())
            {
                if (state.IsDead)
                {
                    if (state.RespawnAt != default && now >= state.RespawnAt)
                    {
                        state.Health = state.MaxHealth;
                        state.PositionX = state.Spawn.PositionX;
                        state.PositionY = state.Spawn.PositionY;
                        state.PositionZ = state.Spawn.PositionZ;
                        state.Orientation = state.Spawn.Orientation;
                        state.ThreatTargetGuid = 0;
                        state.PendingSpellCast = null;
                        state.RespawnAt = default;
                        await session.SendPacketAsync(
                            Opcode.SMSG_UPDATE_OBJECT,
                            CreatureUpdatePacketWriter.BuildHealthUpdate(BuildCreatureGuid(state.Spawn.SpawnId), state.Health, state.MaxHealth),
                            cancellationToken).ConfigureAwait(false);
                    }
                    continue;
                }

                var distanceSquared = DistanceSquared(
                    state.PositionX,
                    state.PositionY,
                    state.PositionZ,
                    player.Character.PositionX,
                    player.Character.PositionY,
                    player.Character.PositionZ);
                var aggroRange = state.Template.DetectionRange > 0 ? state.Template.DetectionRange : 20;
                var playerFaction = FactionRelations.GetPlayerFactionTemplate(player.Character.Race);
                var canAggro = state.Template.Civilian == 0
                    && state.Template.Type != 8 // Critters never proximity-aggro.
                    && state.Template.LevelMin > 0
                    && FactionRelations.IsHostile(state.Template.Faction, playerFaction);
                if (state.ThreatTargetGuid == 0 && canAggro && distanceSquared <= aggroRange * aggroRange)
                {
                    state.ThreatTargetGuid = player.Character.Guid;
                    await session.SendPacketAsync(
                        Opcode.SMSG_AI_REACTION,
                        CombatPacketWriter.BuildAiReaction(BuildCreatureGuid(state.Spawn.SpawnId), 1),
                        cancellationToken).ConfigureAwait(false);
                }

                if (state.ThreatTargetGuid != 0
                    && DistanceSquared(
                        state.PositionX,
                        state.PositionY,
                        state.PositionZ,
                        state.Spawn.PositionX,
                        state.Spawn.PositionY,
                        state.Spawn.PositionZ) > MathF.Pow(state.Template.LeashRange > 0 ? state.Template.LeashRange : 80, 2))
                {
                    var wasInCombat = state.WasInCombat;
                    state.ThreatTargetGuid = 0;
                    state.NextPatrolMove = DateTimeOffset.MinValue;
                    if (wasInCombat)
                        foreach (var evadeEvent in state.AiEvents.Where(eventRow => eventRow.EventType == 7))
                            await TriggerCreatureAiEventAsync(session, player, state, evadeEvent, creatures, lootByCreature, cancellationToken).ConfigureAwait(false);
                }

                if (state.PendingSpellCast is not null)
                {
                    if (now >= state.PendingSpellCast.CompletesAt)
                    {
                        await CompletePendingCreatureSpellCastAsync(
                            session,
                            player,
                            state,
                            now,
                            cancellationToken).ConfigureAwait(false);
                    }
                    continue;
                }

                await UpdateCreatureAiAsync(
                    session,
                    player,
                    state,
                    creatures,
                    lootByCreature,
                    now,
                    cancellationToken).ConfigureAwait(false);

                if (state.ThreatTargetGuid == player.Character.Guid)
                {
                    var distance = MathF.Sqrt(distanceSquared);
                    if (distance > 2.8f)
                    {
                        if (state.NextPatrolMove <= now)
                        {
                            var startX = state.PositionX;
                            var startY = state.PositionY;
                            var startZ = state.PositionZ;
                            var step = WorldPathfinder.GetNextStep(
                                state,
                                player.Character.PositionX,
                                player.Character.PositionY,
                                player.Character.PositionZ,
                                creatures.Values);
                            var duration = MoveCreatureTo(
                                state,
                                step.X,
                                step.Y,
                                step.Z,
                                speed: Math.Max(2.5f, 7.0f * state.Template.SpeedRun));
                            state.NextPatrolMove = now.AddMilliseconds(duration);
                            logger.LogInformation(
                                "Creature {CreatureGuid:X16} chasing player {PlayerGuid:X16} distance {Distance:F2} duration {Duration}ms",
                                BuildCreatureGuid(state.Spawn.SpawnId),
                                player.Character.Guid,
                                distance,
                                duration);
                            await session.SendPacketAsync(
                                Opcode.SMSG_MONSTER_MOVE,
                                MonsterMovePacketWriter.Build(
                                    BuildCreatureGuid(state.Spawn.SpawnId),
                                    startX,
                                    startY,
                                    startZ,
                                    step.X,
                                    step.Y,
                                    step.Z,
                                    duration),
                                cancellationToken).ConfigureAwait(false);
                        }
                    }
                    else
                    {
                        if (state.NextSpellCast <= now && state.Spells.Count > 0
                            && await TryCreatureSpellAsync(session, player, state, lootByCreature, now, cancellationToken).ConfigureAwait(false))
                        {
                            continue;
                        }

                        if (state.NextMeleeAttack <= now && state.MeleeAttackEnabled && state.FleeUntil <= now)
                            await ApplyCreatureMeleeAsync(session, player, state, lootByCreature, now, cancellationToken).ConfigureAwait(false);
                    }
                }
                else if (state.NextPatrolMove <= now
                    && state.Spawn.MovementType == 2
                    && state.Waypoints.Count > 1)
                {
                    var waypoint = state.Waypoints[state.WaypointIndex % state.Waypoints.Count];
                    var startX = state.PositionX;
                    var startY = state.PositionY;
                    var startZ = state.PositionZ;
                    var step = WorldPathfinder.GetNextStep(state, waypoint.PositionX, waypoint.PositionY, waypoint.PositionZ, creatures.Values);
                    var duration = MoveCreatureTo(state, step.X, step.Y, step.Z, 2.5f);
                    state.WaypointIndex = (state.WaypointIndex + 1) % state.Waypoints.Count;
                    state.NextPatrolMove = now.AddMilliseconds(duration + waypoint.WaitTime);
                    await session.SendPacketAsync(
                        Opcode.SMSG_MONSTER_MOVE,
                        MonsterMovePacketWriter.Build(
                            BuildCreatureGuid(state.Spawn.SpawnId),
                            startX,
                            startY,
                            startZ,
                            step.X,
                            step.Y,
                            step.Z,
                            duration),
                        cancellationToken).ConfigureAwait(false);
                }
                else if (state.NextPatrolMove <= now
                    && state.Spawn.MovementType == 1
                    && state.Spawn.WanderDistance > 0.0f)
                {
                    // AlphaCore's WANDER behavior chooses a point around the
                    // spawn/home position, walks there, then pauses 4..10 s.
                    var angle = Random.Shared.NextSingle() * MathF.Tau;
                    var radius = MathF.Sqrt(Random.Shared.NextSingle())
                        * state.Spawn.WanderDistance;
                    var destinationX = state.Spawn.PositionX + MathF.Cos(angle) * radius;
                    var destinationY = state.Spawn.PositionY + MathF.Sin(angle) * radius;
                    var destinationZ = state.Spawn.PositionZ;
                    var startX = state.PositionX;
                    var startY = state.PositionY;
                    var startZ = state.PositionZ;
                    var step = WorldPathfinder.GetNextStep(
                        state,
                        destinationX,
                        destinationY,
                        destinationZ,
                        creatures.Values);
                    var duration = MoveCreatureTo(state, step.X, step.Y, step.Z, 2.5f);
                    state.NextPatrolMove = now.AddMilliseconds(
                        duration + Random.Shared.Next(4000, 10001));
                    await session.SendPacketAsync(
                        Opcode.SMSG_MONSTER_MOVE,
                        MonsterMovePacketWriter.Build(
                            BuildCreatureGuid(state.Spawn.SpawnId),
                            startX,
                            startY,
                            startZ,
                            step.X,
                            step.Y,
                            step.Z,
                            duration),
                        cancellationToken).ConfigureAwait(false);
                }
            }
        }

        if (player.LastStateSave == default || now - player.LastStateSave >= TimeSpan.FromSeconds(2))
        {
            player.Character = player.Character with
            {
                Health = player.Health,
                Power1 = player.Power,
                PositionX = player.Character.PositionX,
                PositionY = player.Character.PositionY,
                PositionZ = player.Character.PositionZ
            };
            await characterStateRepository.SaveCharacterStateAsync(player.Character, cancellationToken).ConfigureAwait(false);
            socialState.UpdateCharacter(player.Character);
            if (socialState.FindByGuid(player.Character.Guid) is { } peer)
                peer.InstanceToken = socialState.GetInstanceToken(player.Character.Guid, checked((int)player.Character.Map));
            player.LastStateSave = now;
        }
    }

    private async Task UpdatePlayerAurasAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        DateTimeOffset now,
        CancellationToken cancellationToken)
    {
        foreach (var aura in player.Auras.ToArray())
        {
            var runtime = aura.Value;
            if (now >= runtime.ExpiresAt)
            {
                player.Auras.Remove(aura.Key);
                await session.SendPacketAsync(
                    Opcode.SMSG_UPDATE_AURA_DURATION,
                    SpellPacketWriter.BuildAuraDuration(0, 0),
                    cancellationToken).ConfigureAwait(false);
                continue;
            }

            if (now < runtime.NextTickAt)
                continue;

            var effect = runtime.Spell.Effects[runtime.EffectIndex];
            var amount = CalculateSpellEffect(effect, player.Character.Level);
            if (effect.EffectType is 2 or 9)
            {
                player.Health = amount >= player.Health ? 0 : player.Health - amount;
                await session.SendPacketAsync(
                    Opcode.SMSG_UPDATE_OBJECT,
                    PlayerUpdatePacketWriter.BuildHealthUpdate(player.Character.Guid, player.Health, player.MaxHealth),
                    cancellationToken).ConfigureAwait(false);
            }
            else if (effect.EffectType is 10 or 67 or 30)
            {
                player.Health = effect.EffectType == 30
                    ? player.Health
                    : Math.Min(player.MaxHealth, player.Health + amount);
                if (effect.EffectType == 30)
                    player.Power = Math.Min(player.MaxPower, player.Power + amount);
                await session.SendPacketAsync(
                    Opcode.SMSG_UPDATE_OBJECT,
                    effect.EffectType == 30
                        ? PlayerUpdatePacketWriter.BuildPowerUpdate(player.Character.Guid, player.Power, player.MaxPower)
                        : PlayerUpdatePacketWriter.BuildHealthUpdate(player.Character.Guid, player.Health, player.MaxHealth),
                    cancellationToken).ConfigureAwait(false);
            }
            runtime.NextTickAt = now.AddMilliseconds(Math.Max(250, effect.AuraPeriod));
        }
    }

    private async Task UpdateCreatureAiAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        CreatureRuntimeState state,
        Dictionary<ulong, CreatureRuntimeState> creatures,
        Dictionary<ulong, LootRuntimeState> lootByCreature,
        DateTimeOffset now,
        CancellationToken cancellationToken)
    {
        if (state.AiEvents.Count == 0)
            return;

        var inCombat = state.ThreatTargetGuid == player.Character.Guid && player.Health > 0;
        if (!state.WasInCombat)
        {
            foreach (var spawnEvent in state.AiEvents.Where(eventRow => eventRow.EventType == 11))
                await TriggerCreatureAiEventAsync(session, player, state, spawnEvent, creatures, lootByCreature, cancellationToken).ConfigureAwait(false);
        }
        if (inCombat && !state.WasInCombat)
        {
            state.WasInCombat = true;
            foreach (var enterEvent in state.AiEvents.Where(eventRow => eventRow.EventType == 4))
                await TriggerCreatureAiEventAsync(session, player, state, enterEvent, creatures, lootByCreature, cancellationToken).ConfigureAwait(false);
        }
        else if (!inCombat && state.WasInCombat)
        {
            state.WasInCombat = false;
            foreach (var leaveEvent in state.AiEvents.Where(eventRow => eventRow.EventType == 30))
                await TriggerCreatureAiEventAsync(session, player, state, leaveEvent, creatures, lootByCreature, cancellationToken).ConfigureAwait(false);
        }

        var targetDistance = MathF.Sqrt(DistanceSquared(
            state.PositionX,
            state.PositionY,
            state.PositionZ,
            player.Character.PositionX,
            player.Character.PositionY,
            player.Character.PositionZ));
        var healthPercent = state.MaxHealth == 0 ? 0 : (state.Health * 100.0f) / state.MaxHealth;
        foreach (var eventRow in state.AiEvents)
        {
            var matches = eventRow.EventType switch
            {
                0 => inCombat,
                1 => !inCombat,
                2 => healthPercent <= eventRow.EventParam1 && healthPercent >= Math.Max(0, eventRow.EventParam2),
                9 => inCombat && targetDistance >= eventRow.EventParam1 && targetDistance <= eventRow.EventParam2,
                10 => !inCombat && targetDistance <= Math.Max(1, eventRow.EventParam2),
                12 => inCombat && player.MaxHealth > 0 && (player.Health * 100.0f / player.MaxHealth) <= eventRow.EventParam1,
                _ => false
            };
            if (!matches || state.AiOneShotEventsTriggered.Contains(eventRow.Id))
                continue;
            if (!state.AiEventNext.TryGetValue(eventRow.Id, out var next))
            {
                state.AiEventNext[eventRow.Id] = now.AddMilliseconds(RandomDelay(eventRow.EventParam1, eventRow.EventParam2, 250));
                continue;
            }
            if (next > now)
                continue;

            await TriggerCreatureAiEventAsync(session, player, state, eventRow, creatures, lootByCreature, cancellationToken).ConfigureAwait(false);
            var repeat = RandomDelay(eventRow.EventParam3, eventRow.EventParam4, 0);
            if ((eventRow.EventFlags & 1) != 0 && repeat > 0)
                state.AiEventNext[eventRow.Id] = now.AddMilliseconds(repeat);
            else
                state.AiOneShotEventsTriggered.Add(eventRow.Id);
        }
    }

    private async Task TriggerCreatureAiEventAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        CreatureRuntimeState state,
        WorldCreatureAiEvent eventRow,
        Dictionary<ulong, CreatureRuntimeState> creatures,
        Dictionary<ulong, LootRuntimeState> lootByCreature,
        CancellationToken cancellationToken)
    {
        if (state.AiOneShotEventsTriggered.Contains(eventRow.Id))
            return;
        if (eventRow.Chance < 100 && Random.Shared.Next(1, 101) > Math.Max(0, eventRow.Chance))
        {
            if ((eventRow.EventFlags & 1) == 0)
                state.AiOneShotEventsTriggered.Add(eventRow.Id);
            return;
        }
        var scripts = new[] { eventRow.Action1Script, eventRow.Action2Script, eventRow.Action3Script }
            .Where(scriptId => scriptId > 0)
            .ToArray();
        if ((eventRow.EventFlags & 2) != 0 && scripts.Length > 1)
            scripts = [scripts[Random.Shared.Next(scripts.Length)]];
        foreach (var scriptId in scripts)
        {
            if (!state.AiScripts.TryGetValue(scriptId, out var commands))
                continue;
            foreach (var command in commands)
                await ExecuteCreatureAiCommandAsync(session, player, state, command, creatures, lootByCreature, cancellationToken).ConfigureAwait(false);
        }
        if (eventRow.EventType is 6 or 7 or 11 or 19 or 20 or 21 or 22 or 25 or 26 or 30 or 31 or 32)
            state.AiOneShotEventsTriggered.Add(eventRow.Id);
    }

    private async Task ExecuteCreatureAiCommandAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        CreatureRuntimeState state,
        WorldScriptCommand command,
        Dictionary<ulong, CreatureRuntimeState> creatures,
        Dictionary<ulong, LootRuntimeState> lootByCreature,
        CancellationToken cancellationToken)
    {
        var creatureGuid = BuildCreatureGuid(state.Spawn.SpawnId);
        switch (command.Command)
        {
            case 0:
            {
                var textId = command.DataInt != 0 ? command.DataInt : command.DataInt2;
                var broadcast = textId > 0
                    ? await worldContentRepository.GetBroadcastTextAsync(checked((uint)textId), cancellationToken).ConfigureAwait(false)
                    : null;
                var text = !string.IsNullOrWhiteSpace(broadcast?.MaleText)
                    ? broadcast.MaleText
                    : broadcast?.FemaleText;
                if (string.IsNullOrWhiteSpace(text))
                    text = command.Comments;
                if (!string.IsNullOrWhiteSpace(text))
                    await session.SendPacketAsync(
                        Opcode.SMSG_MESSAGECHAT,
                        ChatPacketWriter.BuildMonsterMessage(state.Template.Name, player.Character.Guid, text, checked((byte)Math.Max(0, broadcast?.ChatType ?? command.DataLong)), checked((uint)Math.Max(0, broadcast?.LanguageId ?? 0))),
                        cancellationToken).ConfigureAwait(false);
                break;
            }
            case 1:
                await session.SendPacketAsync(Opcode.SMSG_TEXT_EMOTE, BitConverter.GetBytes(creatureGuid), cancellationToken).ConfigureAwait(false);
                break;
            case 3:
            case 20:
            {
                var x = command.X == 0 && command.Y == 0 && command.Z == 0 ? state.Spawn.PositionX : command.X;
                var y = command.X == 0 && command.Y == 0 && command.Z == 0 ? state.Spawn.PositionY : command.Y;
                var z = command.X == 0 && command.Y == 0 && command.Z == 0 ? state.Spawn.PositionZ : command.Z;
                var startX = state.PositionX;
                var startY = state.PositionY;
                var startZ = state.PositionZ;
                var step = WorldPathfinder.GetNextStep(state, x, y, z, creatures.Values);
                var duration = MoveCreatureTo(state, step.X, step.Y, step.Z, command.Command == 20 && command.DataLong == 1 ? 2.5f : 7.0f);
                await session.SendPacketAsync(
                    Opcode.SMSG_MONSTER_MOVE,
                    MonsterMovePacketWriter.Build(creatureGuid, startX, startY, startZ, step.X, step.Y, step.Z, duration),
                    cancellationToken).ConfigureAwait(false);
                break;
            }
            case 6:
                state.PositionX = command.X;
                state.PositionY = command.Y;
                state.PositionZ = command.Z;
                state.Orientation = command.O;
                break;
            case 10:
            {
                var template = await worldContentRepository.GetCreatureTemplateAsync(checked((uint)Math.Max(0, command.DataLong)), cancellationToken).ConfigureAwait(false);
                if (template is null)
                    break;
                var spawnId = unchecked((uint)Random.Shared.Next(0x40000000, int.MaxValue));
                var spawn = new WorldCreatureSpawn(
                    spawnId,
                    template.Entry,
                    state.Spawn.Map,
                    command.X == 0 && command.Y == 0 && command.Z == 0 ? state.PositionX : command.X,
                    command.X == 0 && command.Y == 0 && command.Z == 0 ? state.PositionY : command.Y,
                    command.X == 0 && command.Y == 0 && command.Z == 0 ? state.PositionZ : command.Z,
                    command.O);
                var summoned = await CreateCreatureRuntimeAsync(spawn, template, cancellationToken).ConfigureAwait(false);
                creatures[BuildCreatureGuid(spawnId)] = summoned;
                await session.SendPacketAsync(Opcode.SMSG_UPDATE_OBJECT, CreatureUpdatePacketWriter.Build(spawn, template), cancellationToken).ConfigureAwait(false);
                break;
            }
            case 14:
                player.Auras.Remove(checked((uint)Math.Max(0, command.DataLong)));
                break;
            case 15:
            case 74:
            {
                var spell = await worldContentRepository.GetSpellDefinitionAsync(checked((uint)Math.Max(0, command.DataLong)), cancellationToken).ConfigureAwait(false);
                if (spell is not null)
                    await ExecuteCreatureSpellAsync(session, player, state, spell, cancellationToken).ConfigureAwait(false);
                break;
            }
            case 17:
            {
                var itemId = checked((uint)Math.Max(0, command.DataLong));
                var count = Math.Max(1, command.DataLong2);
                var mutation = await characterMutationRepository.ApplyInventoryMutationAsync(
                    player.Character.Guid,
                    [new InventoryItemDelta(itemId, checked((uint)count))],
                    Array.Empty<InventoryItemDelta>(),
                    cancellationToken: cancellationToken).ConfigureAwait(false);
                if (mutation is not null)
                    await SendInventoryMutationUpdatesAsync(session, player, mutation, cancellationToken).ConfigureAwait(false);
                break;
            }
            case 18:
                state.Health = 0;
                state.RespawnAt = DateTimeOffset.UtcNow.AddMilliseconds(Math.Max(1, command.DataLong));
                break;
            case 26:
                state.ThreatTargetGuid = player.Character.Guid;
                break;
            case 28:
                break;
            case 29:
            case 50:
            case 72:
            case 75:
                state.ThreatTargetGuid = player.Character.Guid;
                break;
            case 33:
            case 73:
                state.ThreatTargetGuid = 0;
                state.PositionX = state.Spawn.PositionX;
                state.PositionY = state.Spawn.PositionY;
                state.PositionZ = state.Spawn.PositionZ;
                state.Orientation = state.Spawn.Orientation;
                break;
            case 35:
                state.Orientation = command.O;
                break;
            case 37:
                if (socialState.FindByGuid(player.Character.Guid) is { } peer)
                    peer.InstanceToken = socialState.GetInstanceToken(player.Character.Guid, state.Spawn.Map);
                break;
            case 39:
            {
                var genericRows = await worldContentRepository.GetGenericScriptsAsync(
                    [checked((uint)Math.Max(0, command.DataLong))],
                    cancellationToken).ConfigureAwait(false);
                foreach (var genericCommand in genericRows)
                    await ExecuteCreatureAiCommandAsync(session, player, state, genericCommand, creatures, lootByCreature, cancellationToken).ConfigureAwait(false);
                break;
            }
            case 42:
                state.MeleeAttackEnabled = command.DataLong != 0;
                break;
            case 43:
                state.CombatMovementEnabled = command.DataLong != 0;
                break;
            case 44:
                state.AiPhase = command.DataLong;
                break;
            case 45:
                state.AiPhase = Random.Shared.Next(command.DataLong, Math.Max(command.DataLong + 1, command.DataLong2 + 1));
                break;
            case 46:
                state.AiPhase = Random.Shared.Next(command.DataLong, Math.Max(command.DataLong + 1, command.DataLong2 + 1));
                break;
            case 47:
                state.FleeUntil = DateTimeOffset.UtcNow.AddMilliseconds(Math.Max(1000, command.DataLong));
                state.ThreatTargetGuid = 0;
                break;
            case 48:
                await ApplyDamageToPlayerAsync(session, player, creatureGuid, checked((uint)Math.Max(1, command.DataLong)), 0, cancellationToken).ConfigureAwait(false);
                break;
            case 52:
                state.Invincible = command.DataLong != 0;
                break;
            case 60:
                state.NextPatrolMove = DateTimeOffset.MinValue;
                break;
            case 71:
                state.Health = state.MaxHealth;
                state.RespawnAt = default;
                break;
            case 85:
                foreach (var eventRow in state.AiEvents.Where(eventRow => eventRow.EventType == 31
                    && eventRow.EventParam1 == command.DataLong
                    && eventRow.EventParam2 == command.DataLong2))
                    await TriggerCreatureAiEventAsync(session, player, state, eventRow, creatures, lootByCreature, cancellationToken).ConfigureAwait(false);
                break;
            case 86:
                break;
            default:
                break;
        }
    }

    private static int RandomDelay(int min, int max, int fallback)
    {
        min = Math.Max(0, min);
        max = Math.Max(min, max);
        if (max == 0)
            return fallback;
        return Random.Shared.Next(min, checked(max + 1));
    }

    private async Task ApplyCreatureMeleeAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        CreatureRuntimeState state,
        Dictionary<ulong, LootRuntimeState> lootByCreature,
        DateTimeOffset now,
        CancellationToken cancellationToken)
    {
        var creatureGuid = BuildCreatureGuid(state.Spawn.SpawnId);
        var damage = Math.Max(1u, Math.Max(state.Template.LevelMin, state.Template.LevelMax) * 3u + 5u);
        player.Health = damage >= player.Health ? 0 : player.Health - damage;
        state.NextMeleeAttack = now.AddMilliseconds(Math.Max(250, state.Template.BaseAttackTime));
        await session.SendPacketAsync(
            Opcode.SMSG_ATTACKSTART,
            CombatPacketWriter.BuildAttackStart(creatureGuid, player.Character.Guid),
            cancellationToken).ConfigureAwait(false);
        logger.LogInformation(
            "Creature {CreatureGuid:X16} melee player {PlayerGuid:X16} damage {Damage} health {Health}/{MaxHealth}",
            creatureGuid,
            player.Character.Guid,
            damage,
            player.Health,
            player.MaxHealth);
        await session.SendPacketAsync(
            Opcode.SMSG_ATTACKERSTATEUPDATE,
            CombatPacketWriter.BuildAttackerStateUpdate(creatureGuid, player.Character.Guid, damage),
            cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            Opcode.SMSG_DAMAGE_DONE,
            CombatPacketWriter.BuildDamageDone(player.Character.Guid, damage, creatureGuid),
            cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            Opcode.SMSG_UPDATE_OBJECT,
            PlayerUpdatePacketWriter.BuildHealthUpdate(player.Character.Guid, player.Health, player.MaxHealth),
            cancellationToken).ConfigureAwait(false);
        if (player.Health == 0)
        {
            state.ThreatTargetGuid = 0;
            player.PendingSpellCast = null;
            await session.SendPacketAsync(
                Opcode.SMSG_ATTACKSTOP,
                CombatPacketWriter.BuildAttackStop(creatureGuid, player.Character.Guid),
                cancellationToken).ConfigureAwait(false);
            await session.SendPacketAsync(
                Opcode.SMSG_DEATH_NOTIFY,
                BitConverter.GetBytes(creatureGuid),
                cancellationToken).ConfigureAwait(false);
        }
    }

    private async Task<bool> TryCreatureSpellAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        CreatureRuntimeState state,
        Dictionary<ulong, LootRuntimeState> lootByCreature,
        DateTimeOffset now,
        CancellationToken cancellationToken)
    {
        foreach (var creatureSpell in state.Spells.OrderBy(_ => Random.Shared.Next()))
        {
            if (creatureSpell.Probability <= 0 || Random.Shared.NextDouble() * 100 > creatureSpell.Probability)
                continue;
            var spell = await worldContentRepository.GetSpellDefinitionAsync(creatureSpell.SpellId, cancellationToken).ConfigureAwait(false);
            if (spell is null)
                continue;
            var maxRange = spell.MaxRange <= 0 ? 30 : spell.MaxRange;
            var distance = MathF.Sqrt(DistanceSquared(
                state.PositionX,
                state.PositionY,
                state.PositionZ,
                player.Character.PositionX,
                player.Character.PositionY,
                player.Character.PositionZ));
            if (distance > maxRange)
                continue;

            var repeatDelay = Random.Shared.Next(
                Math.Max(1, creatureSpell.DelayRepeatMin),
                Math.Max(Math.Max(1, creatureSpell.DelayRepeatMin) + 1, creatureSpell.DelayRepeatMax + 1));
            state.NextSpellCast = now.AddSeconds(repeatDelay);
            await ExecuteCreatureSpellAsync(
                session,
                player,
                state,
                spell,
                cancellationToken).ConfigureAwait(false);
            return true;
        }

        state.NextSpellCast = now.AddSeconds(1);
        return false;
    }

    private async Task ExecuteCreatureSpellAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        CreatureRuntimeState caster,
        WorldSpellDefinition spell,
        CancellationToken cancellationToken)
    {
        if (caster.PendingSpellCast is not null)
            return;

        var casterGuid = BuildCreatureGuid(caster.Spawn.SpawnId);
        const ushort targetMask = 0x0802;
        await session.SendPacketAsync(
            Opcode.SMSG_SPELL_START,
            SpellPacketWriter.BuildStart(
                casterGuid,
                casterGuid,
                spell.Id,
                0,
                spell.CastTimeMs,
                targetMask,
                player.Character.Guid,
                caster.PositionX,
                caster.PositionY,
                caster.PositionZ,
                player.Character.PositionX,
                player.Character.PositionY,
                player.Character.PositionZ),
            cancellationToken).ConfigureAwait(false);

        var now = DateTimeOffset.UtcNow;
        caster.PendingSpellCast = new PendingCreatureSpellCast(
            spell,
            now.AddMilliseconds(spell.CastTimeMs));
        logger.LogInformation(
            "Creature {CreatureGuid:X16} started spell {SpellId}; completes in {CastTime}ms",
            casterGuid,
            spell.Id,
            spell.CastTimeMs);

        if (spell.CastTimeMs == 0)
        {
            await CompletePendingCreatureSpellCastAsync(
                session,
                player,
                caster,
                now,
                cancellationToken).ConfigureAwait(false);
        }
    }

    private async Task CompletePendingCreatureSpellCastAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        CreatureRuntimeState caster,
        DateTimeOffset now,
        CancellationToken cancellationToken)
    {
        var pending = caster.PendingSpellCast;
        if (pending is null || now < pending.CompletesAt)
            return;

        caster.PendingSpellCast = null;
        if (caster.IsDead || player.Health == 0)
            return;

        var spell = pending.Spell;
        var casterGuid = BuildCreatureGuid(caster.Spawn.SpawnId);
        const ushort targetMask = 0x0802;

        foreach (var indexedEffect in spell.Effects.Select((effect, index) => (effect, index)))
        {
            var effect = indexedEffect.effect;
            var amount = CalculateSpellEffect(effect, checked((byte)Math.Max(1, Math.Max(caster.Template.LevelMin, caster.Template.LevelMax))));
            if (effect.EffectType is 1 or 2 or 9 or 17 or 58)
            {
                await ApplyDamageToPlayerAsync(session, player, casterGuid, amount, spell.Id, cancellationToken).ConfigureAwait(false);
            }
            else if (effect.EffectType is 6 or 27 or 35)
            {
                var duration = spell.DurationMs > 0 ? spell.DurationMs : 30000;
                player.Auras[spell.Id] = new ActiveAuraRuntime(
                    spell,
                    now.AddMilliseconds(duration),
                    now.AddMilliseconds(Math.Max(250, effect.AuraPeriod)),
                    indexedEffect.index);
                await session.SendPacketAsync(
                    Opcode.SMSG_UPDATE_AURA_DURATION,
                    SpellPacketWriter.BuildAuraDuration(0, duration),
                    cancellationToken).ConfigureAwait(false);
            }
        }

        await session.SendPacketAsync(
            Opcode.SMSG_SPELL_GO,
            SpellPacketWriter.BuildGo(
                casterGuid,
                casterGuid,
                spell.Id,
                0,
                targetMask,
                player.Character.Guid,
                caster.PositionX,
                caster.PositionY,
                caster.PositionZ,
                player.Character.PositionX,
                player.Character.PositionY,
                player.Character.PositionZ),
            cancellationToken).ConfigureAwait(false);
        if (player.Health == 0)
            caster.ThreatTargetGuid = 0;
        logger.LogInformation(
            "Creature {CreatureGuid:X16} completed spell {SpellId} against player {PlayerGuid:X16}",
            casterGuid,
            spell.Id,
            player.Character.Guid);
    }

    private async Task HandleCreatureDefeatedAsync(
        SessionConnection session,
        ulong killerGuid,
        CreatureRuntimeState state,
        Dictionary<ulong, LootRuntimeState> lootByCreature,
        CancellationToken cancellationToken)
    {
        if (state.RespawnAt != default)
            return;
        state.PendingSpellCast = null;
        var creatureGuid = BuildCreatureGuid(state.Spawn.SpawnId);
        if (socialState.FindByGuid(killerGuid)?.Runtime is { } killer)
        {
            var localCreatures = new Dictionary<ulong, CreatureRuntimeState> { [creatureGuid] = state };
            foreach (var deathEvent in state.AiEvents.Where(eventRow => eventRow.EventType == 6))
                await TriggerCreatureAiEventAsync(session, killer, state, deathEvent, localCreatures, lootByCreature, cancellationToken).ConfigureAwait(false);
        }
        await RecordCreatureKillAsync(session, killerGuid, state.Spawn, cancellationToken).ConfigureAwait(false);
        lootByCreature[creatureGuid] = await GenerateLootAsync(state, cancellationToken).ConfigureAwait(false);
        DistributeGroupLoot(killerGuid, lootByCreature[creatureGuid]);
        state.ThreatTargetGuid = 0;
        state.RespawnAt = DateTimeOffset.UtcNow.AddSeconds(60);
    }

    private async Task<LootRuntimeState> GenerateLootAsync(
        CreatureRuntimeState state,
        CancellationToken cancellationToken)
    {
        var source = await worldContentRepository.GetCreatureLootAsync(
            state.Template.LootId > 0 ? state.Template.LootId : state.Template.Entry,
            cancellationToken).ConfigureAwait(false);
        var selected = await RollLootEntriesAsync(source, 0, cancellationToken).ConfigureAwait(false);
        var items = new List<LootRuntimeItem>();
        var slot = 0;
        foreach (var entry in selected)
        {
            var template = await worldRepository.GetItemTemplateAsync(entry.ItemEntry, cancellationToken).ConfigureAwait(false);
            if (template is null)
                continue;
            var min = Math.Max(1, entry.MinCountOrReference);
            var max = Math.Max((uint)min, entry.MaxCount);
            var count = (uint)Random.Shared.Next(min, checked((int)Math.Min(int.MaxValue, max) + 1));
            items.Add(new LootRuntimeItem(entry.ItemEntry, count, template.DisplayId, slot++));
        }

        var moneyMin = state.Template.GoldMin;
        var moneyMax = Math.Max(moneyMin, state.Template.GoldMax);
        var money = moneyMax == 0
            ? 0u
            : (uint)Random.Shared.Next(
                checked((int)Math.Min(int.MaxValue - 1, moneyMin)),
                checked((int)Math.Min(int.MaxValue, moneyMax) + 1));
        return new LootRuntimeState(BuildCreatureGuid(state.Spawn.SpawnId), money, items);
    }

    private void DistributeGroupLoot(ulong killerGuid, LootRuntimeState loot)
    {
        var killer = socialState.FindByGuid(killerGuid);
        var party = killer?.Party;
        var recipients = party is null
            ? Array.Empty<WorldSessionPeer>()
            : socialState.GetPartyMembers(killerGuid, nearbyOnly: true).ToArray();
        if (party is null || recipients.Length < 2)
            return;

        var assignments = new Dictionary<ulong, List<LootRuntimeItem>>();
        foreach (var recipient in recipients)
            assignments[recipient.Character.Guid] = [];

        switch (party.LootMethod)
        {
            case 2:
            {
                var master = assignments.ContainsKey(party.MasterLootGuid)
                    ? party.MasterLootGuid
                    : party.LeaderGuid;
                if (!assignments.ContainsKey(master))
                    master = killerGuid;
                assignments[master].AddRange(loot.Items);
                break;
            }
            case 1:
            {
                for (var index = 0; index < loot.Items.Count; index++)
                    assignments[recipients[index % recipients.Length].Character.Guid].Add(loot.Items[index]);
                break;
            }
            default:
                foreach (var recipient in recipients)
                    assignments[recipient.Character.Guid].AddRange(loot.Items);
                break;
        }

        foreach (var recipient in recipients)
        {
            recipient.LootByCreature[loot.ObjectGuid] = new LootRuntimeState(
                loot.ObjectGuid,
                recipient.Character.Guid == killerGuid ? loot.Money : 0,
                assignments[recipient.Character.Guid],
                shared: party.LootMethod == 0);
        }
    }

    private async Task<IReadOnlyList<WorldLootItem>> RollLootEntriesAsync(
        IReadOnlyList<WorldLootItem> entries,
        int depth,
        CancellationToken cancellationToken)
    {
        if (depth > 5 || entries.Count == 0)
            return Array.Empty<WorldLootItem>();

        var result = new List<WorldLootItem>();
        foreach (var group in entries.GroupBy(entry => entry.GroupId))
        {
            var candidates = group.OrderBy(_ => Random.Shared.Next()).ToArray();
            var equalChance = candidates.Count(entry => entry.ChanceOrQuestChance == 0);
            var roll = Random.Shared.NextDouble() * 100;
            foreach (var entry in candidates)
            {
                var chance = Math.Abs(entry.ChanceOrQuestChance);
                if (chance == 0 && group.Key > 0 && equalChance > 0)
                    chance = 100.0f / equalChance;
                if (chance <= 0 || roll >= chance)
                {
                    if (group.Key > 0)
                        roll -= chance;
                    else
                        roll = Random.Shared.NextDouble() * 100;
                    continue;
                }

                if (entry.MinCountOrReference < 0)
                {
                    var reference = await worldContentRepository.GetReferenceLootAsync(
                        checked((uint)-entry.MinCountOrReference),
                        cancellationToken).ConfigureAwait(false);
                    result.AddRange(await RollLootEntriesAsync(reference, depth + 1, cancellationToken).ConfigureAwait(false));
                }
                else
                {
                    result.Add(entry);
                }

                if (group.Key > 0)
                    break;
            }
        }
        return result;
    }

    private async Task HandleLootRequestAsync(
        SessionConnection session,
        PlayerRuntimeState? player,
        ReadOnlyMemory<byte> payload,
        Dictionary<ulong, LootRuntimeState> lootByCreature,
        Dictionary<ulong, CreatureRuntimeState> creatures,
        CancellationToken cancellationToken)
    {
        var objectGuid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        if (player is null || !creatures.TryGetValue(objectGuid, out var creature) || !creature.IsDead)
        {
            await session.SendPacketAsync(Opcode.SMSG_LOOT_RESPONSE, LootPacketWriter.BuildError(objectGuid, 1), cancellationToken).ConfigureAwait(false);
            return;
        }
        if (DistanceSquared(
                player.Character.PositionX,
                player.Character.PositionY,
                player.Character.PositionZ,
                creature.PositionX,
                creature.PositionY,
                creature.PositionZ) > 30 * 30)
        {
            await session.SendPacketAsync(Opcode.SMSG_LOOT_RESPONSE, LootPacketWriter.BuildError(objectGuid, 3), cancellationToken).ConfigureAwait(false);
            return;
        }

        if (!lootByCreature.TryGetValue(objectGuid, out var loot))
        {
            loot = await GenerateLootAsync(creature, cancellationToken).ConfigureAwait(false);
            lootByCreature[objectGuid] = loot;
        }
        player.ActiveLootGuid = objectGuid;
        var templates = new List<ItemTemplateRecord>();
        foreach (var item in loot.Items.Where(item => !item.Claimed))
        {
            var template = await worldRepository.GetItemTemplateAsync(item.ItemEntry, cancellationToken).ConfigureAwait(false);
            if (template is not null)
                templates.Add(template);
        }
        foreach (var response in ItemQueryPacketWriter.BuildMultiplePayloads(templates))
            await session.SendPacketAsync(Opcode.SMSG_ITEM_QUERY_MULTIPLE_RESPONSE, response, cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(Opcode.SMSG_LOOT_RESPONSE, LootPacketWriter.BuildResponse(objectGuid, loot.Money, loot.Items), cancellationToken).ConfigureAwait(false);
    }

    private async Task HandleLootItemAsync(
        SessionConnection session,
        PlayerRuntimeState? player,
        byte slot,
        Dictionary<ulong, LootRuntimeState> lootByCreature,
        CancellationToken cancellationToken)
    {
        if (player is null || !lootByCreature.TryGetValue(player.ActiveLootGuid, out var loot))
            return;
        var visible = loot.Items.Where(item => !item.Claimed).ToArray();
        if (slot >= visible.Length)
            return;
        var item = visible[slot];
        if (loot.Shared && !socialState.TryClaimSharedLoot(loot.ObjectGuid, item.Slot, player.Character.Guid))
            return;
        var mutation = await characterMutationRepository.ApplyInventoryMutationAsync(
            player.Character.Guid,
            [new InventoryItemDelta(item.ItemEntry, item.Count)],
            Array.Empty<InventoryItemDelta>(),
            cancellationToken: cancellationToken).ConfigureAwait(false);
        if (mutation is null)
            return;

        item.Claimed = true;
        await SendInventoryMutationUpdatesAsync(session, player, mutation, cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(Opcode.SMSG_LOOT_REMOVED, LootPacketWriter.BuildRemoved(slot), cancellationToken).ConfigureAwait(false);
    }

    private async Task HandleLootMoneyAsync(
        SessionConnection session,
        PlayerRuntimeState? player,
        Dictionary<ulong, LootRuntimeState> lootByCreature,
        CancellationToken cancellationToken)
    {
        if (player is null || !lootByCreature.TryGetValue(player.ActiveLootGuid, out var loot) || loot.Money == 0)
            return;
        var amount = loot.Money;
        var owner = socialState.FindByGuid(player.Character.Guid);
        var recipients = owner?.Party is null
            ? owner is null ? Array.Empty<WorldSessionPeer>() : [owner]
            : socialState.GetPartyMembers(player.Character.Guid, nearbyOnly: true).ToArray();
        if (recipients.Length == 0)
            return;

        var share = amount / (uint)recipients.Length;
        var remainder = amount % (uint)recipients.Length;
        for (var index = 0; index < recipients.Length; index++)
        {
            var recipient = recipients[index];
            var recipientAmount = share + (index == 0 ? remainder : 0);
            if (recipientAmount == 0)
                continue;
            var mutation = await characterMutationRepository.ApplyInventoryMutationAsync(
                recipient.Character.Guid,
                Array.Empty<InventoryItemDelta>(),
                Array.Empty<InventoryItemDelta>(),
                recipientAmount,
                cancellationToken).ConfigureAwait(false);
            if (mutation is null)
                continue;
            recipient.Character = recipient.Character with { Money = mutation.Money };
            if (recipient.Runtime is not null)
                recipient.Runtime.Character = recipient.Runtime.Character with { Money = mutation.Money };
            if (recipient.Character.Guid == player.Character.Guid)
                player.Character = player.Character with { Money = mutation.Money };
            await recipient.Session.SendPacketAsync(
                Opcode.SMSG_UPDATE_OBJECT,
                PlayerUpdatePacketWriter.BuildMoneyUpdate(recipient.Character.Guid, mutation.Money),
                cancellationToken).ConfigureAwait(false);
            await recipient.Session.SendPacketAsync(Opcode.SMSG_LOOT_CLEAR_MONEY, ReadOnlyMemory<byte>.Empty, cancellationToken).ConfigureAwait(false);
        }
        foreach (var recipient in recipients)
            if (recipient.LootByCreature.TryGetValue(player.ActiveLootGuid, out var recipientLoot))
                recipientLoot.Money = 0;
    }

    private async Task HandleLootReleaseAsync(
        SessionConnection session,
        PlayerRuntimeState? player,
        ReadOnlyMemory<byte> payload,
        Dictionary<ulong, LootRuntimeState> lootByCreature,
        CancellationToken cancellationToken)
    {
        if (player is null)
            return;
        var objectGuid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        if (player.ActiveLootGuid != objectGuid)
            return;
        player.ActiveLootGuid = 0;
        await session.SendPacketAsync(Opcode.SMSG_LOOT_RELEASE_RESPONSE, LootPacketWriter.BuildRelease(objectGuid), cancellationToken).ConfigureAwait(false);
        if (lootByCreature.TryGetValue(objectGuid, out var loot)
            && loot.Money == 0
            && loot.Items.All(item => item.Claimed))
        {
            lootByCreature.Remove(objectGuid);
        }
    }

    private async Task SendInventoryMutationUpdatesAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        CharacterInventoryMutationResult mutation,
        CancellationToken cancellationToken)
    {
        foreach (var itemGuid in mutation.RemovedItemGuids)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_DESTROY_OBJECT,
                InventoryPacketWriter.BuildDestroy(itemGuid),
                cancellationToken).ConfigureAwait(false);
        }
        foreach (var item in mutation.UpdatedItems)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_UPDATE_OBJECT,
                InventoryPacketWriter.BuildStackCountUpdate(item),
                cancellationToken).ConfigureAwait(false);
        }

        var templates = new List<ItemTemplateRecord>();
        foreach (var item in mutation.AddedItems)
        {
            var template = await worldRepository.GetItemTemplateAsync(item.ItemTemplate, cancellationToken).ConfigureAwait(false);
            if (template is not null)
                templates.Add(template);
        }
        foreach (var response in ItemQueryPacketWriter.BuildMultiplePayloads(templates))
            await session.SendPacketAsync(Opcode.SMSG_ITEM_QUERY_MULTIPLE_RESPONSE, response, cancellationToken).ConfigureAwait(false);
        foreach (var item in mutation.AddedItems)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_UPDATE_OBJECT,
                InventoryPacketWriter.Build(item, player.Character),
                cancellationToken).ConfigureAwait(false);
            await session.SendPacketAsync(
                Opcode.SMSG_ITEM_PUSH_RESULT,
                LootPacketWriter.BuildItemPush(item.Guid, item.ItemTemplate, checked((byte)item.Bag)),
                cancellationToken).ConfigureAwait(false);
        }
        if (mutation.Money != player.Character.Money)
        {
            player.Character = player.Character with { Money = mutation.Money };
            await session.SendPacketAsync(
                Opcode.SMSG_UPDATE_OBJECT,
                PlayerUpdatePacketWriter.BuildMoneyUpdate(player.Character.Guid, mutation.Money),
                cancellationToken).ConfigureAwait(false);
        }
    }

    private async Task HandlePlayerMovementAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        ReadOnlyMemory<byte> payload,
        Opcode opcode,
        CancellationToken cancellationToken)
    {
        if (payload.Length < 48 || player.Health == 0)
            return;
        var span = payload.Span;
        var positionX = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(span[24..28]));
        var positionY = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(span[28..32]));
        var positionZ = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(span[32..36]));
        var orientation = BitConverter.Int32BitsToSingle(BinaryPrimitives.ReadInt32LittleEndian(span[36..40]));
        if (DistanceSquared(player.Character.PositionX, player.Character.PositionY, player.Character.PositionZ, positionX, positionY, positionZ) > 10000)
            return;

        player.Character = player.Character with
        {
            PositionX = positionX,
            PositionY = positionY,
            PositionZ = positionZ,
            Orientation = orientation
        };
        socialState.UpdateCharacter(player.Character);
        await session.SendPacketAsync(
            opcode,
            MovementPacketWriter.Build(player.Character.Guid, positionX, positionY, positionZ, orientation),
            cancellationToken).ConfigureAwait(false);
    }

    private async Task HandleUseItemAsync(
        SessionConnection session,
        PlayerRuntimeState? player,
        ulong selectedTargetGuid,
        ReadOnlyMemory<byte> payload,
        Dictionary<ulong, CreatureRuntimeState> creatures,
        Dictionary<ulong, LootRuntimeState> lootByCreature,
        CancellationToken cancellationToken)
    {
        if (player is null || payload.Length < 5)
            return;
        var span = payload.Span;
        var bag = span[0];
        var slot = span[1];
        var spellSlot = span[2];
        var targetMask = BinaryPrimitives.ReadUInt16LittleEndian(span[3..5]);
        var inventory = await realmRepository.GetCharacterInventoryAsync(
            player.Character.Guid,
            cancellationToken).ConfigureAwait(false);
        var item = inventory.FirstOrDefault(candidate =>
            candidate.Bag == bag && candidate.Slot == slot);
        if (item is null)
            return;
        var template = await worldRepository.GetItemTemplateAsync(
            item.ItemTemplate,
            cancellationToken).ConfigureAwait(false);
        if (template is null)
            return;
        var itemSpell = spellSlot < template.Spells.Count
            ? template.Spells[spellSlot] : null;
        if (itemSpell is null || itemSpell.SpellId <= 0 || itemSpell.Trigger != 0)
            itemSpell = template.Spells.FirstOrDefault(candidate =>
                candidate.SpellId > 0 && candidate.Trigger == 0);
        if (itemSpell is null)
            return;

        // CMSG_USE_ITEM has a three-byte item prefix before the same target
        // mask/tail consumed by CMSG_CAST_SPELL.  Normalize it to that layout.
        var castPayload = new byte[sizeof(uint) + sizeof(ushort) + payload.Length - 5];
        BinaryPrimitives.WriteUInt32LittleEndian(castPayload, checked((uint)itemSpell.SpellId));
        BinaryPrimitives.WriteUInt16LittleEndian(castPayload.AsSpan(sizeof(uint)), targetMask);
        payload.Span[5..].CopyTo(castPayload.AsSpan(sizeof(uint) + sizeof(ushort)));
        await HandleCastSpellAsync(
            session,
            player,
            selectedTargetGuid,
            castPayload,
            creatures,
            lootByCreature,
            cancellationToken,
            requireKnownSpell: false,
            consumedItemEntry: item.ItemTemplate).ConfigureAwait(false);
    }

    private async Task HandleCastSpellAsync(
        SessionConnection session,
        PlayerRuntimeState? player,
        ulong selectedTargetGuid,
        ReadOnlyMemory<byte> payload,
        Dictionary<ulong, CreatureRuntimeState> creatures,
        Dictionary<ulong, LootRuntimeState> lootByCreature,
        CancellationToken cancellationToken,
        bool requireKnownSpell = true,
        uint consumedItemEntry = 0)
    {
        if (player is null)
            return;
        var spellId = BinaryPrimitives.ReadUInt32LittleEndian(payload.Span);
        var targetMask = BinaryPrimitives.ReadUInt16LittleEndian(payload.Span[sizeof(uint)..]);
        var spell = await worldContentRepository.GetSpellDefinitionAsync(spellId, cancellationToken).ConfigureAwait(false);
        if (spell is null || (requireKnownSpell && !player.KnownSpells.Contains(spellId)))
        {
            await session.SendPacketAsync(Opcode.SMSG_CAST_RESULT, SpellPacketWriter.BuildCastResult(spellId, false, 0x23), cancellationToken).ConfigureAwait(false);
            return;
        }
        if (player.Health == 0)
        {
            await session.SendPacketAsync(Opcode.SMSG_CAST_RESULT, SpellPacketWriter.BuildCastResult(spellId, false, 0x0A), cancellationToken).ConfigureAwait(false);
            return;
        }
        if (player.PendingSpellCast is not null)
        {
            await session.SendPacketAsync(Opcode.SMSG_CAST_RESULT, SpellPacketWriter.BuildCastResult(spellId, false, 0x25), cancellationToken).ConfigureAwait(false);
            return;
        }
        if (player.SpellCooldowns.TryGetValue(spellId, out var cooldownUntil) && cooldownUntil > DateTimeOffset.UtcNow)
        {
            await session.SendPacketAsync(Opcode.SMSG_CAST_RESULT, SpellPacketWriter.BuildCastResult(spellId, false, 0x25), cancellationToken).ConfigureAwait(false);
            return;
        }
        if (spell.ManaCost > player.Power)
        {
            await session.SendPacketAsync(Opcode.SMSG_CAST_RESULT, SpellPacketWriter.BuildCastResult(spellId, false, 0x2C), cancellationToken).ConfigureAwait(false);
            return;
        }

        var targetGuid = player.Character.Guid;
        if ((targetMask & 0x0802) != 0 && payload.Length >= sizeof(uint) + sizeof(ushort) + sizeof(ulong))
            targetGuid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span[6..14]);
        else if (selectedTargetGuid != 0 && SpellUsesSelectedUnit(spell))
            targetGuid = selectedTargetGuid;

        logger.LogInformation(
            "Cast spell {SpellId} mask 0x{TargetMask:X4} selected {SelectedTargetGuid:X16} resolved {TargetGuid:X16}",
            spellId,
            targetMask,
            selectedTargetGuid,
            targetGuid);

        CreatureRuntimeState? targetCreature = null;
        if (targetGuid != player.Character.Guid)
        {
            if (!creatures.TryGetValue(targetGuid, out targetCreature))
            {
                var spawn = await worldContentRepository.GetCreatureSpawnAsync(targetGuid, cancellationToken).ConfigureAwait(false);
                var template = spawn is null
                    ? null
                    : await worldContentRepository.GetCreatureTemplateAsync(spawn.Entry, cancellationToken).ConfigureAwait(false);
                if (spawn is not null && template is not null)
                {
                    targetCreature = await CreateCreatureRuntimeAsync(spawn, template, cancellationToken).ConfigureAwait(false);
                    creatures[targetGuid] = targetCreature;
                }
            }
            if (targetCreature is null || targetCreature.IsDead)
            {
                await session.SendPacketAsync(Opcode.SMSG_CAST_RESULT, SpellPacketWriter.BuildCastResult(spellId, false, 0x3F), cancellationToken).ConfigureAwait(false);
                return;
            }
            var targetX = targetCreature.PositionX;
            var targetY = targetCreature.PositionY;
            var targetZ = targetCreature.PositionZ;
            if (spell.MaxRange > 0 && DistanceSquared(player.Character.PositionX, player.Character.PositionY, player.Character.PositionZ, targetX, targetY, targetZ) > (spell.MaxRange * spell.MaxRange))
            {
                await session.SendPacketAsync(Opcode.SMSG_CAST_RESULT, SpellPacketWriter.BuildCastResult(spellId, false, 0x36), cancellationToken).ConfigureAwait(false);
                return;
            }
        }

        var effectiveMask = targetGuid == player.Character.Guid && targetMask == 0 ? (ushort)0 : targetMask;
        if (targetGuid != player.Character.Guid && (effectiveMask & 0x0802) == 0)
            effectiveMask |= 0x0802;
        var now = DateTimeOffset.UtcNow;
        await session.SendPacketAsync(Opcode.SMSG_CAST_RESULT, SpellPacketWriter.BuildCastResult(spellId, true), cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            Opcode.SMSG_SPELL_START,
            SpellPacketWriter.BuildStart(
                player.Character.Guid,
                player.Character.Guid,
                spellId,
                0,
                spell.CastTimeMs,
                effectiveMask,
                targetGuid,
                player.Character.PositionX,
                player.Character.PositionY,
                player.Character.PositionZ,
                targetCreature?.PositionX ?? player.Character.PositionX,
                targetCreature?.PositionY ?? player.Character.PositionY,
                targetCreature?.PositionZ ?? player.Character.PositionZ),
            cancellationToken).ConfigureAwait(false);
        player.PendingSpellCast = new PendingPlayerSpellCast(
            spell,
            effectiveMask,
            targetGuid,
            now.AddMilliseconds(Math.Max(0, spell.CastTimeMs)),
            consumedItemEntry);
        logger.LogInformation(
            "Spell {SpellId} started by {CasterGuid:X16} target {TargetGuid:X16}; completes in {CastTime}ms",
            spell.Id,
            player.Character.Guid,
            targetGuid,
            spell.CastTimeMs);
        if (spell.CastTimeMs <= 0)
            await CompletePendingPlayerSpellCastAsync(
                session,
                player,
                creatures,
                lootByCreature,
                now,
                cancellationToken).ConfigureAwait(false);
    }

    private async Task CompletePendingPlayerSpellCastAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        Dictionary<ulong, CreatureRuntimeState> creatures,
        Dictionary<ulong, LootRuntimeState> lootByCreature,
        DateTimeOffset now,
        CancellationToken cancellationToken)
    {
        var pending = player.PendingSpellCast;
        if (pending is null || now < pending.CompletesAt)
            return;
        player.PendingSpellCast = null;

        if (player.Health == 0)
            return;

        var spell = pending.Spell;
        CreatureRuntimeState? targetCreature = null;
        if (pending.TargetGuid != player.Character.Guid)
        {
            creatures.TryGetValue(pending.TargetGuid, out targetCreature);
            if (targetCreature is null || targetCreature.IsDead)
            {
                await session.SendPacketAsync(
                    Opcode.SMSG_SPELL_FAILURE,
                    SpellPacketWriter.BuildFailure(player.Character.Guid, spell.Id, 0x3F),
                    cancellationToken).ConfigureAwait(false);
                return;
            }
        }
        if (spell.ManaCost > player.Power)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_SPELL_FAILURE,
                SpellPacketWriter.BuildFailure(player.Character.Guid, spell.Id, 0x2C),
                cancellationToken).ConfigureAwait(false);
            return;
        }

        var cooldownMs = Math.Max(spell.RecoveryTime, spell.CategoryRecoveryTime);
        if (cooldownMs > 0)
            player.SpellCooldowns[spell.Id] = now.AddMilliseconds(cooldownMs);
        if (spell.ManaCost > 0)
        {
            player.Power -= spell.ManaCost;
            await session.SendPacketAsync(
                Opcode.SMSG_UPDATE_OBJECT,
                PlayerUpdatePacketWriter.BuildPowerUpdate(player.Character.Guid, player.Power, player.MaxPower),
                cancellationToken).ConfigureAwait(false);
        }
        logger.LogInformation(
            "Spell {SpellId} completed by {CasterGuid:X16} target {TargetGuid:X16}; mana {Power}/{MaxPower}",
            spell.Id,
            player.Character.Guid,
            pending.TargetGuid,
            player.Power,
            player.MaxPower);
        if (cooldownMs > 0)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_SPELL_COOLDOWN,
                SpellPacketWriter.BuildCooldown(spell.Id, player.Character.Guid, cooldownMs),
                cancellationToken).ConfigureAwait(false);
        }

        await ExecuteSpellEffectsAsync(
            session,
            player,
            targetCreature,
            spell,
            player.Character.Guid,
            pending.TargetGuid,
            lootByCreature,
            cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            Opcode.SMSG_SPELL_GO,
            SpellPacketWriter.BuildGo(
                player.Character.Guid,
                player.Character.Guid,
                spell.Id,
                0,
                pending.TargetMask,
                pending.TargetGuid,
                player.Character.PositionX,
                player.Character.PositionY,
                player.Character.PositionZ,
                targetCreature?.PositionX ?? player.Character.PositionX,
                targetCreature?.PositionY ?? player.Character.PositionY,
                targetCreature?.PositionZ ?? player.Character.PositionZ),
            cancellationToken).ConfigureAwait(false);

        if (pending.ConsumedItemEntry != 0U)
        {
            var mutation = await characterMutationRepository.ApplyInventoryMutationAsync(
                player.Character.Guid,
                Array.Empty<InventoryItemDelta>(),
                [new InventoryItemDelta(pending.ConsumedItemEntry, 1)],
                cancellationToken: cancellationToken).ConfigureAwait(false);
            if (mutation is not null)
                await SendInventoryMutationUpdatesAsync(
                    session,
                    player,
                    mutation,
                    cancellationToken).ConfigureAwait(false);
        }
    }

    private async Task ExecuteSpellEffectsAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        CreatureRuntimeState? targetCreature,
        WorldSpellDefinition spell,
        ulong casterGuid,
        ulong targetGuid,
        Dictionary<ulong, LootRuntimeState> lootByCreature,
        CancellationToken cancellationToken)
    {
        foreach (var indexedEffect in spell.Effects.Select((effect, index) => (effect, index)))
        {
            var effect = indexedEffect.effect;
            var amount = CalculateSpellEffect(effect, player.Character.Level);
            switch (effect.EffectType)
            {
                case 1:
                    if (targetCreature is not null)
                        amount = targetCreature.Health;
                    goto case 2;
                case 2:
                case 9:
                case 17:
                case 58:
                    if (targetCreature is not null)
                    {
                        await ApplyDamageToCreatureAsync(session, player.Character.Guid, targetCreature, amount, spell.Id, lootByCreature, cancellationToken).ConfigureAwait(false);
                        if (effect.EffectType == 9)
                            player.Health = Math.Min(player.MaxHealth, player.Health + amount);
                    }
                    else if (targetGuid == player.Character.Guid)
                    {
                        player.Health = amount >= player.Health ? 0 : player.Health - amount;
                    }
                    break;
                case 10:
                case 67:
                case 43:
                    if (targetGuid == player.Character.Guid)
                        player.Health = Math.Min(player.MaxHealth, player.Health + amount);
                    break;
                case 30:
                    if (targetGuid == player.Character.Guid)
                        player.Power = Math.Min(player.MaxPower, player.Power + amount);
                    break;
                case 6:
                case 27:
                case 35:
                    if (targetGuid == player.Character.Guid)
                    {
                        var duration = spell.DurationMs > 0 ? spell.DurationMs : 30000;
                        var period = Math.Max(250, effect.AuraPeriod);
                        player.Auras[spell.Id] = new ActiveAuraRuntime(
                            spell,
                            DateTimeOffset.UtcNow.AddMilliseconds(duration),
                            DateTimeOffset.UtcNow.AddMilliseconds(period),
                            indexedEffect.index);
                        await session.SendPacketAsync(
                            Opcode.SMSG_UPDATE_AURA_DURATION,
                            SpellPacketWriter.BuildAuraDuration(0, duration),
                            cancellationToken).ConfigureAwait(false);
                    }
                    break;
                case 24 when effect.ItemType > 0:
                    var mutation = await characterMutationRepository.ApplyInventoryMutationAsync(
                        player.Character.Guid,
                        [new InventoryItemDelta(checked((uint)effect.ItemType), 1)],
                        Array.Empty<InventoryItemDelta>(),
                        cancellationToken: cancellationToken).ConfigureAwait(false);
                    if (mutation is not null)
                        await SendInventoryMutationUpdatesAsync(session, player, mutation, cancellationToken).ConfigureAwait(false);
                    break;
                case 64 when effect.TriggerSpell > 0:
                    var triggered = await worldContentRepository.GetSpellDefinitionAsync(effect.TriggerSpell, cancellationToken).ConfigureAwait(false);
                    if (triggered is not null)
                    {
                        await ExecuteSpellEffectsAsync(
                            session,
                            player,
                            targetCreature,
                            triggered,
                            casterGuid,
                            targetGuid,
                            lootByCreature,
                            cancellationToken).ConfigureAwait(false);
                    }
                    break;
            }

            if (targetGuid == player.Character.Guid && effect.EffectType is 2 or 9 or 10 or 43 or 67 or 30)
            {
                await session.SendPacketAsync(
                    Opcode.SMSG_UPDATE_OBJECT,
                    effect.EffectType == 30
                        ? PlayerUpdatePacketWriter.BuildPowerUpdate(player.Character.Guid, player.Power, player.MaxPower)
                        : PlayerUpdatePacketWriter.BuildHealthUpdate(player.Character.Guid, player.Health, player.MaxHealth),
                    cancellationToken).ConfigureAwait(false);
            }
        }
    }

    private async Task ApplyDamageToCreatureAsync(
        SessionConnection session,
        ulong attackerGuid,
        CreatureRuntimeState state,
        uint damage,
        uint spellId,
        Dictionary<ulong, LootRuntimeState> lootByCreature,
        CancellationToken cancellationToken)
    {
        if (state.IsDead)
            return;
        if (state.Invincible)
            return;
        var actualDamage = Math.Min(damage, state.Health);
        state.Health -= actualDamage;
        state.ThreatTargetGuid = attackerGuid;
        var creatureGuid = BuildCreatureGuid(state.Spawn.SpawnId);
        logger.LogInformation(
            "Creature {CreatureGuid:X16} took {Damage} from {AttackerGuid:X16} spell {SpellId}; health {Health}/{MaxHealth}, threat acquired",
            creatureGuid,
            actualDamage,
            attackerGuid,
            spellId,
            state.Health,
            state.MaxHealth);
        if (spellId > 0 && socialState.FindByGuid(attackerGuid)?.Runtime is { } attacker)
        {
            var localCreatures = new Dictionary<ulong, CreatureRuntimeState> { [creatureGuid] = state };
            foreach (var spellHitEvent in state.AiEvents.Where(eventRow => eventRow.EventType == 8
                && (eventRow.EventParam1 == 0 || eventRow.EventParam1 == spellId)))
                await TriggerCreatureAiEventAsync(session, attacker, state, spellHitEvent, localCreatures, lootByCreature, cancellationToken).ConfigureAwait(false);
        }
        await session.SendPacketAsync(
            Opcode.SMSG_ATTACKERSTATEUPDATE,
            CombatPacketWriter.BuildAttackerStateUpdate(attackerGuid, creatureGuid, actualDamage),
            cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            Opcode.SMSG_DAMAGE_DONE,
            CombatPacketWriter.BuildDamageDone(creatureGuid, actualDamage, attackerGuid, spellId),
            cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            Opcode.SMSG_UPDATE_OBJECT,
            CreatureUpdatePacketWriter.BuildHealthUpdate(creatureGuid, state.Health, state.MaxHealth),
            cancellationToken).ConfigureAwait(false);
        if (state.IsDead)
        {
            await HandleCreatureDefeatedAsync(session, attackerGuid, state, lootByCreature, cancellationToken).ConfigureAwait(false);
            await session.SendPacketAsync(
                Opcode.SMSG_ATTACKSTOP,
                CombatPacketWriter.BuildAttackStop(attackerGuid, creatureGuid),
                cancellationToken).ConfigureAwait(false);
        }
    }

    private async Task ApplyDamageToPlayerAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        ulong attackerGuid,
        uint damage,
        uint spellId,
        CancellationToken cancellationToken)
    {
        if (player.Health == 0)
            return;
        var actualDamage = Math.Min(damage, player.Health);
        player.Health -= actualDamage;
        await session.SendPacketAsync(
            Opcode.SMSG_ATTACKERSTATEUPDATE,
            CombatPacketWriter.BuildAttackerStateUpdate(attackerGuid, player.Character.Guid, actualDamage),
            cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            Opcode.SMSG_DAMAGE_DONE,
            CombatPacketWriter.BuildDamageDone(player.Character.Guid, actualDamage, attackerGuid, spellId),
            cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            Opcode.SMSG_UPDATE_OBJECT,
            PlayerUpdatePacketWriter.BuildHealthUpdate(player.Character.Guid, player.Health, player.MaxHealth),
            cancellationToken).ConfigureAwait(false);
        if (player.Health == 0)
        {
            player.PendingSpellCast = null;
            await session.SendPacketAsync(
                Opcode.SMSG_ATTACKSTOP,
                CombatPacketWriter.BuildAttackStop(attackerGuid, player.Character.Guid),
                cancellationToken).ConfigureAwait(false);
            await session.SendPacketAsync(
                Opcode.SMSG_DEATH_NOTIFY,
                BitConverter.GetBytes(attackerGuid),
                cancellationToken).ConfigureAwait(false);
        }
    }

    private static uint CalculateSpellEffect(WorldSpellEffect effect, byte level)
    {
        uint value = checked((uint)Math.Max(0, effect.BasePoints + 1));
        var dieRoll = effect.DieSides <= 0 ? 1 : Random.Shared.Next(1, effect.DieSides + 1);
        value += checked((uint)(Math.Max(0, effect.BaseDice) * Math.Max(1, dieRoll)));
        value += checked((uint)Math.Max(0, MathF.Round(Math.Max(0, level - 1) * effect.RealPointsPerLevel)));
        return Math.Max(1u, value);
    }

    private static bool SpellUsesSelectedUnit(WorldSpellDefinition spell)
    {
        static bool IsSelectedUnitTarget(int target) => target is
            6 or  // TARGET_ENEMY_UNIT
            21 or // TARGET_SELECTED_FRIEND
            25 or // TARGET_UNIT
            35 or // TARGET_SINGLE_PARTY
            36;   // TARGET_HOSTILE_UNIT_SELECTION

        return spell.Effects.Any(effect =>
            effect.EffectType != 0 &&
            (IsSelectedUnitTarget(effect.ImplicitTargetA) ||
             IsSelectedUnitTarget(effect.ImplicitTargetB)));
    }

    private static ulong BuildCreatureGuid(uint spawnId) => 0xF130000000000000UL | spawnId;

    private static uint MoveCreatureTo(CreatureRuntimeState state, float destinationX, float destinationY, float destinationZ, float speed)
    {
        var startX = state.PositionX;
        var startY = state.PositionY;
        var startZ = state.PositionZ;
        var distance = MathF.Sqrt(DistanceSquared(startX, startY, startZ, destinationX, destinationY, destinationZ));
        state.Orientation = MathF.Atan2(destinationY - startY, destinationX - startX);
        state.PositionX = destinationX;
        state.PositionY = destinationY;
        state.PositionZ = destinationZ;
        return (uint)Math.Max(250, MathF.Round(distance / Math.Max(0.1f, speed) * 1000));
    }

    private async Task AcceptQuestAsync(
        SessionConnection session,
        ReadOnlyMemory<byte> payload,
        ulong activeCharacterGuid,
        CancellationToken cancellationToken)
    {
        if (activeCharacterGuid == 0)
            return;

        var guid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        var questEntry = BinaryPrimitives.ReadUInt32LittleEndian(payload.Span[sizeof(ulong)..]);
        var spawn = await worldContentRepository.GetCreatureSpawnAsync(guid, cancellationToken).ConfigureAwait(false);
        var creature = spawn is null
            ? null
            : await worldContentRepository.GetCreatureTemplateAsync(spawn.Entry, cancellationToken).ConfigureAwait(false);
        if (spawn is null || creature is null)
            return;

        var starterQuests = await worldContentRepository.GetCreatureQuestIdsAsync(
            creature.Entry,
            starter: true,
            cancellationToken).ConfigureAwait(false);
        if (!starterQuests.Contains(questEntry))
            return;

        var quest = await worldContentRepository.GetQuestTemplateAsync(questEntry, cancellationToken).ConfigureAwait(false);
        if (quest is null)
            return;

        var accepted = await questRepository.AcceptCharacterQuestAsync(
            new CharacterQuestRecord(activeCharacterGuid, questEntry),
            cancellationToken).ConfigureAwait(false);
        if (!accepted)
            return;

        await session.SendPacketAsync(
            Opcode.SMSG_QUEST_QUERY_RESPONSE,
            QuestQueryPacketWriter.Build(quest),
            cancellationToken).ConfigureAwait(false);
    }

    private async Task CompleteQuestAsync(
        SessionConnection session,
        ReadOnlyMemory<byte> payload,
        ulong activeCharacterGuid,
        CancellationToken cancellationToken)
    {
        if (activeCharacterGuid == 0)
            return;

        var guid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        var questEntry = BinaryPrimitives.ReadUInt32LittleEndian(payload.Span[sizeof(ulong)..]);
        var quest = await GetRelatedQuestAsync(guid, questEntry, starter: false, cancellationToken).ConfigureAwait(false);
        if (quest is null)
            return;

        var activeQuests = await questRepository.GetCharacterQuestsAsync(activeCharacterGuid, cancellationToken).ConfigureAwait(false);
        if (!activeQuests.Any(activeQuest => activeQuest.QuestId == questEntry))
            return;

        var complete = await IsQuestCompleteAsync(activeCharacterGuid, quest, cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            complete ? Opcode.SMSG_QUESTGIVER_OFFER_REWARD : Opcode.SMSG_QUESTGIVER_REQUEST_ITEMS,
            complete
                ? QuestGiverPacketWriters.BuildOfferReward(guid, quest)
                : QuestGiverPacketWriters.BuildRequestItems(guid, quest, complete: false),
            cancellationToken).ConfigureAwait(false);
    }

    private async Task RequestQuestRewardAsync(
        SessionConnection session,
        ReadOnlyMemory<byte> payload,
        ulong activeCharacterGuid,
        CancellationToken cancellationToken)
    {
        if (activeCharacterGuid == 0)
            return;

        var guid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        var questEntry = BinaryPrimitives.ReadUInt32LittleEndian(payload.Span[sizeof(ulong)..]);
        var quest = await GetRelatedQuestAsync(guid, questEntry, starter: false, cancellationToken).ConfigureAwait(false);
        if (quest is null)
            return;

        var activeQuests = await questRepository.GetCharacterQuestsAsync(activeCharacterGuid, cancellationToken).ConfigureAwait(false);
        if (!activeQuests.Any(activeQuest => activeQuest.QuestId == questEntry)
            || !await IsQuestCompleteAsync(activeCharacterGuid, quest, cancellationToken).ConfigureAwait(false))
            return;

        await session.SendPacketAsync(
            Opcode.SMSG_QUESTGIVER_OFFER_REWARD,
            QuestGiverPacketWriters.BuildOfferReward(guid, quest),
            cancellationToken).ConfigureAwait(false);
    }

    private async Task ChooseQuestRewardAsync(
        SessionConnection session,
        ReadOnlyMemory<byte> payload,
        ulong activeCharacterGuid,
        PlayerRuntimeState? player,
        CancellationToken cancellationToken)
    {
        if (activeCharacterGuid == 0)
            return;

        var guid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        var questEntry = BinaryPrimitives.ReadUInt32LittleEndian(payload.Span[sizeof(ulong)..]);
        var rewardChoice = BinaryPrimitives.ReadUInt32LittleEndian(payload.Span[(sizeof(ulong) + sizeof(uint))..]);
        var quest = await GetRelatedQuestAsync(guid, questEntry, starter: false, cancellationToken).ConfigureAwait(false);
        if (quest is null)
            return;

        var activeQuests = await questRepository.GetCharacterQuestsAsync(activeCharacterGuid, cancellationToken).ConfigureAwait(false);
        if (!activeQuests.Any(activeQuest => activeQuest.QuestId == questEntry)
            || !await IsQuestCompleteAsync(activeCharacterGuid, quest, cancellationToken).ConfigureAwait(false))
            return;

        var rewards = quest.RewardItemIds
            .Select((item, index) => (ItemId: item, Count: index < quest.RewardItemCounts.Count ? quest.RewardItemCounts[index] : 0))
            .Where(reward => reward.ItemId > 0)
            .Select(reward => (reward.ItemId, Math.Max(1u, reward.Count)))
            .ToList();
        var hasChoiceRewards = quest.RewardChoiceItemIds.Any(item => item > 0);
        if (hasChoiceRewards && (rewardChoice >= quest.RewardChoiceItemIds.Count
            || quest.RewardChoiceItemIds[checked((int)rewardChoice)] == 0))
        {
            await session.SendPacketAsync(
                Opcode.SMSG_QUESTGIVER_OFFER_REWARD,
                QuestGiverPacketWriters.BuildOfferReward(guid, quest),
                cancellationToken).ConfigureAwait(false);
            return;
        }
        if (rewardChoice < quest.RewardChoiceItemIds.Count && quest.RewardChoiceItemIds[checked((int)rewardChoice)] > 0)
        {
            var index = checked((int)rewardChoice);
            var count = index < quest.RewardChoiceItemCounts.Count
                ? quest.RewardChoiceItemCounts[index]
                : 0;
            rewards.Add((quest.RewardChoiceItemIds[index], Math.Max(1u, count)));
        }

        var removals = quest.Requirements
            .Where(requirement => requirement.ItemId > 0 && requirement.ItemCount > 0)
            .Select(requirement => new InventoryItemDelta(requirement.ItemId, requirement.ItemCount))
            .ToArray();
        var additions = rewards
            .Select(reward => new InventoryItemDelta(reward.ItemId, reward.Item2))
            .ToArray();
        var mutation = await characterMutationRepository.ApplyInventoryMutationAsync(
            activeCharacterGuid,
            additions,
            removals,
            quest.RewardOrRequiredMoney > 0 ? quest.RewardOrRequiredMoney : 0,
            cancellationToken).ConfigureAwait(false);
        if (mutation is null)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_QUESTGIVER_OFFER_REWARD,
                QuestGiverPacketWriters.BuildOfferReward(guid, quest),
                cancellationToken).ConfigureAwait(false);
            return;
        }

        await questRepository.RemoveCharacterQuestAsync(activeCharacterGuid, questEntry, cancellationToken).ConfigureAwait(false);
        foreach (var item in mutation.RemovedItemGuids)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_DESTROY_OBJECT,
                InventoryPacketWriter.BuildDestroy(item),
                cancellationToken).ConfigureAwait(false);
        }
        foreach (var item in mutation.UpdatedItems)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_UPDATE_OBJECT,
                InventoryPacketWriter.BuildStackCountUpdate(item),
                cancellationToken).ConfigureAwait(false);
        }
        if (mutation.AddedItems.Count > 0)
        {
            var addedTemplates = new List<BattleNet.Persistence.Models.ItemTemplateRecord>();
            foreach (var item in mutation.AddedItems)
            {
                var template = await worldRepository.GetItemTemplateAsync(item.ItemTemplate, cancellationToken).ConfigureAwait(false);
                if (template is not null)
                    addedTemplates.Add(template);
            }
            foreach (var responsePayload in ItemQueryPacketWriter.BuildMultiplePayloads(addedTemplates))
            {
                await session.SendPacketAsync(
                    Opcode.SMSG_ITEM_QUERY_MULTIPLE_RESPONSE,
                    responsePayload,
                    cancellationToken).ConfigureAwait(false);
            }

            var owner = await realmRepository.FindCharacterAsync(
                activeCharacterGuid,
                options.LocalRealmId,
                cancellationToken).ConfigureAwait(false);
            if (owner is not null)
            {
                foreach (var item in mutation.AddedItems)
                {
                    await session.SendPacketAsync(
                        Opcode.SMSG_UPDATE_OBJECT,
                        InventoryPacketWriter.Build(item, owner),
                        cancellationToken).ConfigureAwait(false);
                }
            }
        }
        if (quest.RewardOrRequiredMoney > 0)
        {
            if (player is not null)
                player.Character = player.Character with { Money = mutation.Money };
            await session.SendPacketAsync(
                Opcode.SMSG_UPDATE_OBJECT,
                PlayerUpdatePacketWriter.BuildMoneyUpdate(activeCharacterGuid, mutation.Money),
                cancellationToken).ConfigureAwait(false);
        }
        await session.SendPacketAsync(
            Opcode.SMSG_QUESTGIVER_QUEST_COMPLETE,
            QuestGiverPacketWriters.BuildComplete(questEntry, rewards, quest.RewardOrRequiredMoney > 0 ? checked((uint)quest.RewardOrRequiredMoney) : 0),
            cancellationToken).ConfigureAwait(false);
    }

    private async Task<WorldQuestTemplate?> GetRelatedQuestAsync(
        ulong guid,
        uint questEntry,
        bool starter,
        CancellationToken cancellationToken)
    {
        var spawn = await worldContentRepository.GetCreatureSpawnAsync(guid, cancellationToken).ConfigureAwait(false);
        var creature = spawn is null
            ? null
            : await worldContentRepository.GetCreatureTemplateAsync(spawn.Entry, cancellationToken).ConfigureAwait(false);
        if (spawn is null || creature is null)
            return null;

        var relatedQuests = await worldContentRepository.GetCreatureQuestIdsAsync(
            creature.Entry,
            starter,
            cancellationToken).ConfigureAwait(false);
        if (!relatedQuests.Contains(questEntry))
            return null;

        return await worldContentRepository.GetQuestTemplateAsync(questEntry, cancellationToken).ConfigureAwait(false);
    }

    private async Task RemoveQuestAsync(
        SessionConnection session,
        ReadOnlyMemory<byte> payload,
        ulong activeCharacterGuid,
        CancellationToken cancellationToken)
    {
        if (activeCharacterGuid == 0)
            return;

        var slotOrQuestId = BinaryPrimitives.ReadUInt32LittleEndian(payload.Span);
        var quests = await questRepository.GetCharacterQuestsAsync(activeCharacterGuid, cancellationToken).ConfigureAwait(false);
        var questId = quests.FirstOrDefault(quest => quest.QuestId == slotOrQuestId)?.QuestId ??
            (slotOrQuestId < quests.Count ? quests[checked((int)slotOrQuestId)].QuestId : 0);
        if (questId != 0)
        {
            await questRepository.RemoveCharacterQuestAsync(activeCharacterGuid, questId, cancellationToken).ConfigureAwait(false);
        }
    }

    private async Task SendVendorInventoryAsync(
        SessionConnection session,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        var guid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        var spawn = await worldContentRepository.GetCreatureSpawnAsync(guid, cancellationToken).ConfigureAwait(false);
        var creature = spawn is null
            ? null
            : await worldContentRepository.GetCreatureTemplateAsync(spawn.Entry, cancellationToken).ConfigureAwait(false);
        if (spawn is null || creature is null || (creature.NpcFlags & 0x1) == 0)
            return;

        var items = await worldContentRepository.GetVendorItemsAsync(
            creature.Entry,
            creature.VendorId,
            cancellationToken).ConfigureAwait(false);
        var itemTemplates = new List<BattleNet.Persistence.Models.ItemTemplateRecord>(items.Count);
        foreach (var item in items)
        {
            var template = await worldRepository.GetItemTemplateAsync(item.ItemEntry, cancellationToken).ConfigureAwait(false);
            if (template is not null)
                itemTemplates.Add(template);
        }

        foreach (var responsePayload in ItemQueryPacketWriter.BuildMultiplePayloads(itemTemplates))
        {
            await session.SendPacketAsync(
                Opcode.SMSG_ITEM_QUERY_MULTIPLE_RESPONSE,
                responsePayload,
                cancellationToken).ConfigureAwait(false);
        }

        await session.SendPacketAsync(
            Opcode.SMSG_LIST_INVENTORY,
            VendorPacketWriter.BuildInventory(guid, items),
            cancellationToken).ConfigureAwait(false);
    }

    private Task SendQueryTimeAsync(SessionConnection session, CancellationToken cancellationToken)
    {
        var payload = new byte[sizeof(uint)];
        BinaryPrimitives.WriteUInt32LittleEndian(
            payload,
            checked((uint)DateTimeOffset.UtcNow.ToUnixTimeSeconds()));
        return session.SendPacketAsync(Opcode.SMSG_QUERY_TIME_RESPONSE, payload, cancellationToken);
    }

    private async Task HandleRepopRequestAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        Dictionary<ulong, CreatureRuntimeState> creatures,
        CancellationToken cancellationToken)
    {
        if (player.Health != 0 || player.PendingTeleport is not null)
            return;

        if (player.Corpse is not null)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_DESTROY_OBJECT,
                CorpseUpdatePacketWriter.BuildDestroy(player.Corpse.Guid),
                cancellationToken).ConfigureAwait(false);
        }

        var raceDefinition = await dbcRepository.GetRaceDefinitionAsync(
            player.Character.Race,
            cancellationToken).ConfigureAwait(false);
        var displayId = raceDefinition is null
            ? 0u
            : player.Character.Gender == 0
                ? raceDefinition.MaleDisplayId
                : raceDefinition.FemaleDisplayId;
        var corpseItems = new uint[19];
        var inventory = await realmRepository.GetCharacterInventoryAsync(
            player.Character.Guid,
            cancellationToken).ConfigureAwait(false);
        foreach (var item in inventory.Where(item => item.Bag == 23 && item.Slot < corpseItems.Length))
        {
            var template = await worldRepository.GetItemTemplateAsync(
                item.ItemTemplate,
                cancellationToken).ConfigureAwait(false);
            if (template is not null)
            {
                corpseItems[item.Slot] = template.DisplayId |
                    ((template.InventoryType & 0xFFu) << 24);
            }
        }

        var corpseGuid = 0xF101000000000000UL |
            unchecked((ulong)Interlocked.Increment(ref _nextCorpseLowGuid));
        player.Corpse = new PlayerCorpseRuntime(
            corpseGuid,
            player.Character.Guid,
            player.Character.Map,
            player.Character.PositionX,
            player.Character.PositionY,
            player.Character.PositionZ,
            player.Character.Orientation,
            displayId,
            PlayerUpdatePacketWriter.GetNativeScale(
                player.Character.Race,
                player.Character.Gender),
            PackCharacterBytes(
                player.Character.Skin,
                player.Character.Gender,
                player.Character.Race,
                0),
            PackCharacterBytes(
                player.Character.FacialHair,
                player.Character.HairColor,
                player.Character.HairStyle,
                player.Character.Face),
            player.Character.GuildId,
            player.Character.Level,
            corpseItems,
            DateTimeOffset.UtcNow.AddMinutes(10));
        await session.SendPacketAsync(
            Opcode.SMSG_UPDATE_OBJECT,
            CorpseUpdatePacketWriter.Build(player.Corpse),
            cancellationToken).ConfigureAwait(false);

        foreach (var creature in creatures.Values)
        {
            creature.ThreatTargetGuid = 0;
            creature.PendingSpellCast = null;
            creature.WasInCombat = false;
        }
        player.PendingSpellCast = null;
        player.ActiveLootGuid = 0;

        var deathbind = await realmRepository.GetStartingLocationAsync(
            player.Character.Race,
            player.Character.Class,
            cancellationToken).ConfigureAwait(false) ?? new StartingLocation(
                player.Character.Map,
                player.Character.Zone,
                player.Character.PositionX,
                player.Character.PositionY,
                player.Character.PositionZ,
                player.Character.Orientation);
        var isLongTeleport = deathbind.Map != player.Character.Map;
        player.PendingTeleport = new PendingPlayerTeleport(
            deathbind.Map,
            deathbind.Zone,
            deathbind.PositionX,
            deathbind.PositionY,
            deathbind.PositionZ,
            deathbind.Orientation,
            IsResurrection: true,
            IsLongTeleport: isLongTeleport);

        if (isLongTeleport)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_TRANSFER_PENDING,
                ReadOnlyMemory<byte>.Empty,
                cancellationToken).ConfigureAwait(false);
            await session.SendPacketAsync(
                Opcode.SMSG_NEW_WORLD,
                BuildNewWorldPayload(player.Character with
                {
                    Map = deathbind.Map,
                    Zone = deathbind.Zone,
                    PositionX = deathbind.PositionX,
                    PositionY = deathbind.PositionY,
                    PositionZ = deathbind.PositionZ,
                    Orientation = deathbind.Orientation
                }),
                cancellationToken).ConfigureAwait(false);
        }
        else
        {
            await session.SendPacketAsync(
                Opcode.MSG_MOVE_TELEPORT_ACK,
                PlayerTeleportPacketWriter.Build(
                    deathbind.PositionX,
                    deathbind.PositionY,
                    deathbind.PositionZ,
                    deathbind.Orientation),
                cancellationToken).ConfigureAwait(false);
        }

        logger.LogInformation(
            "Player {PlayerGuid:X16} released spirit; corpse {CorpseGuid:X16} at map {CorpseMap} ({CorpseX:F2}, {CorpseY:F2}, {CorpseZ:F2}), deathbind map {BindMap} ({BindX:F2}, {BindY:F2}, {BindZ:F2})",
            player.Character.Guid,
            corpseGuid,
            player.Corpse.Map,
            player.Corpse.PositionX,
            player.Corpse.PositionY,
            player.Corpse.PositionZ,
            deathbind.Map,
            deathbind.PositionX,
            deathbind.PositionY,
            deathbind.PositionZ);
    }

    private async Task CompletePlayerTeleportAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        Dictionary<ulong, CreatureRuntimeState> creatures,
        bool longTeleportAcknowledgement,
        CancellationToken cancellationToken)
    {
        var teleport = player.PendingTeleport;
        if (teleport is null || teleport.IsLongTeleport != longTeleportAcknowledgement)
            return;

        foreach (var creatureGuid in creatures.Keys)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_DESTROY_OBJECT,
                CorpseUpdatePacketWriter.BuildDestroy(creatureGuid),
                cancellationToken).ConfigureAwait(false);
        }
        creatures.Clear();

        player.Character = player.Character with
        {
            Map = teleport.Map,
            Zone = teleport.Zone,
            PositionX = teleport.PositionX,
            PositionY = teleport.PositionY,
            PositionZ = teleport.PositionZ,
            Orientation = teleport.Orientation
        };
        if (teleport.IsResurrection)
        {
            player.Health = player.MaxHealth;
            // Alpha respawn restores mana and energy, while rage starts empty.
            player.Power = player.Character.Class == 1 ? 0u : player.MaxPower;
            player.HasResurrectionSickness = true;
        }
        player.PendingTeleport = null;
        player.Character = player.Character with
        {
            Health = player.Health,
            Power1 = player.Power
        };
        socialState.UpdateCharacter(player.Character);

        await session.SendPacketAsync(
            Opcode.SMSG_UPDATE_OBJECT,
            PlayerUpdatePacketWriter.BuildHealthUpdate(
                player.Character.Guid,
                player.Health,
                player.MaxHealth),
            cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            Opcode.SMSG_UPDATE_OBJECT,
            PlayerUpdatePacketWriter.BuildPowerUpdate(
                player.Character.Guid,
                player.Power,
                player.MaxPower),
            cancellationToken).ConfigureAwait(false);

        if (teleport.IsResurrection)
            await ApplyResurrectionSicknessAsync(session, player, cancellationToken).ConfigureAwait(false);

        await LoadCreatureRuntimeAsync(player, creatures, cancellationToken).ConfigureAwait(false);
        foreach (var state in creatures.Values)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_UPDATE_OBJECT,
                CreatureUpdatePacketWriter.Build(state.Spawn, state.Template),
                cancellationToken).ConfigureAwait(false);
            await session.SendPacketAsync(
                Opcode.SMSG_UPDATE_OBJECT,
                CreatureUpdatePacketWriter.BuildHealthUpdate(
                    BuildCreatureGuid(state.Spawn.SpawnId),
                    state.Health,
                    state.MaxHealth),
                cancellationToken).ConfigureAwait(false);
        }

        await characterStateRepository.SaveCharacterStateAsync(
            player.Character,
            cancellationToken).ConfigureAwait(false);
        logger.LogInformation(
            "Player {PlayerGuid:X16} completed {TeleportKind} teleport and is alive at map {Map} ({X:F2}, {Y:F2}, {Z:F2})",
            player.Character.Guid,
            longTeleportAcknowledgement ? "worldport" : "near",
            player.Character.Map,
            player.Character.PositionX,
            player.Character.PositionY,
            player.Character.PositionZ);
    }

    private async Task ApplyResurrectionSicknessAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        CancellationToken cancellationToken)
    {
        const uint resurrectionSicknessSpell = 2146;
        var spell = await worldContentRepository.GetSpellDefinitionAsync(
            resurrectionSicknessSpell,
            cancellationToken).ConfigureAwait(false);
        if (spell is null)
            return;
        var auraEffect = spell.Effects
            .Select((effect, index) => (Effect: effect, Index: index))
            .FirstOrDefault(value => value.Effect.EffectType is 6 or 27 or 35);
        var duration = spell.DurationMs > 0 ? spell.DurationMs : 600000;
        player.Auras[resurrectionSicknessSpell] = new ActiveAuraRuntime(
            spell,
            DateTimeOffset.UtcNow.AddMilliseconds(duration),
            DateTimeOffset.MaxValue,
            auraEffect.Index);
        await session.SendPacketAsync(
            Opcode.SMSG_SPELL_GO,
            SpellPacketWriter.BuildGo(
                player.Character.Guid,
                player.Character.Guid,
                resurrectionSicknessSpell,
                0,
                0x0802,
                player.Character.Guid,
                player.Character.PositionX,
                player.Character.PositionY,
                player.Character.PositionZ,
                player.Character.PositionX,
                player.Character.PositionY,
                player.Character.PositionZ),
            cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            Opcode.SMSG_UPDATE_AURA_DURATION,
            SpellPacketWriter.BuildAuraDuration(0, duration),
            cancellationToken).ConfigureAwait(false);
    }

    private async Task HandleReclaimCorpseAsync(
        SessionConnection session,
        PlayerRuntimeState player,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        var corpseGuid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        var corpse = player.Corpse;
        if (corpse is null || corpse.Guid != corpseGuid ||
            corpse.OwnerGuid != player.Character.Guid ||
            corpse.Map != player.Character.Map ||
            DistanceSquared(
                corpse.PositionX,
                corpse.PositionY,
                corpse.PositionZ,
                player.Character.PositionX,
                player.Character.PositionY,
                player.Character.PositionZ) > 25.0f)
        {
            return;
        }

        player.Corpse = null;
        player.HasResurrectionSickness = false;
        player.Auras.Remove(2146);
        await session.SendPacketAsync(
            Opcode.SMSG_DESTROY_OBJECT,
            CorpseUpdatePacketWriter.BuildDestroy(corpseGuid),
            cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            Opcode.SMSG_UPDATE_AURA_DURATION,
            SpellPacketWriter.BuildAuraDuration(0, 0),
            cancellationToken).ConfigureAwait(false);
        logger.LogInformation(
            "Player {PlayerGuid:X16} reclaimed corpse {CorpseGuid:X16}",
            player.Character.Guid,
            corpseGuid);
    }

    private static uint PackCharacterBytes(byte first, byte second, byte third, byte fourth) =>
        first | ((uint)second << 8) | ((uint)third << 16) | ((uint)fourth << 24);

    private async Task<ulong?> LoginCharacterAsync(
        SessionConnection session,
        int accountId,
        ReadOnlyMemory<byte> payload,
        CancellationToken cancellationToken)
    {
        var guid = BinaryPrimitives.ReadUInt64LittleEndian(payload.Span);
        var characters = await realmRepository.GetCharactersAsync(accountId, options.LocalRealmId, cancellationToken).ConfigureAwait(false);
        var character = characters.FirstOrDefault(candidate => candidate.Guid == guid);
        if (character is null)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_CHARACTER_LOGIN_FAILED,
                new[] { (byte)CharacterLoginCode.Failed },
                cancellationToken).ConfigureAwait(false);
            return null;
        }

        await session.SendPacketAsync(Opcode.SMSG_TRANSFER_PENDING, ReadOnlyMemory<byte>.Empty, cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            Opcode.SMSG_NEW_WORLD,
            BuildNewWorldPayload(character),
            cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            Opcode.SMSG_LOGIN_SETTIMESPEED,
            BuildLoginTimeSpeedPayload(),
            cancellationToken).ConfigureAwait(false);

        var spells = await realmRepository.GetCharacterSpellsAsync(character.Guid, cancellationToken).ConfigureAwait(false);
        var actionButtons = await realmRepository.GetActionButtonsAsync(character.Guid, cancellationToken).ConfigureAwait(false);
        var spellButtons = await realmRepository.GetSpellButtonsAsync(character.Guid, cancellationToken).ConfigureAwait(false);
        var inventory = await realmRepository.GetCharacterInventoryAsync(character.Guid, cancellationToken).ConfigureAwait(false);
        var itemTemplates = new List<BattleNet.Persistence.Models.ItemTemplateRecord>();
        foreach (var itemTemplateId in inventory.Select(item => item.ItemTemplate).Distinct())
        {
            var itemTemplate = await worldRepository.GetItemTemplateAsync(itemTemplateId, cancellationToken).ConfigureAwait(false);
            if (itemTemplate is not null)
                itemTemplates.Add(itemTemplate);
        }
        await session.SendPacketAsync(
            Opcode.SMSG_INITIAL_SPELLS,
            InitialSpellPacketWriter.Build(spells, spellButtons),
            cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            Opcode.SMSG_ACTION_BUTTONS,
            ActionButtonPacketWriter.Build(actionButtons),
            cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            Opcode.SMSG_NAME_QUERY_RESPONSE,
            NameQueryPacketWriter.Build(character),
            cancellationToken).ConfigureAwait(false);
        foreach (var responsePayload in ItemQueryPacketWriter.BuildMultiplePayloads(itemTemplates))
        {
            await session.SendPacketAsync(
                Opcode.SMSG_ITEM_QUERY_MULTIPLE_RESPONSE,
                responsePayload,
                cancellationToken).ConfigureAwait(false);
        }
        var raceDefinition = await dbcRepository.GetRaceDefinitionAsync(character.Race, cancellationToken).ConfigureAwait(false);
        await session.SendPacketAsync(
            Opcode.SMSG_UPDATE_OBJECT,
            PlayerUpdatePacketWriter.Build(
                character,
                raceDefinition,
                inventory),
            cancellationToken).ConfigureAwait(false);

        foreach (var item in inventory)
        {
            await session.SendPacketAsync(
                Opcode.SMSG_UPDATE_OBJECT,
                InventoryPacketWriter.Build(item, character),
                cancellationToken).ConfigureAwait(false);
        }

        var activeQuests = await questRepository.GetCharacterQuestsAsync(character.Guid, cancellationToken).ConfigureAwait(false);
        foreach (var activeQuest in activeQuests.Where(quest => !quest.Rewarded))
        {
            var quest = await worldContentRepository.GetQuestTemplateAsync(activeQuest.QuestId, cancellationToken).ConfigureAwait(false);
            if (quest is not null)
            {
                await session.SendPacketAsync(
                    Opcode.SMSG_QUEST_QUERY_RESPONSE,
                    QuestQueryPacketWriter.Build(quest),
                    cancellationToken).ConfigureAwait(false);
            }
        }

        var nearbySpawns = await worldContentRepository.GetCreatureSpawnsNearAsync(
            checked((int)character.Map),
            character.PositionX,
            character.PositionY,
            character.PositionZ,
            radius: 250,
            limit: 128,
            cancellationToken).ConfigureAwait(false);
        foreach (var spawn in nearbySpawns)
        {
            var creature = await worldContentRepository.GetCreatureTemplateAsync(spawn.Entry, cancellationToken).ConfigureAwait(false);
            if (creature is not null)
            {
                await session.SendPacketAsync(
                    Opcode.SMSG_UPDATE_OBJECT,
                    CreatureUpdatePacketWriter.Build(spawn, creature),
                    cancellationToken).ConfigureAwait(false);
            }
        }

        var nearbyGameObjects = await worldContentRepository.GetGameObjectSpawnsNearAsync(
            checked((int)character.Map),
            character.PositionX,
            character.PositionY,
            character.PositionZ,
            radius: 250,
            limit: 128,
            cancellationToken).ConfigureAwait(false);
        foreach (var spawn in nearbyGameObjects)
        {
            var gameObject = await worldContentRepository.GetGameObjectTemplateAsync(spawn.Entry, cancellationToken).ConfigureAwait(false);
            if (gameObject is not null)
            {
                await session.SendPacketAsync(
                    Opcode.SMSG_UPDATE_OBJECT,
                    GameObjectUpdatePacketWriter.Build(spawn, gameObject),
                    cancellationToken).ConfigureAwait(false);
            }
        }

        return character.Guid;
    }

    private async Task<BattleNet.Persistence.Models.CharacterStartupState> BuildStartingStateAsync(
        byte race,
        byte classId,
        byte gender,
        CancellationToken cancellationToken)
    {
        var spells = await worldRepository.GetStartingSpellIdsAsync(race, classId, cancellationToken).ConfigureAwait(false);
        var actionButtons = await worldRepository.GetStartingActionButtonsAsync(race, classId, cancellationToken).ConfigureAwait(false);
        var outfitItems = await dbcRepository.GetStartingOutfitItemIdsAsync(race, classId, gender, cancellationToken).ConfigureAwait(false);
        var startingItems = outfitItems
            .Select(itemId => new BattleNet.Persistence.Models.StartingItemRecord(itemId))
            .ToList();
        if (startingItems.Count == 0)
        {
            startingItems.AddRange(await worldRepository.GetStartingItemRecordsAsync(
                race,
                classId,
                cancellationToken).ConfigureAwait(false));
        }

        var inventory = new List<BattleNet.Persistence.Models.CharacterInventoryRecord>();
        var lastBagSlot = 23u; // InventorySlots.SLOT_INBACKPACK.

        foreach (var startingItem in startingItems)
        {
            var itemEntry = startingItem.ItemId;
            var template = await worldRepository.GetItemTemplateAsync(itemEntry, cancellationToken).ConfigureAwait(false);
            if (template is null)
                continue;

            var inventoryType = template.InventoryType;
            uint bag;
            uint slot;
            uint stackCount;
            if (inventoryType == 0x18) // InventoryTypes.AMMO.
            {
                bag = 19; // InventorySlots.SLOT_BAG1.
                slot = 0;
                stackCount = 100;
            }
            else
            {
                slot = GetStartingInventorySlot(inventoryType);
                bag = 23; // InventorySlots.SLOT_INBACKPACK.
                if (slot >= 23)
                {
                    slot = lastBagSlot++;
                }

                stackCount = template.Class == 0 && inventoryType == 0
                    ? template.Spells.FirstOrDefault()?.SpellId == 430 ? 2u : 4u
                    : Math.Max(1u, startingItem.Amount);
            }

            inventory.Add(new BattleNet.Persistence.Models.CharacterInventoryRecord(
                0,
                0,
                0,
                bag,
                checked((byte)slot),
                itemEntry,
                stackCount,
                template.SpellCharges1,
                template.SpellCharges2,
                template.SpellCharges3,
                template.SpellCharges4,
                template.SpellCharges5,
                Duration: template.Duration));
        }

        return new BattleNet.Persistence.Models.CharacterStartupState(
            spells.Distinct().ToArray(),
            actionButtons,
            inventory);
    }

    private static uint GetStartingInventorySlot(uint inventoryType) => inventoryType switch
    {
        0x00 => 23, // NONE_EQUIP.
        0x01 => 0,  // HEAD.
        0x02 => 1,  // NECK.
        0x03 => 2,  // SHOULDER.
        0x04 => 3,  // BODY.
        0x05 => 4,  // CHEST.
        0x06 => 5,  // WAIST.
        0x07 => 6,  // LEGS.
        0x08 => 7,  // FEET.
        0x09 => 8,  // WRIST.
        0x0A => 9,  // HAND.
        0x0B => 10, // FINGER (Alpha chooses the left slot first).
        0x0C => 12, // TRINKET.
        0x0D => 15, // WEAPON.
        0x0E => 16, // SHIELD.
        0x0F => 17, // RANGED.
        0x10 => 14, // CLOAK.
        0x11 => 15, // TWOHANDEDWEAPON.
        0x12 => 19, // BAG.
        0x13 => 18, // TABARD.
        0x14 => 4,  // ROBE.
        0x15 => 15, // WEAPONMAINHAND.
        0x16 => 16, // WEAPONOFFHAND.
        0x17 => 16, // HOLDABLE.
        0x18 => 23, // AMMO.
        0x19 => 17, // THROWN.
        0x1A => 17, // RANGEDRIGHT.
        _ => 23
    };

    private byte[] BuildNewWorldPayload(BattleNet.Persistence.Models.CharacterSummary character)
    {
        var payload = new byte[17];
        payload[0] = checked((byte)character.Map);
        BinaryPrimitives.WriteInt32LittleEndian(payload.AsSpan(1, 4), BitConverter.SingleToInt32Bits(character.PositionX));
        BinaryPrimitives.WriteInt32LittleEndian(payload.AsSpan(5, 4), BitConverter.SingleToInt32Bits(character.PositionY));
        BinaryPrimitives.WriteInt32LittleEndian(payload.AsSpan(9, 4), BitConverter.SingleToInt32Bits(character.PositionZ));
        BinaryPrimitives.WriteInt32LittleEndian(payload.AsSpan(13, 4), BitConverter.SingleToInt32Bits(character.Orientation));
        return payload;
    }

    private byte[] BuildLoginTimeSpeedPayload()
    {
        var payload = new byte[8];
        BinaryPrimitives.WriteUInt32LittleEndian(payload.AsSpan(0, 4), BuildGameTimeBits());
        BinaryPrimitives.WriteInt32LittleEndian(payload.AsSpan(4, 4), BitConverter.SingleToInt32Bits(options.GameSpeed));
        return payload;
    }

    private static uint BuildGameTimeBits()
    {
        var now = DateTime.Now;
        var dayOfWeek = now.DayOfWeek == DayOfWeek.Sunday ? 6 : (int)now.DayOfWeek - 1;
        var year = Math.Clamp(now.Year - 2000, 0, 255);
        return (uint)(now.Minute
            | (now.Hour << 6)
            | (dayOfWeek << 11)
            | ((now.Day - 1) << 14)
            | ((now.Month - 1) << 20)
            | (year << 24));
    }

    private static bool IsSupportedRace(byte race) => race is >= 1 and <= 8;

    private static bool IsSupportedClass(byte classId) => classId is 1 or 2 or 3 or 4 or 5 or 7 or 8 or 9 or 11;

    private static bool IsValidCharacterName(string name)
    {
        if (name.Length is < 3 or > 12 || name.Count(character => character == '`') > 1)
            return false;

        var normalized = name.Replace("`", string.Empty, StringComparison.Ordinal);
        return normalized.Length > 0
            && normalized.All(character => character is >= 'A' and <= 'Z' or >= 'a' and <= 'z');
    }
}

using System.Buffers.Binary;
using System.Net.Sockets;
using BattleNet.Core.Configuration;
using BattleNet.Core.Networking;
using BattleNet.Persistence;
using BattleNet.Protocol;
using Microsoft.Extensions.Logging;

namespace BattleNet.Server.Services;

internal sealed class RealmServerService(
    ILogger<RealmServerService> logger,
    ServerOptions options,
    IAuthRepository authRepository)
    : TcpListenerService(logger, "RealmServer", options.BindAddress, options.RealmPort)
{
    protected override async Task HandleClientAsync(TcpClient client, CancellationToken stoppingToken)
    {
        await using var session = new SessionConnection(client);
        var realms = await authRepository.GetRealmsAsync(stoppingToken).ConfigureAwait(false);
        var output = new List<byte> { checked((byte)Math.Min(realms.Count, byte.MaxValue)) };

        foreach (var realm in realms.Take(byte.MaxValue))
        {
            output.AddRange(PacketWriter.StringToBytes(realm.Name));
            output.AddRange(PacketWriter.StringToBytes($"{realm.ProxyAddress}:{realm.ProxyPort}"));
            var count = new byte[4];
            BinaryPrimitives.WriteUInt32LittleEndian(count, checked((uint)realm.OnlinePlayers));
            output.AddRange(count);
        }

        await session.SendRawAsync(output.ToArray(), stoppingToken).ConfigureAwait(false);
        logger.LogDebug("Sent {RealmCount} realm entries", realms.Count);
    }
}

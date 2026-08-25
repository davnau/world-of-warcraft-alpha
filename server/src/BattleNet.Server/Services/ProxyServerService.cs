using System.Net.Sockets;
using BattleNet.Core.Configuration;
using BattleNet.Core.Networking;
using BattleNet.Persistence;
using BattleNet.Protocol;
using Microsoft.Extensions.Logging;

namespace BattleNet.Server.Services;

internal sealed class ProxyServerService(
    ILogger<ProxyServerService> logger,
    ServerOptions options,
    IAuthRepository authRepository)
    : TcpListenerService(logger, "ProxyServer", options.BindAddress, options.ProxyPort)
{
    protected override async Task HandleClientAsync(TcpClient client, CancellationToken stoppingToken)
    {
        await using var session = new SessionConnection(client);
        var realms = await authRepository.GetRealmsAsync(stoppingToken).ConfigureAwait(false);
        var realm = realms.FirstOrDefault(item => item.Id == options.LocalRealmId) ?? realms.FirstOrDefault();
        if (realm is null)
            return;

        await session.SendRawAsync(PacketWriter.StringToBytes($"{realm.Address}:{realm.Port}"), stoppingToken).ConfigureAwait(false);
        logger.LogDebug("Redirected client to {Address}:{Port}", realm.Address, realm.Port);
    }
}

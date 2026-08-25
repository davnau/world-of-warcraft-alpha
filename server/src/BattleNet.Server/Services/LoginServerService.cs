using System.Net.Sockets;
using System.Text;
using BattleNet.Core.Configuration;
using BattleNet.Core.Networking;
using BattleNet.Persistence;
using BattleNet.Protocol;
using Microsoft.Extensions.Logging;

namespace BattleNet.Server.Services;

internal sealed class LoginServerService(
    ILogger<LoginServerService> logger,
    ServerOptions options,
    IAuthRepository authRepository)
    : TcpListenerService(logger, "LoginServer", options.BindAddress, options.LoginPort)
{
    protected override async Task HandleClientAsync(TcpClient client, CancellationToken stoppingToken)
    {
        await using var session = new SessionConnection(client);
        var srp = new Srp6ServerSession();

        while (!stoppingToken.IsCancellationRequested)
        {
            var packet = await session.ReadWorldPacketAsync(stoppingToken).ConfigureAwait(false);
            if (packet is null)
                return;

            switch (packet.Value.Opcode)
            {
                case Opcode.CMSG_AUTH_SRP6_BEGIN:
                    await HandleBeginAsync(session, srp, packet.Value.Payload, stoppingToken).ConfigureAwait(false);
                    break;
                case Opcode.CMSG_AUTH_SRP6_PROOF:
                    if (await HandleProofAsync(session, srp, packet.Value.Payload, client, stoppingToken).ConfigureAwait(false))
                        return;
                    break;
                case Opcode.CMSG_AUTH_SRP6_RECODE:
                    await session.SendSrpPacketAsync(
                        new byte[] { (byte)AuthCode.AuthFailed, (byte)Srp6ResponseType.AuthProof }, stoppingToken).ConfigureAwait(false);
                    return;
            }
        }
    }

    private async Task HandleBeginAsync(
        SessionConnection session,
        Srp6ServerSession srp,
        byte[] payload,
        CancellationToken cancellationToken)
    {
        if (payload.Length < 9)
        {
            await SendAuthErrorAsync(session, AuthCode.AuthFailed, cancellationToken).ConfigureAwait(false);
            return;
        }

        var usernameLength = payload[8];
        if (usernameLength == 0 || usernameLength > 64 || payload.Length < 9 + usernameLength)
        {
            await SendAuthErrorAsync(session, AuthCode.AuthFailed, cancellationToken).ConfigureAwait(false);
            return;
        }

        var username = Encoding.Latin1.GetString(payload, 9, usernameLength).TrimEnd('\0').Trim();
        var account = await authRepository.FindAccountAsync(username, cancellationToken).ConfigureAwait(false);
        if (account is null)
        {
            await SendAuthErrorAsync(session, AuthCode.AuthUnknownAccount, cancellationToken).ConfigureAwait(false);
            return;
        }

        srp.Start(account);
        await session.SendSrpPacketAsync(srp.BuildChallenge(), cancellationToken).ConfigureAwait(false);
        logger.LogDebug("SRP6 challenge sent for {Username}", username);
    }

    private async Task<bool> HandleProofAsync(
        SessionConnection session,
        Srp6ServerSession srp,
        byte[] payload,
        TcpClient client,
        CancellationToken cancellationToken)
    {
        if (payload.Length != 52 || srp.Account is null)
        {
            await SendAuthErrorAsync(session, AuthCode.AuthFailed, cancellationToken).ConfigureAwait(false);
            return true;
        }

        var valid = srp.VerifyProof(payload.AsSpan(0, 32), payload.AsSpan(32, 20));
        if (!valid)
        {
            await SendAuthErrorAsync(session, AuthCode.AuthIncorrectPassword, cancellationToken).ConfigureAwait(false);
            return true;
        }

        var ipAddress = client.Client.RemoteEndPoint?.ToString() ?? string.Empty;
        var saved = await authRepository.UpdateSessionKeyAsync(
            srp.Account.Name,
            Convert.ToHexString(srp.SessionKey.Span).ToLowerInvariant(),
            ipAddress,
            cancellationToken).ConfigureAwait(false);
        if (!saved)
        {
            await SendAuthErrorAsync(session, AuthCode.AuthSystemError, cancellationToken).ConfigureAwait(false);
            return true;
        }

        await session.SendSrpPacketAsync(srp.BuildProofResponse(), cancellationToken).ConfigureAwait(false);
        logger.LogInformation("SRP6 login completed for {Username}", srp.Account.Name);
        return true;
    }

    private static Task SendAuthErrorAsync(SessionConnection session, AuthCode code, CancellationToken cancellationToken) =>
        session.SendSrpPacketAsync(new byte[] { (byte)code, (byte)Srp6ResponseType.AuthChallenge }, cancellationToken);
}

using System.Buffers.Binary;
using System.Net.Sockets;
using BattleNet.Core.Configuration;
using BattleNet.Core.Networking;
using BattleNet.Protocol;
using Microsoft.Extensions.Logging;

namespace BattleNet.Server.Services;

internal sealed class UpdateServerService(
    ILogger<UpdateServerService> logger,
    ServerOptions options)
    : TcpListenerService(logger, "UpdateServer", options.BindAddress, options.UpdatePort)
{
    protected override async Task HandleClientAsync(TcpClient client, CancellationToken stoppingToken)
    {
        if (!options.UpdateServerEnabled)
            return;

        await using var session = new SessionConnection(client);
        await SendUpdateFrameAsync(session.Stream, new byte[] { (byte)UpdateServerOpcode.ServerHello, 0, 0, 0, 0 }, stoppingToken).ConfigureAwait(false);
        var hello = await ReadUpdateFrameAsync(session.Stream, stoppingToken).ConfigureAwait(false);
        if (hello is null || hello.Length == 0 || hello[0] != (byte)UpdateServerOpcode.ClientHello)
            return;

        await SendUpdateFrameAsync(session.Stream, new byte[] { (byte)UpdateServerOpcode.ServerHelloAck, 1 }, stoppingToken).ConfigureAwait(false);
        logger.LogWarning("Update server handshake is present, but the patch catalog is not ported yet.");
        await SendUpdateFrameAsync(session.Stream, new byte[] { (byte)UpdateServerOpcode.ServerError }, stoppingToken).ConfigureAwait(false);
    }

    private static async Task SendUpdateFrameAsync(Stream stream, ReadOnlyMemory<byte> payload, CancellationToken cancellationToken)
    {
        var size = payload.Length;
        byte[] header;
        if (size < 0x8000)
        {
            header = new byte[2];
            BinaryPrimitives.WriteUInt16BigEndian(header, checked((ushort)size));
        }
        else
        {
            header = [(byte)(((size >> 16) & 0x7F) | 0x80), (byte)(size >> 8), (byte)size];
        }

        await stream.WriteAsync(header, cancellationToken).ConfigureAwait(false);
        await stream.WriteAsync(payload, cancellationToken).ConfigureAwait(false);
        await stream.FlushAsync(cancellationToken).ConfigureAwait(false);
    }

    private static async Task<byte[]?> ReadUpdateFrameAsync(Stream stream, CancellationToken cancellationToken)
    {
        var first = new byte[1];
        if (!await ReadExactlyAsync(stream, first, cancellationToken).ConfigureAwait(false))
            return null;

        var header = new byte[(first[0] & 0x80) != 0 ? 2 : 1];
        if (!await ReadExactlyAsync(stream, header, cancellationToken).ConfigureAwait(false))
            return null;

        var size = (first[0] & 0x80) != 0
            ? ((first[0] & 0x7F) << 16) | (header[0] << 8) | header[1]
            : (first[0] << 8) | header[0];
        if (size > 0x400000)
            throw new InvalidDataException("Update frame exceeds the safety limit.");

        var payload = new byte[size];
        return await ReadExactlyAsync(stream, payload, cancellationToken).ConfigureAwait(false) ? payload : null;
    }

    private static async Task<bool> ReadExactlyAsync(Stream stream, Memory<byte> buffer, CancellationToken cancellationToken)
    {
        var offset = 0;
        while (offset < buffer.Length)
        {
            var read = await stream.ReadAsync(buffer[offset..], cancellationToken).ConfigureAwait(false);
            if (read == 0)
                return false;
            offset += read;
        }
        return true;
    }
}

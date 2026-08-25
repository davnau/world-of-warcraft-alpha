using System.Security.Cryptography;
using BattleNet.Persistence.Models;
using BattleNet.Protocol;

namespace BattleNet.Server;

internal sealed class Srp6ServerSession
{
    private AccountRecord? _account;
    private byte[] _serverPrivateKey = [];
    private byte[] _serverPublicKey = [];
    private byte[] _clientPublicKey = [];
    private byte[] _sessionKey = [];
    private byte[] _clientProof = [];

    public AccountRecord? Account => _account;
    public ReadOnlyMemory<byte> SessionKey => _sessionKey;

    public void Start(AccountRecord account)
    {
        _account = account;
        _serverPrivateKey = RandomNumberGenerator.GetBytes(Srp6.KeySize);
        _serverPublicKey = Srp6.CalculateServerPublicKey(Convert.FromHexString(account.VerifierHex), _serverPrivateKey);
    }

    public byte[] BuildChallenge()
    {
        EnsureStarted();
        var account = _account!;
        var data = new List<byte>(2 + 2 + Srp6.KeySize * 2 + account.SaltHex.Length / 2)
        {
            (byte)AuthCode.AuthOk,
            (byte)Srp6ResponseType.AuthChallenge,
            (byte)Srp6.GeneratorBytes.Length
        };
        data.AddRange(Srp6.GeneratorBytes);
        data.Add((byte)Srp6.ModulusBytes.Length);
        data.AddRange(Srp6.ModulusBytes);
        data.AddRange(Convert.FromHexString(account.SaltHex));
        data.AddRange(_serverPublicKey);
        return [.. data];
    }

    public bool VerifyProof(ReadOnlySpan<byte> clientPublicKey, ReadOnlySpan<byte> clientProof)
    {
        EnsureStarted();
        if (clientPublicKey.Length != Srp6.KeySize || clientProof.Length != 20)
            return false;

        var clientValue = Srp6.FromLittleEndian(clientPublicKey);
        if (clientValue % Srp6.Modulus == 0)
            return false;

        _clientPublicKey = clientPublicKey.ToArray();
        var scrambling = Srp6.CalculateU(_clientPublicKey, _serverPublicKey);
        if (Srp6.FromLittleEndian(scrambling).IsZero)
            return false;

        var secret = Srp6.CalculateServerSKey(
            _clientPublicKey,
            Convert.FromHexString(_account!.VerifierHex),
            scrambling,
            _serverPrivateKey);
        _sessionKey = Srp6.CalculateInterleaved(secret);
        _clientProof = Srp6.CalculateClientProof(
            Srp6.XorNg,
            _account.Name,
            _sessionKey,
            _clientPublicKey,
            _serverPublicKey,
            Convert.FromHexString(_account.SaltHex));

        return CryptographicOperations.FixedTimeEquals(_clientProof, clientProof);
    }

    public byte[] BuildProofResponse()
    {
        EnsureStarted();
        var serverProof = Srp6.CalculateServerProof(_clientPublicKey, _clientProof, _sessionKey);
        var data = new byte[2 + serverProof.Length + 4];
        data[0] = (byte)AuthCode.AuthOk;
        data[1] = (byte)Srp6ResponseType.AuthProof;
        serverProof.CopyTo(data, 2);
        return data;
    }

    private void EnsureStarted()
    {
        if (_account is null || _serverPublicKey.Length != Srp6.KeySize)
            throw new InvalidOperationException("SRP6 session has not been initialized.");
    }
}

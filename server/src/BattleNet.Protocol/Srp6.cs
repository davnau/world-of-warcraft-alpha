using System.Numerics;
using System.Security.Cryptography;
using System.Text;

namespace BattleNet.Protocol;

public static class Srp6
{
    public const int KeySize = 32;
    public const byte Generator = 7;
    public const int Multiplier = 3;

    public static readonly byte[] GeneratorBytes = [Generator];
    public static readonly BigInteger Modulus = new(
        Convert.FromHexString("894B645E89E1535BBDAD5B8B290650530801B18EBFBF5E8FAB3C82872A3E9BB7"),
        isUnsigned: true,
        isBigEndian: true);
    public static readonly byte[] ModulusBytes = ToRawLittleEndian(Modulus, KeySize);
    public static readonly byte[] XorNg =
    [
        221, 123, 176, 58, 56, 172, 115, 17, 3, 152, 124,
        90, 80, 111, 202, 150, 108, 123, 194, 167
    ];

    public static byte[] GenerateSalt() => RandomNumberGenerator.GetBytes(KeySize);

    public static byte[] CalculateX(string username, string password, ReadOnlySpan<byte> salt)
    {
        var first = Sha1(Encoding.UTF8.GetBytes($"{username.ToUpperInvariant()}:{password.ToUpperInvariant()}"));
        return Sha1(Concat(salt.ToArray(), first));
    }

    public static byte[] CalculatePasswordVerifier(string username, string password, ReadOnlySpan<byte> salt)
    {
        var x = FromLittleEndian(CalculateX(username, password, salt));
        return ToLittleEndian(BigInteger.ModPow(Generator, x, Modulus), KeySize);
    }

    public static byte[] CalculateServerPublicKey(ReadOnlySpan<byte> verifierBytes, ReadOnlySpan<byte> privateKeyBytes)
    {
        var verifier = FromLittleEndian(verifierBytes);
        var privateKey = FromLittleEndian(privateKeyBytes);
        var publicKey = (Multiplier * verifier + BigInteger.ModPow(Generator, privateKey, Modulus)) % Modulus;
        if (publicKey.IsZero)
            throw new InvalidDataException("SRP6 server public key is zero.");
        return ToLittleEndian(publicKey, KeySize);
    }

    public static byte[] CalculateServerSKey(
        ReadOnlySpan<byte> clientPublicKeyBytes,
        ReadOnlySpan<byte> verifierBytes,
        ReadOnlySpan<byte> scramblingBytes,
        ReadOnlySpan<byte> privateKeyBytes)
    {
        var clientPublicKey = FromLittleEndian(clientPublicKeyBytes);
        var verifier = FromLittleEndian(verifierBytes);
        var scrambling = FromLittleEndian(scramblingBytes);
        var privateKey = FromLittleEndian(privateKeyBytes);
        var baseValue = Normalize(clientPublicKey * BigInteger.ModPow(verifier, scrambling, Modulus));
        return ToLittleEndian(BigInteger.ModPow(baseValue, privateKey, Modulus), KeySize);
    }

    public static byte[] CalculateU(ReadOnlySpan<byte> clientPublicKey, ReadOnlySpan<byte> serverPublicKey) =>
        Sha1(Concat(clientPublicKey.ToArray(), serverPublicKey.ToArray()));

    public static byte[] CalculateInterleaved(ReadOnlySpan<byte> secret)
    {
        var value = secret.ToArray();
        while (value.Length > 1 && value[0] == 0)
            value = value[2..];

        var even = value.Where((_, index) => index % 2 == 0).ToArray();
        var odd = value.Where((_, index) => index % 2 != 0).ToArray();
        var g = Sha1(even);
        var h = Sha1(odd);
        var result = new byte[g.Length + h.Length];
        for (var i = 0; i < g.Length; i++)
        {
            result[i * 2] = g[i];
            result[i * 2 + 1] = h[i];
        }
        return result;
    }

    public static byte[] CalculateServerProof(
        ReadOnlySpan<byte> clientPublicKey,
        ReadOnlySpan<byte> clientProof,
        ReadOnlySpan<byte> sessionKey) =>
        Sha1(Concat(clientPublicKey.ToArray(), clientProof.ToArray(), sessionKey.ToArray()));

    public static byte[] CalculateClientProof(
        ReadOnlySpan<byte> x,
        string username,
        ReadOnlySpan<byte> sessionKey,
        ReadOnlySpan<byte> clientPublicKey,
        ReadOnlySpan<byte> serverPublicKey,
        ReadOnlySpan<byte> salt)
    {
        var usernameHash = Sha1(Encoding.UTF8.GetBytes(username.ToUpperInvariant()));
        return Sha1(Concat(
            x.ToArray(), usernameHash, salt.ToArray(), clientPublicKey.ToArray(),
            serverPublicKey.ToArray(), sessionKey.ToArray()));
    }

    public static byte[] CalculateWorldServerProof(
        string username,
        ReadOnlySpan<byte> clientSeed,
        ReadOnlySpan<byte> serverSeed,
        ReadOnlySpan<byte> sessionKey) =>
        Sha1(Concat(
            Encoding.UTF8.GetBytes(username.ToUpperInvariant()),
            [0, 0, 0, 0], clientSeed.ToArray(), [0, 0, 0, 0], serverSeed.ToArray(), sessionKey.ToArray()));

    public static BigInteger FromLittleEndian(ReadOnlySpan<byte> bytes) =>
        new(bytes.ToArray(), isUnsigned: true, isBigEndian: false);

    public static byte[] ToLittleEndian(BigInteger value, int size)
    {
        return ToRawLittleEndian(Normalize(value), size);
    }

    private static byte[] ToRawLittleEndian(BigInteger value, int size)
    {
        var raw = value.ToByteArray(isUnsigned: true, isBigEndian: false);
        var result = new byte[size];
        Array.Copy(raw, result, Math.Min(raw.Length, result.Length));
        return result;
    }

    private static byte[] Sha1(byte[] data) => SHA1.HashData(data);

    private static byte[] Concat(params byte[][] parts)
    {
        var length = parts.Sum(static part => part.Length);
        var result = new byte[length];
        var offset = 0;
        foreach (var part in parts)
        {
            part.CopyTo(result, offset);
            offset += part.Length;
        }
        return result;
    }

    private static BigInteger Normalize(BigInteger value)
    {
        value %= Modulus;
        return value.Sign < 0 ? value + Modulus : value;
    }
}

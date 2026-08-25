using System.Security.Cryptography;
using System.Text;
using BattleNet.Persistence.Models;
using BattleNet.Protocol;

namespace BattleNet.Persistence;

public enum AccountCreateResult
{
    Success,
    InvalidUsername,
    InvalidPassword,
    AlreadyExists
}

public static class AccountRegistration
{
    public const int MinimumUsernameLength = 3;
    public const int MaximumUsernameLength = 32;
    public const int MinimumPasswordLength = 6;
    public const int MaximumPasswordLength = 64;

    public static string NormalizeUsername(string? username) =>
        (username ?? string.Empty).Trim().ToUpperInvariant();

    public static bool IsValidUsername(string username)
    {
        if (username.Length is < MinimumUsernameLength or > MaximumUsernameLength)
            return false;

        return username.All(static character =>
            character is >= 'A' and <= 'Z'
            or >= '0' and <= '9');
    }

    public static bool IsValidPassword(string? password)
    {
        if (password is null
            || password.Length is < MinimumPasswordLength or > MaximumPasswordLength)
            return false;

        return password.All(static character => character is >= ' ' and <= '~');
    }

    public static AccountRecord CreateRecord(int id, string username, string password)
    {
        var normalizedUsername = NormalizeUsername(username);
        var salt = Srp6.GenerateSalt();
        var verifier = Srp6.CalculatePasswordVerifier(normalizedUsername, password, salt);
        var passwordHash = Convert.ToHexString(
            SHA256.HashData(Encoding.UTF8.GetBytes(password))).ToLowerInvariant();
        return new AccountRecord(
            id,
            normalizedUsername,
            passwordHash,
            Convert.ToHexString(salt).ToLowerInvariant(),
            Convert.ToHexString(verifier).ToLowerInvariant(),
            string.Empty,
            0,
            string.Empty);
    }
}

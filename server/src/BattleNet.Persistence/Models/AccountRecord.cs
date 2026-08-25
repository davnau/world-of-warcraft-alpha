namespace BattleNet.Persistence.Models;

public sealed record AccountRecord(
    int Id,
    string Name,
    string PasswordHash,
    string SaltHex,
    string VerifierHex,
    string SessionKeyHex,
    int SecurityLevel,
    string LastIp)
{
    public AccountRecord WithSessionKey(string sessionKeyHex, string ipAddress) =>
        this with { SessionKeyHex = sessionKeyHex, LastIp = ipAddress };
}

namespace BattleNet.Persistence.Models;

public sealed record RealmInfo(
    int Id,
    string Name,
    string Address,
    int Port,
    string ProxyAddress,
    int ProxyPort,
    int OnlinePlayers);

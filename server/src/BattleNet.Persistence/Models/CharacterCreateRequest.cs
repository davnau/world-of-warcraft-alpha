namespace BattleNet.Persistence.Models;

public sealed record CharacterCreateRequest(
    int AccountId,
    int RealmId,
    string Name,
    byte Race,
    byte Class,
    byte Gender,
    byte Skin,
    byte Face,
    byte HairStyle,
    byte HairColor,
    byte FacialHair,
    byte Level,
    StartingLocation StartingLocation);

public sealed record StartingLocation(
    uint Map,
    uint Zone,
    float PositionX,
    float PositionY,
    float PositionZ,
    float Orientation)
{
    public static StartingLocation Default { get; } = new(0, 0, 0, 0, 0, 0);
}

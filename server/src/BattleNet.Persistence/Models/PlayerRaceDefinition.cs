namespace BattleNet.Persistence.Models;

/// <summary>
/// The Alpha ChrRaces row needed to initialize a player object.
/// </summary>
public sealed record PlayerRaceDefinition(
    byte Race,
    uint FactionId,
    uint MaleDisplayId,
    uint FemaleDisplayId,
    float MountScale,
    uint BaseLanguage,
    uint CreatureType);

namespace BattleNet.Persistence.Models;

/// <summary>
/// The local equivalent of Alpha Core's character_quest_state row.
/// Requirement progress is added separately as creature/item systems mature.
/// </summary>
public sealed record CharacterQuestRecord(
    ulong CharacterGuid,
    uint QuestId,
    int State = 2,
    bool Rewarded = false,
    IReadOnlyList<uint>? ObjectiveCounts = null)
{
    public IReadOnlyList<uint> Progress => ObjectiveCounts ?? Array.Empty<uint>();
}

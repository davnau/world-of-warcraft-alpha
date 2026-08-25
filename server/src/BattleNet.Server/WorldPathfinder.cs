namespace BattleNet.Server;

/// <summary>
/// Small local A* pathfinder for the standalone world slice. The exported Alpha
/// tables do not contain terrain collision meshes, so this avoids dynamic unit
/// bodies and still falls back to a direct spline when no obstacle is present.
/// </summary>
internal static class WorldPathfinder
{
    private const float CellSize = 2.0f;
    private const int SearchRadius = 12;

    public static (float X, float Y, float Z) GetNextStep(
        CreatureRuntimeState mover,
        float destinationX,
        float destinationY,
        float destinationZ,
        IEnumerable<CreatureRuntimeState> obstacles)
    {
        if (IsClear(mover.PositionX, mover.PositionY, destinationX, destinationY, mover, obstacles))
            return (destinationX, destinationY, destinationZ);

        var start = (X: 0, Y: 0);
        var goal = (
            X: (int)MathF.Round((destinationX - mover.PositionX) / CellSize),
            Y: (int)MathF.Round((destinationY - mover.PositionY) / CellSize));
        goal.X = Math.Clamp(goal.X, -SearchRadius, SearchRadius);
        goal.Y = Math.Clamp(goal.Y, -SearchRadius, SearchRadius);

        var open = new PriorityQueue<(int X, int Y), int>();
        var parents = new Dictionary<(int X, int Y), (int X, int Y)>();
        var costs = new Dictionary<(int X, int Y), int> { [start] = 0 };
        open.Enqueue(start, 0);
        var found = false;
        while (open.TryDequeue(out var current, out _))
        {
            if (current == goal)
            {
                found = true;
                break;
            }
            foreach (var neighbor in Neighbors(current))
            {
                if (Math.Abs(neighbor.X) > SearchRadius || Math.Abs(neighbor.Y) > SearchRadius)
                    continue;
                if (neighbor != goal && IsBlocked(mover.PositionX + neighbor.X * CellSize, mover.PositionY + neighbor.Y * CellSize, mover, obstacles))
                    continue;
                var nextCost = costs[current] + 1;
                if (costs.TryGetValue(neighbor, out var oldCost) && oldCost <= nextCost)
                    continue;
                costs[neighbor] = nextCost;
                parents[neighbor] = current;
                var priority = nextCost + Math.Abs(goal.X - neighbor.X) + Math.Abs(goal.Y - neighbor.Y);
                open.Enqueue(neighbor, priority);
            }
        }

        if (!found)
            return (destinationX, destinationY, destinationZ);

        var step = goal;
        while (parents.TryGetValue(step, out var parent) && parent != start)
            step = parent;
        return (
            mover.PositionX + step.X * CellSize,
            mover.PositionY + step.Y * CellSize,
            destinationZ);
    }

    private static bool IsClear(
        float startX,
        float startY,
        float destinationX,
        float destinationY,
        CreatureRuntimeState mover,
        IEnumerable<CreatureRuntimeState> obstacles)
    {
        var distance = MathF.Sqrt((destinationX - startX) * (destinationX - startX) + (destinationY - startY) * (destinationY - startY));
        var steps = Math.Max(1, (int)MathF.Ceiling(distance / 1.0f));
        for (var index = 1; index < steps; index++)
        {
            var t = index / (float)steps;
            if (IsBlocked(startX + (destinationX - startX) * t, startY + (destinationY - startY) * t, mover, obstacles))
                return false;
        }
        return true;
    }

    private static bool IsBlocked(float x, float y, CreatureRuntimeState mover, IEnumerable<CreatureRuntimeState> obstacles) =>
        obstacles.Any(obstacle => !ReferenceEquals(obstacle, mover)
            && !obstacle.IsDead
            && (obstacle.PositionX - x) * (obstacle.PositionX - x) + (obstacle.PositionY - y) * (obstacle.PositionY - y) < 2.25f);

    private static IEnumerable<(int X, int Y)> Neighbors((int X, int Y) point)
    {
        yield return (point.X + 1, point.Y);
        yield return (point.X - 1, point.Y);
        yield return (point.X, point.Y + 1);
        yield return (point.X, point.Y - 1);
        yield return (point.X + 1, point.Y + 1);
        yield return (point.X - 1, point.Y - 1);
        yield return (point.X + 1, point.Y - 1);
        yield return (point.X - 1, point.Y + 1);
    }
}

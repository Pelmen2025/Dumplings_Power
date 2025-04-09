public class SegmentBase
{
    public double Start { get; protected set; }
    public double End { get; protected set; }

    public SegmentBase(double start, double end)
    {
        Start = start;
        End = end;
    }

    public override string ToString()
    {
        return $"Начало: {Start}, Конец: {End}";
    }
}

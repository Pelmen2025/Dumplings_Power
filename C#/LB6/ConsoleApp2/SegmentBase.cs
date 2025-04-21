public class SegmentBase
{
    private double _start;
    private double _end;

    public SegmentBase(double start, double end)
    {
        Start = start;
        End = end;
    }

    public double Start
    {
        get
        {
            return _start;
        }
        set
        {
            _start = value;
        }
    }

    public double End
    {
        get
        {
            return _end;
        }
        set
        {
            _end = value;
        }
    }

    public override string ToString()
    {
        return $"Начало: {Start}, Конец: {End}";
    }
}

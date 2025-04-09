public class LineSegment : SegmentBase
{
    public LineSegment(double x1, double x2) : base(x1, x2)
    {
        Normalize();
    }

    private void Normalize()
    {
        if (Start > End)
        {
            double temp = Start;
            Start = End;
            End = temp;
        }
    }

    public bool Intersects(LineSegment other)
    {
        return !(End < other.Start || Start > other.End);
    }

    public override string ToString()
    {
        return $"Отрезок от {Start} до {End}";
    }
}

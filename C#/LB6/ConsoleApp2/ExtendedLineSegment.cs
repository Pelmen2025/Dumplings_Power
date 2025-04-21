using System;

public class ExtendedLineSegment : LineSegment
{
    public ExtendedLineSegment(double x1, double x2) : base(x1, x2) { }

    public static double operator !(ExtendedLineSegment segment)
    {
        return segment.End - segment.Start;
    }

    public static ExtendedLineSegment operator ++(ExtendedLineSegment segment)
    {
        return new ExtendedLineSegment(segment.Start - 1, segment.End + 1);
    }

    public static implicit operator int(ExtendedLineSegment segment)
    {
        return (int)segment.Start;
    }

    public static explicit operator double(ExtendedLineSegment segment)
    {
        return segment.End;
    }

    public static ExtendedLineSegment operator -(ExtendedLineSegment segment, int value)
    {
        return new ExtendedLineSegment(segment.Start - value, segment.End);
    }

    public static ExtendedLineSegment operator -(int value, ExtendedLineSegment segment)
    {
        return new ExtendedLineSegment(segment.Start, segment.End - value);
    }

    public static bool operator <(ExtendedLineSegment a, ExtendedLineSegment b)
    {
        return a.Intersects(b);
    }

    public static bool operator >(ExtendedLineSegment a, ExtendedLineSegment b)
    {
        return a.Intersects(b);
    }
}

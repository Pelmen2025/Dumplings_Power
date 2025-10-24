public class LineSegment
{
    private double _x1; 
    private double _x2; 

    public double Start
    {
        get => _x1;
        set => _x1 = value;
    }

    public double End
    {
        get => _x2;
        set => _x2 = value;
    }

    public LineSegment()
    {
        _x1 = 0;
        _x2 = 0;
    }

    public LineSegment(double x1, double x2)
    {
        _x1 = x1;
        _x2 = x2;
    }

    public double GetLength()
    {
        return Math.Abs(_x2 - _x1);
    }

    public override string ToString()
    {
        return $"[{_x1}; {_x2}] (длина: {GetLength():F2})";
    }


    public static LineSegment operator +(LineSegment seg)
    {
        return new LineSegment(seg._x1, seg._x2);
    }

    public static LineSegment operator -(LineSegment seg)
    {
        return new LineSegment(-seg._x1, -seg._x2);
    }


    public static implicit operator double(LineSegment seg)
    {
        return (seg._x1 + seg._x2) / 2.0;
    }

    public static LineSegment operator +(LineSegment seg, int d)
    {
        return new LineSegment(seg._x1 + d, seg._x2 + d);
    }

    public static LineSegment operator +(int d, LineSegment seg)
    {
        return seg + d;
    }

    public static LineSegment operator -(LineSegment seg, int d)
    {
        return seg + (-d);
    }

    public static LineSegment operator -(int d, LineSegment seg)
    {
        throw new NotSupportedException("Операция int - LineSegment не поддерживается.");
    }
}
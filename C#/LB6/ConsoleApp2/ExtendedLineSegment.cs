using System;

public class ExtendedLineSegment : LineSegment
{
    public ExtendedLineSegment(double x1, double x2) : base(x1, x2) { }

    // Унарная операция ! — длина отрезка
    public static double operator !(ExtendedLineSegment segment)
    {
        return segment.End - segment.Start;
    }

    // Унарная операция ++ — расширить отрезок на 1 влево и вправо
    public static ExtendedLineSegment operator ++(ExtendedLineSegment segment)
    {
        return new ExtendedLineSegment(segment.Start - 1, segment.End + 1);
    }

    // Преобразование к int (неявное) — целая часть начала
    public static implicit operator int(ExtendedLineSegment segment)
    {
        return (int)segment.Start;
    }

    // Преобразование к double (явное) — координата конца
    public static explicit operator double(ExtendedLineSegment segment)
    {
        return segment.End;
    }

    // Отрезок - число (уменьшить Start)
    public static ExtendedLineSegment operator -(ExtendedLineSegment segment, int value)
    {
        return new ExtendedLineSegment(segment.Start - value, segment.End);
    }

    // Число - отрезок (уменьшить End)
    public static ExtendedLineSegment operator -(int value, ExtendedLineSegment segment)
    {
        return new ExtendedLineSegment(segment.Start, segment.End - value);
    }

    // Сравнение < — проверка пересечения
    public static bool operator <(ExtendedLineSegment a, ExtendedLineSegment b)
    {
        return a.Intersects(b);
    }

    public static bool operator >(ExtendedLineSegment a, ExtendedLineSegment b)
    {
        return a.Intersects(b);
    }
}

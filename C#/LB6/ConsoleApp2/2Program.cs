using System;

public class Program
{
    public static void Main()
    {
        Console.WriteLine("Программа проверки пересечения двух отрезков на координатной прямой.");

        Console.WriteLine("\nВведите координаты первого отрезка:");
        double x1 = InputValidator.ReadDouble("Начало: ");
        double x2 = InputValidator.ReadDouble("Конец: ");
        LineSegment segment1 = new LineSegment(x1, x2);
        Console.WriteLine("Первый " + segment1);

        Console.WriteLine("\nВведите координаты второго отрезка:");
        double x3 = InputValidator.ReadDouble("Начало: ");
        double x4 = InputValidator.ReadDouble("Конец: ");
        LineSegment segment2 = new LineSegment(x3, x4);
        Console.WriteLine("Второй " + segment2);

        bool intersects = segment1.Intersects(segment2);

        Console.WriteLine($"\nРезультат: Отрезки {(intersects ? "пересекаются" : "не пересекаются")}.");
    }
}

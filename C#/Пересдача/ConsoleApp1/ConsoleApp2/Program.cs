using G;

class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("=== Тестирование класса LineSegment ===\n");

        // Тест конструкторов
        Console.WriteLine("1. Конструкторы:");
        LineSegment s1 = new LineSegment();
        LineSegment s2 = new LineSegment(2.5, 8);
        Console.WriteLine($"   По умолчанию: {s1}");
        Console.WriteLine($"   С параметрами: {s2}\n");

        // Тест свойств и метода CalculateLength
        Console.WriteLine("2. Свойства и длина:");
        s1.Start = -4.0;
        s1.End = 5.0;
        Console.WriteLine($"   {s1}");
        Console.WriteLine($"   Длина: {s1.CalculateLength()}\n");

        // Тест операций
        Console.WriteLine("3. Операции:");
        LineSegment s4 = new LineSegment(5.0, 11.5);
        Console.WriteLine($"   Исходный: {s4}");
        Console.WriteLine($"   s4 + 3: {s4 + 3}");
        Console.WriteLine($"   5 + s4: {5 + s4}");
        double end = s4;
        Console.WriteLine($"   Преобразование в double: {end}");

        Console.WriteLine("\n=== Тестирование завершено ===");
        Console.ReadKey();
    }
}
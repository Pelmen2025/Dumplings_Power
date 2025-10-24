class Program
{
    static void Main()
    {
        Console.OutputEncoding = System.Text.Encoding.UTF8;
        Console.WriteLine("=== Тест класса LineSegment ===\n");

        // 1. Конструктор по умолчанию
        var seg = new LineSegment();
        Console.WriteLine($"По умолчанию: {seg}");

        // 2. Конструктор с параметрами + длина
        seg = new LineSegment(2, 7);
        Console.WriteLine($"Отрезок: {seg}");
        Console.WriteLine($"Длина: {seg.GetLength():F2}\n");

        // 3. Унарные операции
        Console.WriteLine($"+seg = {+seg}");
        Console.WriteLine($"-seg = {-seg}\n");

        // 4. Приведение к double (середина)
        double mid = seg;
        Console.WriteLine($"Середина отрезка: {mid:F2}\n");

        // 5. Бинарные операции с int
        Console.WriteLine($"seg + 3 = {seg + 3}");
        Console.WriteLine($"5 + seg = {5 + seg}");
        Console.WriteLine($"seg - 2 = {seg - 2}\n");

        // 6. Ввод с клавиатуры
        Console.Write("Введите начало (x1): ");
        double x1 = double.Parse(Console.ReadLine() ?? "0");
        Console.Write("Введите конец (x2): ");
        double x2 = double.Parse(Console.ReadLine() ?? "0");

        var userSeg = new LineSegment(x1, x2);
        Console.WriteLine($"\nВаш отрезок: {userSeg}");
        Console.WriteLine($"Середина: {(double)userSeg:F2}");
        Console.WriteLine($"Сдвиг +1: {userSeg + 1}");

        // 7. Изменение через свойства
        userSeg.Start += 10;
        userSeg.End -= 5;
        Console.WriteLine($"После изменения: {userSeg}");

        Console.WriteLine("\nГотово. Нажмите любую клавишу...");
        Console.ReadKey();
    }
}
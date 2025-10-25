using T;

class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("=== Тестирование класса Time ===\n");

        // Тест конструкторов
        Time t1 = new Time();
        Time t2 = new Time(14, 30);
        Console.WriteLine($"Конструкторы: {t1}, {t2}");

        // Тест свойств и ToString
        t1.Hours = 10;
        t1.Minutes = 45;
        Console.WriteLine($"Свойства: {t1.Hours}:{t1.Minutes}");

        // Тест унарной операции --
        Time t4 = new Time(10, 0);
        t4 = --t4;
        Console.WriteLine($"Операция --: {t4}");

        // Тест неявного приведения к int
        Time t5 = new Time(2, 30);
        int totalMinutes = t5;
        Console.WriteLine($"Приведение к int: {totalMinutes} минут");

        // Тест операций < и >
        Time t6 = new Time(10, 30);
        Time t7 = new Time(12, 15);
        Console.WriteLine($"{t6} < {t7} = {t6 < t7}");
        Console.WriteLine($"{t7} > {t6} = {t7 > t6}");

        Console.WriteLine("\n=== Тестирование завершено ===");
    }
}
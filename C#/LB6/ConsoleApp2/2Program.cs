using System;

class Program
{
    static void Main()
    {
        Console.WriteLine("Программа проверки пересечения двух отрезков на координатной прямой.");

        Console.WriteLine("\nВведите координаты первого отрезка:");
        double x1 = InputValidator.ReadDouble("Начало: ");
        double x2 = InputValidator.ReadDouble("Конец: ");
        ExtendedLineSegment segment1 = new ExtendedLineSegment(x1, x2);
        Console.WriteLine("Первый " + segment1);

        Console.WriteLine("\nВведите координаты второго отрезка:");
        double x3 = InputValidator.ReadDouble("Начало: ");
        double x4 = InputValidator.ReadDouble("Конец: ");
        ExtendedLineSegment segment2 = new ExtendedLineSegment(x3, x4);
        Console.WriteLine("Второй " + segment2);

        // Проверка на пересечение
        bool intersects = segment1.Intersects(segment2);
        Console.WriteLine($"\nРезультат: Отрезки {(intersects ? "пересекаются" : "не пересекаются")}.");

<<<<<<< HEAD
        Console.WriteLine("\nТестирование унарной операции ! (длина):");
        Console.WriteLine($"Длина первого отрезка: {!segment1}");

=======
        // Тестирование унарного оператора !
        Console.WriteLine("\nТестирование унарной операции ! (длина):");
        Console.WriteLine($"Длина первого отрезка: {!segment1}");

        // Тестирование унарного оператора ++
>>>>>>> origin/main
        Console.WriteLine("\nТестирование унарной операции ++ (расширение):");
        ExtendedLineSegment extended = ++segment1;
        Console.WriteLine($"Расширенный отрезок: {extended}");

<<<<<<< HEAD
=======
        // Преобразование к int (неявное)
>>>>>>> origin/main
        Console.WriteLine("\nТестирование приведения к int:");
        int intValue = segment1;
        Console.WriteLine($"Целая часть начала отрезка: {intValue}");

<<<<<<< HEAD
=======
        // Преобразование к double (явное)
>>>>>>> origin/main
        Console.WriteLine("\nТестирование приведения к double:");
        double doubleValue = (double)segment1;
        Console.WriteLine($"Конец отрезка как double: {doubleValue}");

<<<<<<< HEAD
=======
        // Бинарный оператор: отрезок - число
>>>>>>> origin/main
        Console.WriteLine("\nТестирование бинарной операции - (отрезок - число):");
        ExtendedLineSegment shiftedLeft = segment1 - 2;
        Console.WriteLine($"Отрезок с уменьшенным началом: {shiftedLeft}");

<<<<<<< HEAD
=======
        // Бинарный оператор: число - отрезок
>>>>>>> origin/main
        Console.WriteLine("\nТестирование бинарной операции - (число - отрезок):");
        ExtendedLineSegment shiftedRight = 2 - segment1;
        Console.WriteLine($"Отрезок с уменьшенным концом: {shiftedRight}");

<<<<<<< HEAD
        Console.WriteLine("\nТестирование бинарной операции < (пересекаются ли):");
        Console.WriteLine(segment1 < segment2
            ? "Отрезки пересекаются."
            : "Отрезки не пересекаются.");
=======
        // Бинарный оператор < (пересечение)
        Console.WriteLine("\nТестирование бинарной операции < (пересекаются ли):");
        Console.WriteLine(segment1 < segment2
            ? "Отрезки пересекаются (оператор <)."
            : "Отрезки не пересекаются (оператор <).");
>>>>>>> origin/main
    }
}

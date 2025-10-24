class Program
{
    static string? ReadOptional(string prompt)
    {
        Console.Write(prompt);
        string? input = Console.ReadLine();
        return string.IsNullOrWhiteSpace(input) ? null : input.Trim();
    }

    static void Main()
    {
        Console.OutputEncoding = System.Text.Encoding.UTF8;
        Console.WriteLine(" Ввод имён через клавиатуру \n");

        var names = new List<FullName>();

        Console.WriteLine("Введите 3 имени. Пропустить часть — нажать Enter.\n");

        for (int i = 1; i <= 3; i++)
        {
            Console.WriteLine($" Имя {i} ");

            string? fam = ReadOptional("Фамилия (Enter — пропустить): ");
            string? name = ReadOptional("Личное имя (Enter — пропустить): ");
            string? otchestvo = ReadOptional("Отчество (Enter — пропустить): ");

            var fullName = new FullName(fam, name, otchestvo);
            names.Add(fullName);

            Console.WriteLine($"Создано: {fullName}\n");
        }

        Console.WriteLine("=== Все введённые имена ===");
        for (int i = 0; i < names.Count; i++)
        {
            Console.WriteLine($"{i + 1}. {names[i]}");
        }

        Console.WriteLine("\nНажмите любую клавишу для выхода...");
        Console.ReadKey();
    }
}
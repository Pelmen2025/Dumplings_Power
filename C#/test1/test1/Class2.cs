using System.Xml.Serialization;

public static class TasksClass
{
    // Задание 1
    public static void Task1()
    {
        string fileName = "task1.txt";
        var numbers = File.ReadAllLines(fileName).Select(int.Parse).ToList();
        int min = numbers.Min();
        int max = numbers.Max();
        double avg = (min + max) / 2.0;
        Console.WriteLine($"Среднее арифметическое максимального и минимального элементов: {avg}");
    }

    // Задание 2
    public static void Task2()
    {
        string fileName = "task2.txt";
        var lines = File.ReadAllLines(fileName);
        long product = 1;

        foreach (var line in lines)
        {
            var numbers = line.Split(' ').Select(int.Parse).Where(x => x % 2 == 0).ToList();
            foreach (var num in numbers)
            {
                product *= num;
            }
        }

        Console.WriteLine($"Произведение чётных элементов: {product}");
    }

    // Задание 3
    public static void Task3()
    {
        string inputFileName = "task3.txt";
        string outputFileName = "task3_output.txt";
        BaseClass.CopyLinesWithoutLatinLetters(inputFileName, outputFileName);
        Console.WriteLine($"Строки без латинских букв записаны в файл {outputFileName}");
    }

    // Задание 4 (Бинарные файлы: вывести сгенерированные числа)
    public static void Task4()
    {
        string fileName = "task4.bin";
        Random rand = new Random();
        List<int> generatedNumbers = new List<int>();

        // Заполняем бинарный файл случайными числами
        using (BinaryWriter writer = new BinaryWriter(File.Open(fileName, FileMode.Create)))
        {
            for (int i = 0; i < 10; i++)
            {
                int num = rand.Next(1, 100);
                generatedNumbers.Add(num);
                writer.Write(num);
            }
        }

        Console.WriteLine("Сгенерированные числа:");
        Console.WriteLine(string.Join(", ", generatedNumbers));

        // Читаем файл и считаем количество квадратов нечётных чисел
        int oddSquares = 0;
        using (BinaryReader reader = new BinaryReader(File.Open(fileName, FileMode.Open)))
        {
            while (reader.BaseStream.Position < reader.BaseStream.Length)
            {
                int number = reader.ReadInt32();
                if (number % 2 != 0 && Math.Sqrt(number) % 1 == 0)
                {
                    oddSquares++;
                }
            }
        }

        Console.WriteLine($"Количество квадратов нечётных чисел: {oddSquares}");
    }

    // Задание 5 (Пример для бинарных файлов и структур с XML сериализацией)
    [Serializable]
    public struct Toy
    {
        public string Name;
        public decimal Price;
        public string AgeRange;
    }

    public static void Task5()
    {
        string fileName = "toys.xml";
        List<Toy> toys = new List<Toy>
            {
                new Toy { Name = "Toy1", Price = 100m, AgeRange = "2-4" },
                new Toy { Name = "Toy2", Price = 200m, AgeRange = "5-7" },
                new Toy { Name = "Toy3", Price = 300m, AgeRange = "2-4" }
            };

        XmlSerializer serializer = new XmlSerializer(typeof(List<Toy>));
        using (StreamWriter writer = new StreamWriter(fileName))
        {
            serializer.Serialize(writer, toys);
        }

        // Десериализация и вывод самой дорогой игрушки для детей до 4 лет
        List<Toy> deserializedToys;
        using (StreamReader reader = new StreamReader(fileName))
        {
            deserializedToys = (List<Toy>)serializer.Deserialize(reader);
        }

        var mostExpensiveToy = deserializedToys
            .Where(t => t.AgeRange == "2-4")
            .OrderByDescending(t => t.Price)
            .FirstOrDefault();

        Console.WriteLine($"Самая дорогая игрушка для детей до 4 лет: {mostExpensiveToy.Name} с ценой {mostExpensiveToy.Price} руб.");
    }

    // Задание 6
    public static void Task6()
    {
        List<int> L1 = new List<int> { 1, 2, 3, 4, 5 };
        List<int> L2 = new List<int> { 3, 4, 5, 6, 7 };
        var commonElements = L1.Intersect(L2).ToList();
        Console.WriteLine($"Элементы, которые входят одновременно в оба списка: {string.Join(", ", commonElements)}");
    }

    // Задание 7
    public static void Task7()
    {
        LinkedList<int> L = new LinkedList<int>(new[] { 1, 2, 3 });
        var tempList = new LinkedList<int>(L.Reverse());
        foreach (var item in tempList)
        {
            L.AddLast(item);
        }
        Console.WriteLine($"Новый список: {string.Join(", ", L)}");
    }

    // Задание 8
    public static void Task8()
    {
        HashSet<string> trcList = new HashSet<string> { "ТРЦ1", "ТРЦ2", "ТРЦ3" };
        Dictionary<string, HashSet<string>> studentsTRC = new Dictionary<string, HashSet<string>>
            {
                { "Student1", new HashSet<string> { "ТРЦ1", "ТРЦ3" } },
                { "Student2", new HashSet<string> { "ТРЦ2", "ТРЦ3" } },
                { "Student3", new HashSet<string> { "ТРЦ1", "ТРЦ2" } }
            };

        var allVisited = trcList.Where(trc => studentsTRC.All(student => student.Value.Contains(trc))).ToList();
        var someVisited = trcList.Where(trc => studentsTRC.Any(student => student.Value.Contains(trc))).ToList();
        var noneVisited = trcList.Where(trc => studentsTRC.All(student => !student.Value.Contains(trc))).ToList();

        Console.WriteLine($"Все студенты посещали: {string.Join(", ", allVisited)}");
        Console.WriteLine($"Некоторые студенты посещали: {string.Join(", ", someVisited)}");
        Console.WriteLine($"Никто не посещал: {string.Join(", ", noneVisited)}");
    }

    // Задание 9
    public static void Task9()
    {
        string text = "Пример текста с разными буквами.";
        var distinctLetters = text.Where(c => Char.IsLetter(c)).Select(c => Char.ToLower(c)).Distinct();
        Console.WriteLine($"Количество разных букв в тексте: {distinctLetters.Count()}");
    }

    // Задание 10
    public static void Task10()
    {
        Dictionary<string, (int[], int)> scores = new Dictionary<string, (int[], int)>();
        scores.Add("Иванов Сергей", (new int[] { 100, 30, 78, 13 }, 221));
        scores.Add("Петров Антон", (new int[] { 90, 16, 98, 14 }, 218));
        scores.Add("Сидоров Юрий", (new int[] { 100, 70, 30, 21 }, 221));

        var sortedScores = scores.OrderByDescending(s => s.Value.Item2).ToList();
        int place = 1;
        foreach (var score in sortedScores)
        {
            Console.WriteLine($"{score.Key} {score.Value.Item2} {place}");
            place++;
        }
    }
}
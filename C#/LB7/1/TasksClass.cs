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
            var numbers = line.Split(' '); 
            foreach (var numStr in numbers)
            {
                if (int.TryParse(numStr, out int num)) 
                {
                    if (num % 2 == 0) 
                    {
                        product *= num; 
                    }
                }
            }
        }

        Console.WriteLine($"Произведение чётных элементов: {product}");
    }

    // Задание 3
    public static void Task3()
    {
        string inputFileName = "task3.txt";
        string outputFileName = "task3_output.txt";
        Class.CopyLinesWithoutLatinLetters(inputFileName, outputFileName);
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
        private string _name;
        private double _price;
        private string _ageRange;

        public string Name
        {
            get
            {
                return _name;
            }
            set
            {
                _name = value;
            }
        }
        public double Price
        {
            get
            {
                return _price;
            }
            set
            {
                _price = value;
            }
        }
        public string AgeRange
        {
            get
            {
                return _ageRange;
            }
            set
            {
                _ageRange = value;
            }
        }
    }



    public static void Task5()
    {
        string fileName = "toys.xml";
        List<Toy> toys = new List<Toy>
    {
        new Toy { Name = "Мишка", Price = 100, AgeRange = "2-4" },
        new Toy { Name = "Медведь", Price = 200, AgeRange = "5-7" },
        new Toy { Name = "Незнайка", Price = 300, AgeRange = "2-4" }
    };

        XmlSerializer serializer = new XmlSerializer(typeof(List<Toy>));
        using (StreamWriter writer = new StreamWriter(fileName))
        {
            serializer.Serialize(writer, toys);
        }

        List<Toy> deserializedToys;
        using (StreamReader reader = new StreamReader(fileName))
        {
            deserializedToys = (List<Toy>)serializer.Deserialize(reader);
        }

        Toy mostExpensiveToy = new Toy();
        double maxPrice = 0;

        foreach (var toy in deserializedToys)
        {
            if (toy.AgeRange == "2-4" && toy.Price > maxPrice)
            {
                mostExpensiveToy = toy;
                maxPrice = toy.Price;
            }
        }

        if (mostExpensiveToy.Name != null)
        {
            Console.WriteLine($"Самая дорогая игрушка для детей до 4 лет: {mostExpensiveToy.Name} с ценой {mostExpensiveToy.Price} руб.");
        }
        else
        {
            Console.WriteLine("Игрушки для детей до 4 лет не найдены.");
        }
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
        LinkedList<int> L = new LinkedList<int>(new[] { 1, 2, 3, 4, 5, 10 });
        var tempList = new LinkedList<int>(L.Reverse());
        foreach (var item in tempList)
        {
            L.AddLast(item);
        }
        Console.WriteLine($"Новый список: {string.Join(", ", L)}");
    }

    // Задание 9
    public static void Task9()
    {
        string text = "Надеюсь сдам ЯПОНский";
        HashSet<char> distinctLetters = new HashSet<char>();

        foreach (char c in text)
        {
            if (Char.IsLetter(c))
            {
                distinctLetters.Add(Char.ToLower(c));
            }
        }

        Console.WriteLine($"Количество разных букв в тексте: {distinctLetters.Count}");
    }


    // Задание 10
    public static void Task10()
    {
        // Запрос количества спортсменов с валидацией
        int N = InputValidator.ValidateNumberOfAthletes();

        // Запрос количества видов спорта с валидацией
        int M = InputValidator.ValidateNumberOfSports();

        // Словарь для хранения информации о спортсменах
        Dictionary<string, (int sum, int[] scores)> scores = new Dictionary<string, (int sum, int[] scores)>();

        // Чтение данных о спортсменах с валидацией
        for (int i = 0; i < N; i++)
        {
            var (fullName, individualScores) = InputValidator.ValidateAthleteData(M);
            int sum = 0;
            // Суммируем баллы вручную (без использования LINQ)
            foreach (var score in individualScores)
            {
                sum += score;
            }

            // Добавляем информацию о спортсмене в словарь
            scores[fullName] = (sum, individualScores);
        }

        // Сортировка спортсменов по убыванию суммы баллов (без LINQ)
        var sortedScores = new List<KeyValuePair<string, (int sum, int[] scores)>>(scores);
        sortedScores.Sort((x, y) => y.Value.sum.CompareTo(x.Value.sum));

        // Вывод результатов с указанием мест
        Console.WriteLine("\nРезультаты соревнований (сортировка по убыванию суммы баллов):");
        int place = 1;
        int previousScore = -1; // Для отслеживания одинаковых баллов
        foreach (var score in sortedScores)
        {
            if (score.Value.sum != previousScore)
            {
                // Если баллы изменились, обновляем место
                place = sortedScores.IndexOf(score) + 1;
            }

            Console.WriteLine($"{score.Key} {score.Value.sum} {place}");

            previousScore = score.Value.sum;
        }
    }
}

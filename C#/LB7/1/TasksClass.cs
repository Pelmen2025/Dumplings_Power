using System;
using System.Collections.Generic;
using System.IO;
using System.Xml.Serialization;

public static class TasksClass
{
    // Задание 1
    public static void Task1()//
    {
        string fileName = "task1.txt";
        Class.FillRandomDataToFile(fileName, 10);
        string[] lines = File.ReadAllLines(fileName);
        Console.WriteLine("Содержимое файла task1.txt:");
        for (int i = 0; i < lines.Length; i++)
        {
            Console.WriteLine(lines[i]);
        }
        int min = int.MaxValue;
        int max = int.MinValue;

        for (int i = 0; i < lines.Length; i++)
        {
            if (int.TryParse(lines[i], out int num))
            {
                if (num < min) min = num;
                if (num > max) max = num;
            }
        }
        double avg = (min + max) / 2.0;
        Console.WriteLine($"Среднее арифметическое максимального и минимального элементов: {avg}");
    }

    // Задание 2
    public static void Task2()//
    {
        string fileName = "task2.txt";
        Class.FillRandomDataToFileWithMultipleNumbers(fileName, 5, 4);
        string[] lines = File.ReadAllLines(fileName);
        Console.WriteLine("Содержимое файла task2.txt:");
        for (int i = 0; i < lines.Length; i++)
        {
            Console.WriteLine(lines[i]);
        }
        long product = 1;

        for (int i = 0; i < lines.Length; i++)
        {
            string[] numbers = lines[i].Split(' ');
            for (int j = 0; j < numbers.Length; j++)
            {
                if (int.TryParse(numbers[j], out int num))
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
    public static void Task3()//
    {
        string inputFileName = "task3.txt";
        string outputFileName = "task3_output.txt";
        Console.WriteLine("Содержимое файла task3.txt:");
        string[] lines = File.ReadAllLines(inputFileName);
        for (int i = 0; i < lines.Length; i++)
        {
            Console.WriteLine(lines[i]);
        }
        Class.CopyLinesWithoutLatinLetters(inputFileName, outputFileName);
        Console.WriteLine("Содержимое файла task3_output.txt (строки без латинских букв):");
        lines = File.ReadAllLines(outputFileName);
        for (int i = 0; i < lines.Length; i++)
        {
            Console.WriteLine(lines[i]);
        }
        Console.WriteLine($"Строки без латинских букв записаны в файл {outputFileName}");
    }

    // Задание 4
    public static void Task4()
    {
        string fileName = "task4.bin";
        Class.FillRandomBinaryData(fileName, 10);
        int[] generatedNumbers = new int[10];
        int index = 0;

        using (BinaryReader reader = new BinaryReader(File.Open(fileName, FileMode.Open)))
        {
            while (reader.BaseStream.Position < reader.BaseStream.Length)
            {
                generatedNumbers[index++] = reader.ReadInt32();
            }
        }

        Console.WriteLine("Содержимое файла task4.bin (сгенерированные числа):");
        for (int i = 0; i < generatedNumbers.Length; i++)
        {
            Console.Write((i < generatedNumbers.Length - 1) ? $"{generatedNumbers[i]}, " : $"{generatedNumbers[i]}");
        }
        Console.WriteLine();

        int oddSquares = 0;
        for (int i = 0; i < generatedNumbers.Length; i++)
        {
            int number = generatedNumbers[i];
            if (number % 2 != 0)
            {
                double sqrt = Math.Sqrt(number);
                if (sqrt == (int)sqrt)
                {
                    oddSquares++;
                }
            }
        }

        Console.WriteLine($"Количество квадратов нечётных чисел: {oddSquares}");
    }

    // Задание 5
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

        for (int i = 0; i < deserializedToys.Count; i++)
        {
            Toy toy = deserializedToys[i];
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
        List<int> commonElements = new List<int>();

        for (int i = 0; i < L1.Count; i++)
        {
            for (int j = 0; j < L2.Count; j++)
            {
                if (L1[i] == L2[j])
                {
                    bool exists = false;
                    for (int k = 0; k < commonElements.Count; k++)
                    {
                        if (commonElements[k] == L1[i])
                        {
                            exists = true;
                            break;
                        }
                    }
                    if (!exists)
                    {
                        commonElements.Add(L1[i]);
                    }
                }
            }
        }

        Console.WriteLine("Элементы, которые входят одновременно в оба списка: ");
        for (int i = 0; i < commonElements.Count; i++)
        {
            Console.Write((i < commonElements.Count - 1) ? $"{commonElements[i]}, " : $"{commonElements[i]}");
        }
        Console.WriteLine();
    }

    // Задание 7
    public static void Task7()
    {
        LinkedList<int> L = new LinkedList<int>(new[] { 1, 2, 3, 4, 5, 10 });
        LinkedList<int> tempList = new LinkedList<int>();
        LinkedListNode<int> node = L.Last;
        while (node != null)
        {
            tempList.AddLast(node.Value);
            node = node.Previous;
        }
        node = tempList.First;
        while (node != null)
        {
            L.AddLast(node.Value);
            node = node.Next;
        }
        Console.WriteLine("Новый список: ");
        node = L.First;
        while (node != null)
        {
            Console.Write(node.Next != null ? $"{node.Value}, " : $"{node.Value}");
            node = node.Next;
        }
        Console.WriteLine();
    }

    // Задание 8
    public static void Task8()
    {
        string fileName = "task8.txt";

        if (!File.Exists(fileName))
        {
            Console.WriteLine($"Ошибка: Файл {fileName} не найден.");
            return;
        }

        Console.WriteLine("Содержимое файла task8.txt:");
        string[] lines = File.ReadAllLines(fileName);
        for (int i = 0; i < lines.Length; i++)
        {
            Console.WriteLine(lines[i]);
        }

        if (lines.Length == 0)
        {
            Console.WriteLine("Ошибка: Файл пуст.");
            return;
        }

        string[] allTRCs = lines[0].Split(' ');
        if (allTRCs.Length == 0)
        {
            Console.WriteLine("Ошибка: Список ТРЦ пуст.");
            return;
        }

        List<string> allVisited = new List<string>();
        for (int i = 0; i < allTRCs.Length; i++)
        {
            allVisited.Add(allTRCs[i]);
        }

        List<string> someVisited = new List<string>();

        for (int i = 1; i < lines.Length; i++)
        {
            string[] parts = lines[i].Split(' ');
            if (parts.Length < 1)
            {
                Console.WriteLine($"Предупреждение: Пропущена строка {i + 1} (пустая или некорректная).");
                continue;
            }
            string[] visitedTRCs = new string[parts.Length - 1];
            for (int j = 1; j < parts.Length; j++)
            {
                visitedTRCs[j - 1] = parts[j];
            }

            for (int j = 0; j < allVisited.Count; j++)
            {
                bool found = false;
                for (int k = 0; k < visitedTRCs.Length; k++)
                {
                    if (allVisited[j] == visitedTRCs[k])
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    allVisited.RemoveAt(j);
                    j--;
                }
            }

            for (int j = 0; j < visitedTRCs.Length; j++)
            {
                bool exists = false;
                for (int k = 0; k < someVisited.Count; k++)
                {
                    if (someVisited[k] == visitedTRCs[j])
                    {
                        exists = true;
                        break;
                    }
                }
                if (!exists)
                {
                    someVisited.Add(visitedTRCs[j]);
                }
            }
        }

        for (int i = 0; i < someVisited.Count; i++)
        {
            for (int j = 0; j < allVisited.Count; j++)
            {
                if (someVisited[i] == allVisited[j])
                {
                    someVisited.RemoveAt(i);
                    i--;
                    break;
                }
            }
        }

        List<string> notVisited = new List<string>();
        for (int i = 0; i < allTRCs.Length; i++)
        {
            bool visited = false;
            for (int j = 0; j < allVisited.Count; j++)
            {
                if (allTRCs[i] == allVisited[j])
                {
                    visited = true;
                    break;
                }
            }
            if (!visited)
            {
                for (int j = 0; j < someVisited.Count; j++)
                {
                    if (allTRCs[i] == someVisited[j])
                    {
                        visited = true;
                        break;
                    }
                }
            }
            if (!visited)
            {
                notVisited.Add(allTRCs[i]);
            }
        }

        Console.WriteLine("\nТРЦ, посещенные всеми студентами:");
        if (allVisited.Count == 0)
        {
            Console.WriteLine("Таких ТРЦ нет.");
        }
        else
        {
            for (int i = 0; i < allVisited.Count; i++)
            {
                Console.WriteLine(allVisited[i]);
            }
        }

        Console.WriteLine("\nТРЦ, посещенные некоторыми студентами (но не всеми):");
        if (someVisited.Count == 0)
        {
            Console.WriteLine("Таких ТРЦ нет.");
        }
        else
        {
            for (int i = 0; i < someVisited.Count; i++)
            {
                Console.WriteLine(someVisited[i]);
            }
        }

        Console.WriteLine("\nТРЦ, которые никто не посещал:");
        if (notVisited.Count == 0)
        {
            Console.WriteLine("Таких ТРЦ нет.");
        }
        else
        {
            for (int i = 0; i < notVisited.Count; i++)
            {
                Console.WriteLine(notVisited[i]);
            }
        }

    }

    // Задание 9
    public static void Task9()
    {
        string text = "Надеюсь сдам ЯПОНский";
        HashSet<char> distinctLetters = new HashSet<char>();

        for (int i = 0; i < text.Length; i++)
        {
            char c = text[i];
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
        int N = InputValidator.ValidateNumberOfAthletes();
        int M = InputValidator.ValidateNumberOfSports();
        Dictionary<string, (int sum, int[] scores)> scores = new Dictionary<string, (int sum, int[] scores)>();

        for (int i = 0; i < N; i++)
        {
            var (fullName, individualScores) = InputValidator.ValidateAthleteData(M);
            int sum = 0;
            for (int j = 0; j < individualScores.Length; j++)
            {
                sum += individualScores[j];
            }
            scores[fullName] = (sum, individualScores);
        }

        KeyValuePair<string, (int sum, int[] scores)>[] sortedScores =
            new KeyValuePair<string, (int sum, int[] scores)>[scores.Count];
        int index = 0;
        foreach (var pair in scores)
        {
            sortedScores[index] = pair;
            index++;
        }

        for (int i = 0; i < sortedScores.Length - 1; i++)
        {
            for (int j = 0; j < sortedScores.Length - i - 1; j++)
            {
                if (sortedScores[j].Value.sum < sortedScores[j + 1].Value.sum)
                {
                    var temp = sortedScores[j];
                    sortedScores[j] = sortedScores[j + 1];
                    sortedScores[j + 1] = temp;
                }
            }
        }

        Console.WriteLine("\nРезультаты соревнований (сортировка по убыванию суммы баллов):");
        int place = 1;
        int previousScore = -1;
        for (int i = 0; i < sortedScores.Length; i++)
        {
            if (sortedScores[i].Value.sum != previousScore)
            {
                place = i + 1;
            }
            Console.WriteLine($"{sortedScores[i].Key} {sortedScores[i].Value.sum} {place}");
            previousScore = sortedScores[i].Value.sum;
        }
    }
}
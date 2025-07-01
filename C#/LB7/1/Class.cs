using System;
using System.IO;
using System.Text;

public class Class
{
    // Генерация случайных чисел для задания 1 (по одному числу в строке)
    public static void FillRandomDataToFile(string fileName, int lineCount, string separator = "\n")
    {
        Random rand = new Random();
        using (StreamWriter writer = new StreamWriter(fileName))
        {
            for (int i = 0; i < lineCount; i++)
            {
                writer.WriteLine(rand.Next(1, 100)); // Заполняем случайными числами от 1 до 99
            }
        }
    }

    // Генерация случайных чисел для задания 2 (несколько чисел в строке)
    public static void FillRandomDataToFileWithMultipleNumbers(string fileName, int lineCount, int numbersPerLine)
    {
        Random rand = new Random();
        using (StreamWriter writer = new StreamWriter(fileName))
        {
            for (int i = 0; i < lineCount; i++)
            {
                string[] numbers = new string[numbersPerLine];
                for (int j = 0; j < numbersPerLine; j++)
                {
                    numbers[j] = rand.Next(1, 9).ToString();
                }
                writer.WriteLine(string.Join(" ", numbers));
            }
        }
    }

    // Копирование строк без латинских букв для задания 3
    public static void CopyLinesWithoutLatinLetters(string inputFileName, string outputFileName)
    {
        using (StreamReader reader = new StreamReader(inputFileName, Encoding.UTF8))
        using (StreamWriter writer = new StreamWriter(outputFileName, false, Encoding.UTF8))
        {
            string line;
            while ((line = reader.ReadLine()) != null)
            {
                bool hasLatin = false;
                for (int i = 0; i < line.Length; i++)
                {
                    char c = line[i];
                    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
                    {
                        hasLatin = true;
                        break;
                    }
                }
                if (!hasLatin)
                {
                    writer.WriteLine(line);
                }
            }
        }
    }

    // Генерация случайных чисел для задания 4 (бинарный файл)
    public static void FillRandomBinaryData(string fileName, int numberCount)
    {
        Random rand = new Random();
        using (BinaryWriter writer = new BinaryWriter(File.Open(fileName, FileMode.Create)))
        {
            for (int i = 0; i < numberCount; i++)
            {
                writer.Write(rand.Next(1, 100)); // Заполняем случайными числами от 1 до 99
            }
        }
    }
}
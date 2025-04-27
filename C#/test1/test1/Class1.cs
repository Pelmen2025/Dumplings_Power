public class BaseClass
{
    public static void FillRandomDataToFile(string fileName, int lineCount, string separator = "\n")
    {
        Random rand = new Random();
        using (StreamWriter writer = new StreamWriter(fileName))
        {
            for (int i = 0; i < lineCount; i++)
            {
                writer.WriteLine(rand.Next(1, 100));  // Заполняем случайными числами
            }
        }
    }

    public static void FillRandomDataToFileWithMultipleNumbers(string fileName, int lineCount, int numbersPerLine)
    {
        Random rand = new Random();
        using (StreamWriter writer = new StreamWriter(fileName))
        {
            for (int i = 0; i < lineCount; i++)
            {
                writer.WriteLine(string.Join(" ", Enumerable.Range(0, numbersPerLine).Select(x => rand.Next(1, 100))));
            }
        }
    }

    public static void CopyLinesWithoutLatinLetters(string inputFileName, string outputFileName)
    {
        using (StreamReader reader = new StreamReader(inputFileName))
        using (StreamWriter writer = new StreamWriter(outputFileName))
        {
            string line;
            while ((line = reader.ReadLine()) != null)
            {
                if (!line.Any(c => c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z'))
                {
                    writer.WriteLine(line);
                }
            }
        }
    }
}
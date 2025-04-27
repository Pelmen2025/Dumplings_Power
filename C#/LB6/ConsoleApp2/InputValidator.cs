using System;

public static class InputValidator
{
    public static double ReadDouble(string prompt)
    {
        double result;
        while (true)
        {
            Console.Write(prompt);
            var input = Console.ReadLine();
            if (double.TryParse(input, out result))
            {
                return result;
            }

            Console.WriteLine("Ошибка: введите корректное число.");
        }
    }
}

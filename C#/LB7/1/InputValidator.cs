using System;

public static class InputValidator
{
    // Проверка ввода количества спортсменов
    public static int ValidateNumberOfAthletes()
    {
        int numberOfAthletes;
        while (true)
        {
            Console.WriteLine("Введите количество спортсменов (N):");
            if (int.TryParse(Console.ReadLine(), out numberOfAthletes) && numberOfAthletes > 0 && numberOfAthletes < 1000)
            {
                return numberOfAthletes;
            }
            else
            {
                Console.WriteLine("Ошибка: Введите корректное число спортсменов (от 1 до 999).");
            }
        }
    }

    // Проверка ввода количества видов спорта
    public static int ValidateNumberOfSports()
    {
        int numberOfSports;
        while (true)
        {
            Console.WriteLine("Введите количество видов спорта (M):");
            if (int.TryParse(Console.ReadLine(), out numberOfSports) && numberOfSports > 0)
            {
                return numberOfSports;
            }
            else
            {
                Console.WriteLine("Ошибка: Введите корректное число видов спорта (больше 0).");
            }
        }
    }

    // Проверка ввода данных спортсмена
    public static (string, int[]) ValidateAthleteData(int M)
    {
        while (true)
        {
            Console.WriteLine("Введите данные спортсмена (Фамилия Имя Баллы):");
            string[] input = Console.ReadLine().Split();
            if (input.Length == M + 2)
            {
                string fullName = input[0] + " " + input[1];
                int[] scores = new int[M];
                bool validScores = true;

                for (int i = 2; i < input.Length; i++)
                {
                    if (!int.TryParse(input[i], out scores[i - 2]))
                    {
                        validScores = false;
                        break;
                    }
                }

                if (validScores)
                {
                    return (fullName, scores);
                }
                else
                {
                    Console.WriteLine("Ошибка: Баллы должны быть целыми числами.");
                }
            }
            else
            {
                Console.WriteLine($"Ошибка: Неверное количество данных для спортсмена. Ожидается {M + 2} элемента(ов).");
            }
        }
    }
}

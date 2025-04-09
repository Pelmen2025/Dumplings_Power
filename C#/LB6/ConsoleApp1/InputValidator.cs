public static class InputValidator
{
    public static bool GetBooleanInput(string prompt)
    {
        while (true)
        {
            Console.Write($"{prompt} (1 — да, 2 — нет): ");
            string input = Console.ReadLine();

            if (input == "1")
            {
                return true;
            }
            else if (input == "2")
            {
                return false;
            }
            else
            {
                Console.WriteLine("Неверный ввод. Пожалуйста, введите 1 или 2.");
            }
        }
    }
}

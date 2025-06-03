using System;
using System.IO;

namespace PascalLexer
{
    class Program
    {
        static void Main()
        {
            try
            {
                if (!File.Exists("test_code.txt"))
                {
                    Console.WriteLine("Ошибка: файл test_code.txt не найден!");
                    Console.WriteLine("Создайте файл с кодом на Паскале в папке с программой.");
                    return;
                }

                using var fs = new StreamReader("test_code.txt");
                InputOutput.SetFile(fs);

                Console.WriteLine("ЛЕКСИЧЕСКИЙ АНАЛИЗАТОР PASCAL\n");
                LexicalAnalyzer analyzer = new LexicalAnalyzer();
                analyzer.PrintSymbolTable();
                Console.WriteLine("Начинаем лексический анализ...\n");
                analyzer.Analyze();
                Console.WriteLine("\nВсего обработано токенов: " + analyzer.TokenCodes.Count);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Критическая ошибка: {ex.Message}");
                Console.WriteLine("Подробности:");
                Console.WriteLine(ex.StackTrace);
            }
            finally
            {
                Console.WriteLine("\nНажмите любую клавишу для выхода...");
                Console.ReadKey();
            }
        }
    }
}
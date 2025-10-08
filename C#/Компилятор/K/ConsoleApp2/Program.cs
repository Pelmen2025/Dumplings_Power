using System;
using System.IO;

namespace PascalAnalyzer
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
                    CreateSampleFile();
                    return;
                }

                using StreamReader file = new("test_code.txt");
                InputOutput.SetFile(file);

                Console.WriteLine("СИНТАКСИЧЕСКИЙ АНАЛИЗАТОР PASCAL\n");

                LexicalAnalyzer lexer = new();
                lexer.Analyze();
                Console.WriteLine($"\nВсего обработано токенов: {lexer.TokenCodes.Count}");

                if (InputOutput.ErrorCount == 0)
                {
                    Console.WriteLine("\nЛексических ошибок не найдено. Переходим к синтаксическому анализу.");
                    SyntaxAnalyzer syntaxAnalyzer = new(lexer);
                    syntaxAnalyzer.Analyze();
                }
                else
                {
                    Console.WriteLine("\nОбнаружены лексические ошибки. Синтаксический анализ пропущен.");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Критическая ошибка: {ex.Message}");
            }
            finally
            {
                InputOutput.PrintErrors();
                Console.WriteLine("\nНажмите любую клавишу для выхода...");
                Console.ReadKey();
            }
        }

        private static void CreateSampleFile()
        {
            string sampleCode = @"var
    x, y integer;  { ошибка: пропущено двоеточие }
    z: real;
    a char;  { ошибка: пропущена запятая }
begin
    x := 10;
    y = x + 5;  { ошибка: = вместо := }
    for i := 1 to 10  { ошибка: пропущено do }
        x := x + i;
    while y > 0  { ошибка: пропущено do }
        y := y - 1;
    z := 3;
end.";

            try
            {
                File.WriteAllText("test_code.txt", sampleCode);
                Console.WriteLine("Создан пример файла test_code.txt с синтаксическими ошибками.");
                Console.WriteLine("Перезапустите программу для анализа.");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Не удалось создать файл: {ex.Message}");
            }
        }
    }
}
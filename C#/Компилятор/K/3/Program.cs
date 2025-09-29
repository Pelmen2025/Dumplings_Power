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
                    CreateSampleFile(); // <- НОВОЕ: автоматическое создание примера
                    return;
                }

                using var fs = new StreamReader("test_code.txt");
                InputOutput.SetFile(fs);

                Console.WriteLine("ЛЕКСИЧЕСКИЙ И СИНТАКСИЧЕСКИЙ АНАЛИЗАТОР PASCAL\n"); // <- ОБНОВЛЕНО: добавлено "И СИНТАКСИЧЕСКИЙ"

                // Лексический анализ
                LexicalAnalyzer lexer = new LexicalAnalyzer();
                lexer.PrintSymbolTable();
                Console.WriteLine("Начинаем лексический анализ...\n");
                lexer.Analyze();
                Console.WriteLine($"\nВсего обработано токенов: {lexer.TokenCodes.Count}");

                // НОВОЕ: Синтаксический анализ
                if (InputOutput.AllErrors.Count == 0)
                {
                    Console.WriteLine("\nЛексических ошибок не найдено. Переходим к синтаксическому анализу.");
                    SyntaxAnalyzer syntaxAnalyzer = new SyntaxAnalyzer(lexer);
                    syntaxAnalyzer.Analyze();
                }
                else
                {
                    Console.WriteLine("\nОбнаружены лексические ошибки. Синтаксический анализ пропущен.");
                }

                // ОБНОВЛЕНО: Итоговая печать ошибок (вместо вызова в lexer.Analyze())
                InputOutput.PrintErrors();

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

        // НОВОЕ: Метод для создания примера файла
        private static void CreateSampleFile()
        {
            string sampleCode = @"var
  x, y: integer;
  z real;  { ошибка: пропущено двоеточие }
  a: char;
begin
  x := 10;
  y = x + 5;  { ошибка: должно быть := вместо = }
  for i := 1 to 10
    x := x + i;  { ошибка: пропущено do }
  while y > 0
    begin  { ошибка: пропущено do }
      y := y - 1
    end;
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
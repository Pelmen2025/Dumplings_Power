using System;
using System.IO;
using System.Linq;

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

                Console.WriteLine("╔════════════════════════════════════════════╗");
                Console.WriteLine("║   АНАЛИЗАТОР ЯЗЫКА PASCAL                  ║");
                Console.WriteLine("╚════════════════════════════════════════════╝\n");

                LexicalAnalyzer lexer = new LexicalAnalyzer();
                lexer.PrintSymbolTable();

                Console.WriteLine("─────────────────────────────────────────────");
                Console.WriteLine("ЭТАП 1: ЛЕКСИЧЕСКИЙ АНАЛИЗ");
                Console.WriteLine("─────────────────────────────────────────────\n");

                lexer.Analyze();
                Console.WriteLine($"\n✓ Всего обработано токенов: {lexer.TokenCodes.Count}");

                
                var criticalErrors = InputOutput.AllErrors
                    .Where(e => e.ErrorCode == 1 || e.ErrorCode == 7 ||
                               e.ErrorCode == 12 || e.ErrorCode == 203)
                    .ToList();

                if (criticalErrors.Count > 0)
                {
                    Console.WriteLine("\n⚠ Синтаксический анализ не выполнен из-за критических лексических ошибок");
                    Console.WriteLine("  (недопустимые символы, незакрытые строки/комментарии, переполнение)");
                    return;
                }

                Console.WriteLine("\n─────────────────────────────────────────────");
                Console.WriteLine("ЭТАП 2: СИНТАКСИЧЕСКИЙ АНАЛИЗ");
                Console.WriteLine("─────────────────────────────────────────────");

                SyntaxAnalyzer syntax = new SyntaxAnalyzer(lexer);
                syntax.Analyze();

            }
            catch (Exception ex)
            {
                Console.WriteLine($"\n✗ Критическая ошибка: {ex.Message}");
                Console.WriteLine("\nПодробности:");
                Console.WriteLine(ex.StackTrace);
            }
            finally
            {
                Console.WriteLine("\n─────────────────────────────────────────────");
                Console.WriteLine("Нажмите любую клавишу для выхода...");
                Console.ReadKey();
            }
        }
    }
}
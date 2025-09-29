using System;
using System.Collections.Generic;
using System.IO;

namespace PascalLexer
{
    class InputOutput
    {
        private static StreamReader file;
        private static char ch;
        private static int lineNumber;
        private static int charNumber;
        private static List<Err> allErrors = new List<Err>();
        private static List<string> lines = new List<string>();
        private static string currentLine = "";
        private static Dictionary<byte, string> errorMessages = new Dictionary<byte, string>
        {
            // Лексические ошибки
            { 1, "Недопустимый символ" },
            { 2, "Ожидалась точка в конце программы" },
            { 7, "Незакрытая строковая константа" },
            { 12, "Незакрытый комментарий" },
            { 203, "Число выходит за пределы допустимого диапазона" },
            
            // Синтаксические ошибки
            { 10, "Ожидалось ключевое слово 'var'" },
            { 11, "Ожидался идентификатор" },
            { 13, "Ожидался тип переменной" },
            { 14, "Ожидалась точка с запятой" },
            { 15, "Ожидался оператор присваивания ':='" },
            { 16, "Ожидалось выражение" },
            { 17, "Ожидалось ключевое слово 'begin'" },
            { 18, "Ожидалось ключевое слово 'end'" },
            { 19, "Ожидалось ключевое слово 'do'" },
            { 20, "Ожидалось 'to' или 'downto'" },
            { 21, "Ожидалось ключевое слово 'while'" },
            { 22, "Ожидалась открывающая скобка '('" },
            { 23, "Ожидалась закрывающая скобка ')'" },
            { 24, "Неожиданный символ" },
            { 25, "Ожидалось двоеточие" }
        };

        public static char Ch => ch;
        public static int LineNumber => lineNumber;
        public static int CharNumber => charNumber;
        public static List<Err> AllErrors => allErrors;
        public static List<string> Lines => lines;

        public static void SetFile(StreamReader reader)
        {
            file = reader;
            lineNumber = 1;
            charNumber = 0;
            ch = ' ';
            lines.Clear();
            allErrors.Clear();
            currentLine = "";
        }

        public static void NextCh()
        {
            if (file.Peek() != -1)
            {
                ch = (char)file.Read();
                if (ch == '\n')
                {
                    if (currentLine.Length > 0)
                    {
                        lines.Add(currentLine);
                        currentLine = "";
                    }
                    lineNumber++;
                    charNumber = 0;
                }
                else if (ch != '\r') // Игнорируем \r для корректной обработки Windows line endings
                {
                    currentLine += ch;
                    charNumber++;
                }
            }
            else
            {
                if (currentLine.Length > 0)
                {
                    lines.Add(currentLine);
                    currentLine = "";
                }
                ch = '\0';
            }
        }

        public static char LookAhead()
        {
            int next = file.Peek();
            return next == -1 ? '\0' : (char)next;
        }

        public static bool IsEndOfFile()
        {
            return file.Peek() == -1;
        }

        public static void Error(byte errorCode, TextPosition position)
        {
            if (allErrors.Count < 100 && !allErrors.Exists(e =>
                e.Position.LineNumber == position.LineNumber
                    && e.Position.CharNumber == position.CharNumber
                        && e.ErrorCode == errorCode))
            {
                allErrors.Add(new Err(position, errorCode));
            }
        }

        public static void PrintErrors()
        {
            // Диагностическая информация
            if (allErrors.Count > 0)
            {
                Console.WriteLine($"\nДИАГНОСТИКА ОШИБОК:");
                foreach (var error in allErrors)
                {
                    string message = errorMessages.ContainsKey(error.ErrorCode)
                        ? errorMessages[error.ErrorCode]
                        : "Неизвестная ошибка";
                    Console.WriteLine($"  Код {error.ErrorCode}: {message} в позиции {error.Position.LineNumber}:{error.Position.CharNumber}");
                }
            }

            if (allErrors.Count == 0)
            {
                Console.WriteLine("\nОшибок не обнаружено.");
                Console.WriteLine("Анализ завершен: ошибок - 0!");
                return;
            }

            Console.WriteLine($"\n--- ОТЧЕТ ОБ ОШИБКАХ ---");
            Console.WriteLine($"Всего найдено ошибок: {allErrors.Count}");

            // Правильная группировка ошибок по типам
            var lexicalErrors = allErrors.FindAll(e => e.ErrorCode < 10 || e.ErrorCode == 203);
            var syntaxErrors = allErrors.FindAll(e => e.ErrorCode >= 10 && e.ErrorCode != 203);

            if (lexicalErrors.Count > 0)
            {
                Console.WriteLine($"\nЛЕКСИЧЕСКИЕ ОШИБКИ ({lexicalErrors.Count}):");
                PrintErrorGroup(lexicalErrors);
            }

            if (syntaxErrors.Count > 0)
            {
                Console.WriteLine($"\nСИНТАКСИЧЕСКИЕ ОШИБКИ ({syntaxErrors.Count}):");
                PrintErrorGroup(syntaxErrors);
            }

            Console.WriteLine($"\nАнализ завершен: ошибок - {allErrors.Count}!");
        }

        private static void PrintErrorGroup(List<Err> errors)
        {
            // Сортируем ошибки по номеру строки
            errors.Sort((e1, e2) => e1.Position.LineNumber.CompareTo(e2.Position.LineNumber));

            for (int i = 0; i < lines.Count; i++)
            {
                var errorsOnLine = errors.FindAll(e => e.Position.LineNumber == i + 1);
                if (errorsOnLine.Count > 0)
                {
                    Console.WriteLine($"{i + 1:D3}: {lines[i]}");
                    foreach (var error in errorsOnLine)
                    {
                        int errorPosition = Math.Max(0, error.Position.CharNumber - 1);
                        string spaces = new string(' ', 5 + errorPosition);
                        string message = errorMessages.ContainsKey(error.ErrorCode)
                            ? errorMessages[error.ErrorCode]
                            : "Неизвестная ошибка";
                        Console.WriteLine($"{spaces}^ Ошибка {error.ErrorCode}: {message}");
                    }
                    Console.WriteLine();
                }
            }
        }

        // Добавим метод для сброса ошибок, если нужно принудительно перейти к синтаксическому анализу
        public static void ClearLexicalErrors()
        {
            // Удаляем только лексические ошибки, оставляя потенциальные синтаксические
            allErrors.RemoveAll(e => e.ErrorCode < 10 || e.ErrorCode == 203);
        }

        // Метод для проверки только синтаксических ошибок
        public static bool HasOnlySyntaxErrors()
        {
            return allErrors.All(e => e.ErrorCode >= 10 && e.ErrorCode != 203);
        }

        // Метод для подсчета лексических ошибок
        public static int LexicalErrorCount()
        {
            return allErrors.Count(e => e.ErrorCode < 10 || e.ErrorCode == 203);
        }
    }
}
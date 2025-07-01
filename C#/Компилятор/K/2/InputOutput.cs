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
            { 1, "Недопустимый символ" },
            { 2, "Ожидалась точка в конце программы" },
            { 7, "Незакрытая строковая константа" },
            { 12, "Незакрытый комментарий" },
            { 203, "Число выходит за пределы допустимого диапазона" }
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
                else
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
            return (char)file.Peek();
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
            if (allErrors.Count == 0)
            {
                Console.WriteLine("\nОшибок не обнаружено.");
                Console.WriteLine("Компиляция завершена: ошибок - 0!");
                return;
            }

            Console.WriteLine("\nОШИБКИ В ИСХОДНОМ КОДЕ ");
            for (int i = 0; i < lines.Count; i++)
            {
                Console.WriteLine(lines[i]);
                foreach (var error in allErrors)
                {
                    if (error.Position.LineNumber == i + 1)
                    {
                        string spaces = new string(' ', error.Position.CharNumber > 0 
                            ? error.Position.CharNumber - 1 : 0);
                        string message = errorMessages.ContainsKey(error.ErrorCode)
                            ? errorMessages[error.ErrorCode]
                            : "Неизвестная ошибка";
                        Console.WriteLine($"{spaces}^ **{allErrors.IndexOf(error) + 1:00}**: ошибка {error.ErrorCode} - {message}");
                    }
                }
            }

            Console.WriteLine($"\nКомпиляция завершена: ошибок - {allErrors.Count}!");
        }
    }
}
using System;
using System.Collections.Generic;
using System.IO;

namespace Компилятор
{
    struct TextPosition
    {
        public uint lineNumber;
        public byte charNumber;

        public TextPosition(uint ln = 0, byte c = 0)
        {
            lineNumber = ln;
            charNumber = c;
        }
    }

    struct Err
    {
        public TextPosition errorPosition;
        public byte errorCode;

        public Err(TextPosition errorPosition, byte errorCode)
        {
            this.errorPosition = errorPosition;
            this.errorCode = errorCode;
        }
    }

    class InputOutput
    {
        const byte ERRMAX = 9;
        public static char Ch { get; set; }
        public static TextPosition positionNow = new TextPosition();
        static string line;
        static byte lastInLine = 0;

        // Все ошибки
        public static List<Err> allErrors = new List<Err>();

        // Ошибки для текущей строки
        public static List<Err> currentLineErrors = new List<Err>();

        static StreamReader File { get; set; }
        static uint errCount = 0;

        static readonly Dictionary<byte, string> errorMessages = new()
        {
            { 1, "Потеря ;" },
            { 2, "Неизвестный символ" },
            { 3, "Ожидалась )" },
            { 4, "Ожидался идентификатор" },
            { 5, "Неправильная скобка" }
        };

        public static void SetFile(StreamReader file)
        {
            File = file;
            positionNow = new TextPosition(0, 0);
            ReadNextLine();
        }

        public static void NextCh()
        {
            if (line == null)
                return;

            if (positionNow.charNumber == lastInLine)
            {
                ListThisLine();

                currentLineErrors = allErrors.FindAll(e => e.errorPosition.lineNumber == positionNow.lineNumber);

                if (currentLineErrors.Count > 0)
                    ListErrors();

                ReadNextLine();
                positionNow.lineNumber++;
                positionNow.charNumber = 0;
            }
            else
            {
                positionNow.charNumber++;
            }

            if (!string.IsNullOrEmpty(line) && positionNow.charNumber < line.Length)
                Ch = line[positionNow.charNumber];
            else
                Ch = '\0';
        }

        private static void ListThisLine()
        {
            Console.WriteLine(line);
        }

        private static void ReadNextLine()
        {
            if (!File.EndOfStream)
            {
                line = File.ReadLine();
                lastInLine = (byte)(line.Length - 1);
            }
            else
            {
                End();
                Environment.Exit(0);
            }
        }

        static void End()
        {
            Console.WriteLine($"\nКомпиляция завершена: ошибок - {errCount}!");
        }

        static void ListErrors()
        {
            foreach (Err item in currentLineErrors)
            {
                ++errCount;
                string spaces = new string(' ', item.errorPosition.charNumber);
                string message = errorMessages.ContainsKey(item.errorCode)
                    ? errorMessages[item.errorCode]
                    : "Неизвестная ошибка";

                Console.WriteLine($"{spaces}^ **{errCount:00}**: ошибка {item.errorCode} - {message}");
            }
        }

        public static void Error(byte errorCode, TextPosition position)
        {
            if (allErrors.Count < ERRMAX)
            {
                allErrors.Add(new Err(position, errorCode));
            }
        }
    }

    class TestInputOutput
    {
        public static void RunTests()
        {
            using var fs = new StreamReader("test_code.txt");
            InputOutput.SetFile(fs);

            // Строка 0: ошибка - потеря ;
            InputOutput.Error(1, new TextPosition(0, 9));

            // Строка 1: ошибка - неизвестный символ
            InputOutput.Error(2, new TextPosition(1, 0));

            // Строка 2: ошибка - неправильная скобка
            InputOutput.Error(5, new TextPosition(2, 8));

            // Строка 3: ошибка - неправильная скобка
            InputOutput.Error(5, new TextPosition(3, 11));

            while (true)
                InputOutput.NextCh();
        }
    }
}

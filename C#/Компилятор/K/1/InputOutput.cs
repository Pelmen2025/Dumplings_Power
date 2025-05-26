using System;
using System.Collections.Generic;
using System.IO;

namespace Компилятор
{
    struct TextPosition
    {
        public TextPosition(uint ln = 0, byte c = 0)
        {
            LineNumber = ln;
            CharNumber = c;
        }

        public uint LineNumber { get; set; }

        public byte CharNumber { get; set; }
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

            if (positionNow.CharNumber == lastInLine)
            {
                ListThisLine();

                currentLineErrors = allErrors.FindAll(e => e.errorPosition.LineNumber == positionNow.LineNumber);

                if (currentLineErrors.Count > 0)
                    ListErrors();

                ReadNextLine();
                positionNow.LineNumber++;
                positionNow.CharNumber = 0;
            }
            else
            {
                positionNow.CharNumber++;
            }

            if (!string.IsNullOrEmpty(line) && positionNow.CharNumber < line.Length)
                Ch = line[positionNow.CharNumber];
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
                string spaces = new string(' ', item.errorPosition.CharNumber);
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

            InputOutput.Error(1, new TextPosition(0, 18));
            InputOutput.NextCh();

            InputOutput.Error(1, new TextPosition(1, 20));
            InputOutput.NextCh();

            InputOutput.Error(1, new TextPosition(3, 8));
            InputOutput.NextCh();

            InputOutput.Error(1, new TextPosition(5, 12));
            InputOutput.NextCh();

            InputOutput.Error(1, new TextPosition(6, 24));
            InputOutput.NextCh();

            while (true)
                InputOutput.NextCh();
        }
    }
}

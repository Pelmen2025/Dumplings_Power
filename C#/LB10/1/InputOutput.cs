using System;
using System.Collections.Generic;
using System.IO;

namespace Компилятор
{
    struct TextPosition
    {
        public uint lineNumber; // номер строки
        public byte charNumber; // номер позиции в строке

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

<<<<<<< HEAD
    static class ErrorTable
    {
        public static readonly Dictionary<byte, string> Errors = new Dictionary<byte, string>
        {
            { 1, "Ожидалась переменная (нельзя считывать в константу или выражение)" },
            { 2, "Неизвестная процедура ввода/вывода" },
            { 3, "Несовместимость типов при вводе/выводе" },
            { 4, "Неверное количество аргументов в read/write" },
            { 5, "Синтаксическая ошибка (ожидались скобки или запятая)" },
            { 6, "Попытка чтения за пределами строки (EOF)" },
            { 7, "Неверный формат вывода (например, write(x:-5))" },
            { 8, "Слишком длинная строка" },
            { 9, "Переполнение значения при чтении (слишком большое число)" }
        };

        public static string GetMessage(byte code)
        {
            return Errors.TryGetValue(code, out string msg)
                ? msg
                : $"Неизвестная ошибка с кодом {code}";
        }
    }
=======
>>>>>>> origin/main

    class InputOutput
    {
        const byte ERRMAX = 9;
        public static char Ch { get; set; }
        public static TextPosition positionNow = new TextPosition();
        static string line;
        static byte lastInLine = 0;
<<<<<<< HEAD
        public static List<Err> err = new List<Err>();
        static StreamReader File { get; set; }
        static uint errCount = 0;

        public static void Init(string filePath)
        {
            File = new StreamReader(filePath);
            ReadNextLine();
        }

        public static void NextCh()
=======
        public static List<Err> err;
        static StreamReader File { get; set; }
        static uint errCount = 0;

        static public void NextCh()
>>>>>>> origin/main
        {
            if (positionNow.charNumber == lastInLine)
            {
                ListThisLine();
                if (err.Count > 0)
                    ListErrors();
<<<<<<< HEAD

                ReadNextLine();
                positionNow.lineNumber += 1;
                positionNow.charNumber = 0;
            }
            else
            {
                ++positionNow.charNumber;
            }

            if (line != null && positionNow.charNumber < line.Length)
                Ch = line[positionNow.charNumber];
            else
                Ch = '\0'; // символ конца строки
=======
                ReadNextLine();
                positionNow.lineNumber = positionNow.lineNumber + 1;
                positionNow.charNumber = 0;
            }
            else ++positionNow.charNumber;
            Ch = line[positionNow.charNumber];
>>>>>>> origin/main
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
<<<<<<< HEAD
                lastInLine = (byte)(line.Length - 1);
=======
>>>>>>> origin/main
                err = new List<Err>();
            }
            else
            {
                End();
            }
        }

        static void End()
        {
<<<<<<< HEAD
            Console.WriteLine($"Компиляция завершена: ошибок — {errCount}!");
            Environment.Exit(0);
=======
            Console.WriteLine($"Компиляция завершена: : ошибок — {errCount}!");
>>>>>>> origin/main
        }

        static void ListErrors()
        {
            int pos = 6 - $"{positionNow.lineNumber} ".Length;
            string s;
<<<<<<< HEAD

=======
>>>>>>> origin/main
            foreach (Err item in err)
            {
                ++errCount;
                s = "**";
                if (errCount < 10) s += "0";
                s += $"{errCount}**";
                while (s.Length - 1 < pos + item.errorPosition.charNumber) s += " ";
<<<<<<< HEAD
                s += $"^ ошибка код {item.errorCode}: {ErrorTable.GetMessage(item.errorCode)}";
=======
                s += $"^ ошибка код {item.errorCode}";
>>>>>>> origin/main
                Console.WriteLine(s);
            }
        }

<<<<<<< HEAD
        public static void Error(byte errorCode, TextPosition position)
        {
            if (err.Count <= ERRMAX)
            {
                Err e = new Err(position, errorCode);
=======
        static public void Error(byte errorCode, TextPosition position)
        {
            Err e;
            if (err.Count <= ERRMAX)
            {
                e = new Err(position, errorCode);
>>>>>>> origin/main
                err.Add(e);
            }
        }
    }
<<<<<<< HEAD
}
=======
}
>>>>>>> origin/main

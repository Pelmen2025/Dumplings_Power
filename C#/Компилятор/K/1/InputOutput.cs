namespace Компилятор
{
    struct TextPosition
    {
        public uint LineNumber { get; set; }
        public byte CharNumber { get; set; }

        public TextPosition(uint ln = 0, byte c = 0)
        {
            LineNumber = ln;
            CharNumber = c;
        }

        
    }

    struct Err
    {
        public TextPosition ErrorPosition { get; set; }
        public byte ErrorCode { get; set; }

        public Err(TextPosition errorPosition, byte errorCode)
        {
            this.ErrorPosition = errorPosition;
            this.ErrorCode = errorCode;
        }
    }

    class InputOutput
    {
        const byte ERRMAX = 9;
        public static char Ch { get; set; }
        private static TextPosition _positionNow;
        public static string Line { get; set; }
        public static byte LastInLine { get; set; }

        public static List<Err> AllErrors { get; set; }

        public static List<Err> CurrentLineErrors { get; set; }

        public static StreamReader File { get; set; }
        public static uint ErrCount { get; set; }

        static readonly Dictionary<byte, string> ErrorMessages = new()
        {
            { 1, "Потеря ;" },
            { 2, "Неизвестный символ" },
            { 3, "Ожидалась )" },
            { 4, "Ожидался идентификатор" },
            { 5, "Неправильная скобка" }
        };
        static InputOutput()
        {
            ErrCount = 0;
            LastInLine = 0;
            CurrentLineErrors = [];
            AllErrors = [];
        }
        public static void SetFile(StreamReader file)
        {

            File = file;
            _positionNow = new TextPosition(0, 0);
            ReadNextLine();
        }

        public static void NextCh()
        {
            if (Line == null)
                return;

            if (_positionNow.CharNumber == LastInLine)
            {
                ListThisLine();

                CurrentLineErrors = AllErrors.FindAll(e => e.ErrorPosition.LineNumber == _positionNow.LineNumber);

                if (CurrentLineErrors.Count > 0)
                    ListErrors();

                ReadNextLine();
                _positionNow.LineNumber++;
                _positionNow.CharNumber = 0;
            }
            else
            {
                _positionNow.CharNumber++;
            }

            if (!string.IsNullOrEmpty(Line) && _positionNow.CharNumber < Line.Length)
                Ch = Line[_positionNow.CharNumber];
            else
                Ch = '\0';
        }

        private static void ListThisLine()
        {
            Console.WriteLine(Line);
        }

        private static void ReadNextLine()
        {
            if (!File.EndOfStream)
            {
                Line = File.ReadLine();
                LastInLine = (byte)(Line.Length - 1);
            }
            else
            {
                End();
                Environment.Exit(0);
                //Line = null;
                //Ch = '\0';
            }
        }

        static void End()
        {
            Console.WriteLine($"\nКомпиляция завершена: ошибок - {ErrCount}!");
        }

        static void ListErrors()
        {
            foreach (Err item in CurrentLineErrors)
            {
                ++ErrCount;
                string spaces = new string(' ', item.ErrorPosition.CharNumber);
                string message = ErrorMessages.ContainsKey(item.ErrorCode)
                    ? ErrorMessages[item.ErrorCode]
                    : "Неизвестная ошибка";

                Console.WriteLine($"{spaces}^ **{ErrCount:00}**: ошибка {item.ErrorCode} - {message}");
            }
        }

        public static void Error(byte errorCode, TextPosition position)
        {
            if (AllErrors.Count < ERRMAX)
            {
                AllErrors.Add(new Err(position, errorCode));
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

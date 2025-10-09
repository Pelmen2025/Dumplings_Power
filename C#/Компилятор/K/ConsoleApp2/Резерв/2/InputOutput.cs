using System;
using System.Collections.Generic;
using System.IO;

namespace PascalAnalyzer
{
    static class InputOutput
    {
        private static StreamReader _file;
        private static char _ch;
        private static int _lineNumber;
        private static int _charNumber;
        private static readonly List<Err> _errors = new();
        private static readonly List<string> _lines = new();
        private static string _currentLine = "";
        private static readonly Dictionary<byte, string> _errorMessages = new()
        {
            // Лексические ошибки
            { 1, "Недопустимый символ" },
            { 7, "Незакрытая строковая константа" },
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
            { 22, "Ожидалась открывающая скобка '('" },
            { 23, "Ожидалась закрывающая скобка ')'" },
            { 25, "Ожидалось двоеточие" }
        };

        public static char Ch => _ch;
        public static int LineNumber => _lineNumber;
        public static int CharNumber => _charNumber;
        public static IReadOnlyList<Err> Errors => _errors.AsReadOnly();
        public static IReadOnlyList<string> Lines => _lines.AsReadOnly();
        public static int ErrorCount => _errors.Count;

        public static void SetFile(StreamReader reader)
        {
            _file = reader;
            _lineNumber = 1;
            _charNumber = 0;
            _ch = ' ';
            _lines.Clear();
            _errors.Clear();
            _currentLine = "";
        }

        public static void NextCh()
        {
            if (_file.Peek() != -1)
            {
                _ch = (char)_file.Read();
                if (_ch == '\n')
                {
                    if (_currentLine.Length > 0)
                    {
                        _lines.Add(_currentLine);
                        _currentLine = "";
                    }
                    _lineNumber++;
                    _charNumber = 0;
                }
                else if (_ch != '\r')
                {
                    _currentLine += _ch;
                    _charNumber++;
                }
            }
            else
            {
                if (_currentLine.Length > 0)
                {
                    _lines.Add(_currentLine);
                    _currentLine = "";
                }
                _ch = '\0';
            }
        }

        public static char LookAhead()
        {
            int next = _file.Peek();
            return next == -1 ? '\0' : (char)next;
        }

        public static bool IsEndOfFile()
        {
            return _file.Peek() == -1;
        }

        public static void AddError(byte errorCode, TextPosition position)
        {
            if (_errors.Count < 100 && !_errors.Exists(e =>
                e.Position.LineNumber == position.LineNumber &&
                e.Position.CharNumber == position.CharNumber &&
                e.ErrorCode == errorCode))
            {
                _errors.Add(new Err(position, errorCode));
            }
        }

        public static void PrintErrors()
        {
            if (_errors.Count == 0)
            {
                Console.WriteLine("\nОшибок не обнаружено.");
                Console.WriteLine("Анализ завершен: ошибок - 0!");
                return;
            }

            Console.WriteLine($"\n--- ОТЧЕТ ОБ ОШИБКАХ ---");
            Console.WriteLine($"Всего найдено ошибок: {_errors.Count}");

            foreach (var error in _errors)
            {
                string message = _errorMessages.GetValueOrDefault(error.ErrorCode, "Неизвестная ошибка");
                Console.WriteLine($"Ошибка {error.ErrorCode}: {message} в строке {error.Position.LineNumber}, позиция {error.Position.CharNumber}");
                if (error.Position.LineNumber <= _lines.Count)
                {
                    Console.WriteLine($"  {_lines[error.Position.LineNumber - 1]}");
                    Console.WriteLine($"  {new string(' ', error.Position.CharNumber - 1)}^");
                }
            }

            Console.WriteLine($"\nАнализ завершен: ошибок - {_errors.Count}!");
        }
    }
}
using System;
using System.Collections.Generic;

namespace PascalLexer
{
    class LexicalAnalyzer
    {
        public const byte
            star = 21, slash = 60, equal = 16, comma = 20, semicolon = 14,
            colon = 5, point = 61, arrow = 62, leftpar = 9, rightpar = 4,
            lbracket = 11, rbracket = 12, flpar = 63, frpar = 64, later = 65,
            greater = 66, charc = 83, laterequal = 67, greaterequal = 68,
            latergreater = 69, plus = 70, minus = 71, lcomment = 72,
            rcomment = 73, assign = 51, twopoints = 74, ident = 2,
            floatc = 82, intc = 15, casesy = 31, elsesy = 32, filesy = 57,
            gotosy = 33, thensy = 52, typesy = 34, untilsy = 53, dosy = 54,
            withsy = 37, ifsy = 56, insy = 100, ofsy = 101, orsy = 102,
            tosy = 103, endsy = 104, varsy = 105, divsy = 106, andsy = 107,
            notsy = 108, forsy = 109, modsy = 110, nilsy = 111, setsy = 112,
            labelsy = 117, beginsy = 113, whilesy = 114, arraysy = 115,
            constsy = 116, downtosy = 118, packedsy = 119, recordsy = 120,
            repeatsy = 121, programsy = 122, functionsy = 123, procedurensy = 124,
            integersy = 125, realsy = 126, charsy = 127;

        private const byte ERR_NUMBER_OVERFLOW = 203;
        private const byte ERR_INVALID_CHAR = 1;
        private const byte ERR_UNCLOSED_STRING = 7;
        private const byte ERR_UNCLOSED_COMMENT = 12;

        private readonly Keywords _keywords;
        private TextPosition _tokenPosition;
        private byte _symbol;
        private string _addrName = "";
        private int _nmbInt;
        private float _nmbFloat;
        private char _oneSymbol;
        public List<byte> TokenCodes { get; private set; } = new List<byte>();

        public LexicalAnalyzer()
        {
            _keywords = new Keywords();
            _symbol = 0;
            _tokenPosition = new TextPosition();
            _nmbInt = 0;
            _nmbFloat = 0;
            _oneSymbol = '\0';
        }

        public void PrintSymbolTable()
        {
            Console.WriteLine("ТАБЛИЦА КОДОВ СИМВОЛОВ");

            (string, byte)[] operators = new[]
            {
                ("+", plus), ("-", minus), ("*", star), ("/", slash), ("=", equal),
                (",", comma), (";", semicolon), (":", colon), (".", point), ("^", arrow),
                ("(", leftpar), (")", rightpar), ("[", lbracket), ("]", rbracket),
                ("{", flpar), ("}", frpar), ("<", later), (">", greater),
                ("<=", laterequal), (">=", greaterequal), ("<>", latergreater),
                (":=", assign), ("..", twopoints), ("(*", lcomment), ("*)", rcomment)
            };

            (string, byte)[] tokenTypes = new[]
            {
                ("Идентификатор", ident),
                ("Целая константа", intc),
                ("Вещественная константа", floatc),
                ("Символьная константа", charc)
            };

            Console.WriteLine("Операторы:");
            foreach (var (symbol, code) in operators)
            {
                Console.WriteLine($"{symbol,-4} -> {code}");
            }

            Console.WriteLine("\nТипы токенов:");
            foreach (var (name, code) in tokenTypes)
            {
                Console.WriteLine($"{name,-20} -> {code}");
            }

            Console.WriteLine("\nКлючевые слова:");
            foreach (var group in _keywords.Keyword)
            {
                foreach (var kw in group.Value)
                {
                    Console.WriteLine($"{kw.Key,-10} -> {kw.Value}");
                }
            }
            Console.WriteLine("---------------------------------\n");
        }

        public void Analyze()
        {
            InputOutput.NextCh();
            int programBlockDepth = 0;
            bool foundProgramEndKeyword = false;

            while (!InputOutput.IsEndOfFile())
            {
                byte code = NextSym();
                if (code != 0 && code != lcomment && code != rcomment)
                {
                    TokenCodes.Add(code);
                    Console.Write(code + " ");

                    if (code == beginsy)
                    {
                        programBlockDepth++;
                    }
                    else if (code == endsy)
                    {
                        programBlockDepth--;
                        if (programBlockDepth == 0)
                        {
                            foundProgramEndKeyword = true;
                        }
                    }
                    else if (foundProgramEndKeyword)
                    {
                        if (code == point)
                        {
                            foundProgramEndKeyword = false;
                        }
                        else
                        {
                            InputOutput.Error(2, _tokenPosition);
                            foundProgramEndKeyword = false;
                        }
                    }
                }
            }

            if (foundProgramEndKeyword)
            {
                InputOutput.Error(2, _tokenPosition);
            }
        }

        private byte NextSym()
        {
            while (char.IsWhiteSpace(InputOutput.Ch) || InputOutput.Ch == '\n'
                || InputOutput.Ch == '\r')
            {
                InputOutput.NextCh();
            }

            _tokenPosition = new TextPosition(InputOutput.LineNumber, InputOutput.CharNumber);

            if (InputOutput.IsEndOfFile())
            {
                return 0;
            }

            if (char.IsLetter(InputOutput.Ch))
            {
                return ScanIdentifierOrKeyword();
            }
            else if (char.IsDigit(InputOutput.Ch))
            {
                return ScanNumber();
            }
            else if (InputOutput.Ch == '\'')
            {
                return ScanCharacterConstant();
            }
            else
            {
                return ProcessSymbol();
            }
        }

        private byte ScanNumber()
        {
            const int MAX_INT = 32767; // Увеличиваем лимит для целых чисел
            _nmbInt = 0;
            bool overflow = false;
            string number = "";

            while (char.IsDigit(InputOutput.Ch))
            {
                number += InputOutput.Ch;
                int digit = InputOutput.Ch - '0';
                if (_nmbInt <= MAX_INT / 10 && (_nmbInt < MAX_INT / 10
                    || digit <= MAX_INT % 10))
                {
                    _nmbInt = 10 * _nmbInt + digit;
                }
                else
                {
                    overflow = true;
                }
                InputOutput.NextCh();
            }

            if (InputOutput.Ch == '.' && char.IsDigit((char)InputOutput.LookAhead()))
            {
                InputOutput.NextCh();
                return ScanFloatNumber(number);
            }
            else
            {
                _symbol = intc;
            }

            if (overflow)
            {
                InputOutput.Error(ERR_NUMBER_OVERFLOW, _tokenPosition);
                _nmbInt = 0;
            }

            return _symbol;
        }

        private byte ScanFloatNumber(string integerPart)
        {
            string fractionPart = "";

            while (char.IsDigit(InputOutput.Ch))
            {
                fractionPart += InputOutput.Ch;
                InputOutput.NextCh();
            }

            string fullNumber = integerPart + "." + fractionPart;

            try
            {
                _nmbFloat = float.Parse(fullNumber);
                // Проверяем на переполнение float
                if (float.IsInfinity(_nmbFloat) || float.IsNaN(_nmbFloat))
                {
                    InputOutput.Error(ERR_NUMBER_OVERFLOW, _tokenPosition);
                    _nmbFloat = 0;
                }
            }
            catch
            {
                InputOutput.Error(ERR_NUMBER_OVERFLOW, _tokenPosition);
                _nmbFloat = 0;
            }

            _symbol = floatc;
            return _symbol;
        }

        private byte ScanIdentifierOrKeyword()
        {
            _addrName = "";
            while (char.IsLetterOrDigit(InputOutput.Ch) || InputOutput.Ch == '_')
            {
                _addrName += InputOutput.Ch;
                InputOutput.NextCh();
            }

            byte length = (byte)_addrName.Length;
            if (_keywords.Keyword.TryGetValue(length, out var dict)
                && dict.TryGetValue(_addrName.ToLower(), out byte code))
            {
                _symbol = code;
            }
            else
            {
                _symbol = ident;
            }
            return _symbol;
        }

        private byte ScanCharacterConstant()
        {
            InputOutput.NextCh();
            if (InputOutput.Ch == '\'')
            {
                _oneSymbol = '\0';
                InputOutput.NextCh();
            }
            else if (InputOutput.IsEndOfFile() || InputOutput.Ch == '\n')
            {
                InputOutput.Error(ERR_UNCLOSED_STRING, _tokenPosition);
                _oneSymbol = '\0';
            }
            else
            {
                _oneSymbol = InputOutput.Ch;
                InputOutput.NextCh();
                if (InputOutput.Ch != '\'')
                {
                    InputOutput.Error(ERR_UNCLOSED_STRING, _tokenPosition);
                }
                else
                {
                    InputOutput.NextCh();
                }
            }
            _symbol = charc;
            return _symbol;
        }

        private byte ProcessSymbol()
        {
            switch (InputOutput.Ch)
            {
                case '+':
                    InputOutput.NextCh();
                    return plus;
                case '-':
                    InputOutput.NextCh();
                    return minus;
                case '*':
                    InputOutput.NextCh();
                    return star;
                case '/':
                    InputOutput.NextCh();
                    return slash;
                case '=':
                    InputOutput.NextCh();
                    return equal;
                case ',':
                    InputOutput.NextCh();
                    return comma;
                case ';':
                    InputOutput.NextCh();
                    return semicolon;
                case ':':
                    InputOutput.NextCh();
                    if (InputOutput.Ch == '=')
                    {
                        InputOutput.NextCh();
                        return assign;
                    }
                    return colon;
                case '.':
                    InputOutput.NextCh();
                    if (InputOutput.Ch == '.')
                    {
                        InputOutput.NextCh();
                        return twopoints;
                    }
                    return point;
                case '(':
                    InputOutput.NextCh();
                    if (InputOutput.Ch == '*')
                    {
                        InputOutput.NextCh();
                        SkipComment(true);
                        return lcomment;
                    }
                    return leftpar;
                case ')':
                    InputOutput.NextCh();
                    return rightpar;
                case '[':
                    InputOutput.NextCh();
                    return lbracket;
                case ']':
                    InputOutput.NextCh();
                    return rbracket;
                case '^':
                    InputOutput.NextCh();
                    return arrow;
                case '<':
                    InputOutput.NextCh();
                    if (InputOutput.Ch == '=')
                    {
                        InputOutput.NextCh();
                        return laterequal;
                    }
                    else if (InputOutput.Ch == '>')
                    {
                        InputOutput.NextCh();
                        return latergreater;
                    }
                    return later;
                case '>':
                    InputOutput.NextCh();
                    if (InputOutput.Ch == '=')
                    {
                        InputOutput.NextCh();
                        return greaterequal;
                    }
                    return greater;
                case '{':
                    InputOutput.NextCh();
                    SkipComment(false);
                    return lcomment;
                case '}':
                    InputOutput.NextCh();
                    return frpar;
                default:
                    InputOutput.Error(ERR_INVALID_CHAR, _tokenPosition);
                    InputOutput.NextCh();
                    return 0;
            }
        }

        private void SkipComment(bool isStarComment)
        {
            TextPosition commentStart = _tokenPosition;
            if (isStarComment)
            {
                while (!InputOutput.IsEndOfFile())
                {
                    if (InputOutput.Ch == '*' && InputOutput.LookAhead() == ')')
                    {
                        InputOutput.NextCh();
                        InputOutput.NextCh();
                        return;
                    }
                    InputOutput.NextCh();
                }
                InputOutput.Error(ERR_UNCLOSED_COMMENT, commentStart);
            }
            else
            {
                while (!InputOutput.IsEndOfFile())
                {
                    if (InputOutput.Ch == '}')
                    {
                        InputOutput.NextCh();
                        return;
                    }
                    InputOutput.NextCh();
                }
                InputOutput.Error(ERR_UNCLOSED_COMMENT, commentStart);
            }
        }
    }
}
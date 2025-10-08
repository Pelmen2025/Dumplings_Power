using System;
using System.Collections.Generic;

namespace PascalAnalyzer
{
    class LexicalAnalyzer
    {
        public const byte
            plus = 70, minus = 71, star = 21, slash = 60, equal = 16,
            comma = 20, semicolon = 14, colon = 5, assign = 51, ident = 2,
            intc = 15, floatc = 82, charc = 83, varsy = 105, beginsy = 113,
            endsy = 104, forsy = 109, whilesy = 114, dosy = 54, tosy = 103,
            downtosy = 118, integersy = 125, realsy = 126, charsy = 127,
            andsy = 107, orsy = 102, greater = 66, lcomment = 72, rcomment = 73;

        private const byte ERR_INVALID_CHAR = 1;
        private const byte ERR_UNCLOSED_STRING = 7;
        private const byte ERR_NUMBER_OVERFLOW = 203;
        private const byte ERR_UNCLOSED_COMMENT = 12;

        private readonly Keywords _keywords = new();
        private TextPosition _tokenPosition;
        private byte _symbol;
        private string _identifier = "";
        private int _intValue;
        private float _floatValue;
        private char _charValue;

        public List<byte> TokenCodes { get; } = new();
        public List<TextPosition> TokenPositions { get; } = new();

        public void Analyze()
        {
            InputOutput.NextCh();
            while (!InputOutput.IsEndOfFile())
            {
                byte code = NextSymbol();
                if (code != 0 && code != lcomment && code != rcomment)
                {
                    TokenCodes.Add(code);
                    TokenPositions.Add(_tokenPosition);
                    Console.Write(code + " ");
                }
            }
        }

        private byte NextSymbol()
        {
            while (char.IsWhiteSpace(InputOutput.Ch) || InputOutput.Ch == '\n' || InputOutput.Ch == '\r')
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

            if (char.IsDigit(InputOutput.Ch))
            {
                return ScanNumber();
            }

            if (InputOutput.Ch == '\'')
            {
                return ScanCharacterConstant();
            }

            return ProcessSymbol();
        }

        private byte ScanIdentifierOrKeyword()
        {
            _identifier = "";
            while (char.IsLetterOrDigit(InputOutput.Ch) || InputOutput.Ch == '_')
            {
                _identifier += InputOutput.Ch;
                InputOutput.NextCh();
            }

            if (_keywords.TryGetKeywordCode(_identifier, out byte code))
            {
                _symbol = code;
            }
            else
            {
                _symbol = ident;
            }
            return _symbol;
        }

        private byte ScanNumber()
        {
            const int MAX_INT = 32767;
            _intValue = 0;
            string number = "";

            while (char.IsDigit(InputOutput.Ch))
            {
                number += InputOutput.Ch;
                int digit = InputOutput.Ch - '0';
                if (_intValue <= MAX_INT / 10 && (_intValue < MAX_INT / 10 || digit <= MAX_INT % 10))
                {
                    _intValue = 10 * _intValue + digit;
                }
                else
                {
                    InputOutput.AddError(ERR_NUMBER_OVERFLOW, _tokenPosition);
                    _intValue = 0;
                }
                InputOutput.NextCh();
            }

            if (InputOutput.Ch == '.' && char.IsDigit(InputOutput.LookAhead()))
            {
                InputOutput.NextCh();
                return ScanFloatNumber(number);
            }

            _symbol = intc;
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
                _floatValue = float.Parse(fullNumber);
                if (float.IsInfinity(_floatValue) || float.IsNaN(_floatValue))
                {
                    InputOutput.AddError(ERR_NUMBER_OVERFLOW, _tokenPosition);
                    _floatValue = 0;
                }
            }
            catch
            {
                InputOutput.AddError(ERR_NUMBER_OVERFLOW, _tokenPosition);
                _floatValue = 0;
            }

            _symbol = floatc;
            return _symbol;
        }

        private byte ScanCharacterConstant()
        {
            InputOutput.NextCh();
            if (InputOutput.Ch == '\'')
            {
                _charValue = '\0';
                InputOutput.NextCh();
            }
            else if (InputOutput.IsEndOfFile() || InputOutput.Ch == '\n')
            {
                InputOutput.AddError(ERR_UNCLOSED_STRING, _tokenPosition);
                _charValue = '\0';
            }
            else
            {
                _charValue = InputOutput.Ch;
                InputOutput.NextCh();
                if (InputOutput.Ch != '\'')
                {
                    InputOutput.AddError(ERR_UNCLOSED_STRING, _tokenPosition);
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
            char ch = InputOutput.Ch;
            InputOutput.NextCh();

            switch (ch)
            {
                case '+': return plus;
                case '-': return minus;
                case '*': return star;
                case '/': return slash;
                case '=': return equal;
                case ',': return comma;
                case ';': return semicolon;
                case ':':
                    if (InputOutput.Ch == '=')
                    {
                        InputOutput.NextCh();
                        return assign;
                    }
                    return colon;
                case '>':
                    return greater;
                case '{':
                    SkipComment();
                    return lcomment;
                case '}':
                    return rcomment;
                default:
                    InputOutput.AddError(ERR_INVALID_CHAR, _tokenPosition);
                    return 0;
            }
        }

        private void SkipComment()
        {
            TextPosition commentStart = _tokenPosition;
            while (!InputOutput.IsEndOfFile())
            {
                if (InputOutput.Ch == '}')
                {
                    InputOutput.NextCh();
                    return;
                }
                InputOutput.NextCh();
            }
            InputOutput.AddError(ERR_UNCLOSED_COMMENT, commentStart);
        }
    }
}
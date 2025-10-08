using System;
using System.Collections.Generic;

namespace PascalAnalyzer
{
    class SyntaxAnalyzer
    {
        private const byte ERR_EXPECTED_VAR = 10;
        private const byte ERR_EXPECTED_IDENTIFIER = 11;
        private const byte ERR_EXPECTED_COLON = 25;
        private const byte ERR_EXPECTED_TYPE = 13;
        private const byte ERR_EXPECTED_SEMICOLON = 14;
        private const byte ERR_EXPECTED_ASSIGN = 15;
        private const byte ERR_EXPECTED_EXPRESSION = 16;
        private const byte ERR_EXPECTED_BEGIN = 17;
        private const byte ERR_EXPECTED_END = 18;
        private const byte ERR_EXPECTED_DO = 19;
        private const byte ERR_EXPECTED_TO_OR_DOWNTO = 20;

        private readonly LexicalAnalyzer _lexer;
        private byte _currentSymbol;
        private TextPosition _currentPosition;
        private int _symbolIndex;
        private readonly IReadOnlyList<byte> _tokens;
        private readonly IReadOnlyList<TextPosition> _positions;

        public SyntaxAnalyzer(LexicalAnalyzer lexer)
        {
            _lexer = lexer;
            _tokens = lexer.TokenCodes.AsReadOnly();
            _positions = lexer.TokenPositions.AsReadOnly();
            _symbolIndex = 0;
            _currentSymbol = 0;
            NextSymbol();
        }

        public void Analyze()
        {
            Console.WriteLine("\n--- СИНТАКСИЧЕСКИЙ АНАЛИЗ ---");
            ParseProgram();
            Console.WriteLine("\nСинтаксический анализ завершен.");
        }

        private void NextSymbol()
        {
            if (_symbolIndex < _tokens.Count)
            {
                _currentSymbol = _tokens[_symbolIndex];
                _currentPosition = _positions[_symbolIndex];
                _symbolIndex++;
            }
            else
            {
                _currentSymbol = 0;
                _currentPosition = new TextPosition(InputOutput.LineNumber, InputOutput.CharNumber);
            }
        }

        private void ParseProgram()
        {
            if (_currentSymbol == LexicalAnalyzer.varsy)
            {
                ParseVariableDeclarations();
            }

            if (_currentSymbol != LexicalAnalyzer.beginsy && _currentSymbol != 0)
            {
                ReportError(ERR_EXPECTED_BEGIN, "Ожидалось ключевое слово 'begin'");
                NextSymbol();
                return;
            }

            ParseCompoundStatement();
        }

        private void ParseVariableDeclarations()
        {
            Console.WriteLine("Анализ секции переменных...");
            if (_currentSymbol != LexicalAnalyzer.varsy)
            {
                ReportError(ERR_EXPECTED_VAR, "Ожидалось ключевое слово 'var'");
                return;
            }

            NextSymbol();
            while (_currentSymbol == LexicalAnalyzer.ident && _currentSymbol != 0)
            {
                ParseVariableDeclaration();
                if (_currentSymbol == LexicalAnalyzer.semicolon)
                {
                    NextSymbol();
                }
                else if (_currentSymbol != 0)
                {
                    ReportError(ERR_EXPECTED_SEMICOLON, "Ожидалась точка с запятой");
                    return; // Завершаем разбор переменных, чтобы избежать каскадных ошибок
                }
            }
        }

        private void ParseVariableDeclaration()
        {
            Console.WriteLine("  Анализ объявления переменной...");
            do
            {
                if (_currentSymbol != LexicalAnalyzer.ident)
                {
                    ReportError(ERR_EXPECTED_IDENTIFIER, "Ожидался идентификатор");
                    return;
                }
                NextSymbol();
                if (_currentSymbol == LexicalAnalyzer.comma)
                {
                    NextSymbol();
                }
                else
                {
                    break;
                }
            } while (_currentSymbol == LexicalAnalyzer.ident && _currentSymbol != 0);

            if (_currentSymbol != LexicalAnalyzer.colon)
            {
                ReportError(ERR_EXPECTED_COLON, "Ожидалось двоеточие");
                return;
            }
            NextSymbol();

            if (!IsSimpleType(_currentSymbol))
            {
                ReportError(ERR_EXPECTED_TYPE, "Ожидался тип (integer, real, char)");
                return;
            }
            Console.WriteLine($"    Найден тип: {GetTypeName(_currentSymbol)}");
            NextSymbol();
        }

        private void ParseCompoundStatement()
        {
            Console.WriteLine("Анализ составного оператора...");
            if (_currentSymbol != LexicalAnalyzer.beginsy)
            {
                ReportError(ERR_EXPECTED_BEGIN, "Ожидалось ключевое слово 'begin'");
                return;
            }

            NextSymbol();
            while (_currentSymbol != LexicalAnalyzer.endsy && _currentSymbol != 0)
            {
                ParseStatement();
                if (_currentSymbol == LexicalAnalyzer.semicolon)
                {
                    NextSymbol();
                }
                else if (_currentSymbol != LexicalAnalyzer.endsy && _currentSymbol != 0)
                {
                    ReportError(ERR_EXPECTED_SEMICOLON, "Ожидалась точка с запятой");
                    return; // Завершаем разбор операторов, чтобы избежать каскадных ошибок
                }
            }

            if (_currentSymbol != LexicalAnalyzer.endsy)
            {
                ReportError(ERR_EXPECTED_END, "Ожидалось ключевое слово 'end'");
                return;
            }
            NextSymbol();
        }

        private void ParseStatement()
        {
            Console.WriteLine("  Анализ оператора...");
            if (_currentSymbol == LexicalAnalyzer.ident)
            {
                ParseAssignmentStatement();
            }
            else if (_currentSymbol == LexicalAnalyzer.forsy)
            {
                ParseForStatement();
            }
            else if (_currentSymbol == LexicalAnalyzer.whilesy)
            {
                ParseWhileStatement();
            }
            else
            {
                ReportError(ERR_EXPECTED_IDENTIFIER, "Ожидался оператор (присваивание, for, while)");
                if (_currentSymbol != 0)
                {
                    NextSymbol();
                }
            }
        }

        private void ParseAssignmentStatement()
        {
            Console.WriteLine("    Анализ оператора присваивания...");
            if (_currentSymbol != LexicalAnalyzer.ident)
            {
                ReportError(ERR_EXPECTED_IDENTIFIER, "Ожидался идентификатор");
                return;
            }

            NextSymbol();
            if (_currentSymbol != LexicalAnalyzer.assign)
            {
                ReportError(ERR_EXPECTED_ASSIGN, "Ожидался оператор ':='");
                return;
            }
            NextSymbol();

            ParseExpression();
        }

        private void ParseForStatement()
        {
            Console.WriteLine("    Анализ цикла for...");
            NextSymbol();
            if (_currentSymbol != LexicalAnalyzer.ident)
            {
                ReportError(ERR_EXPECTED_IDENTIFIER, "Ожидался идентификатор");
                return;
            }
            NextSymbol();

            if (_currentSymbol != LexicalAnalyzer.assign)
            {
                ReportError(ERR_EXPECTED_ASSIGN, "Ожидался оператор ':='");
                return;
            }
            NextSymbol();

            ParseExpression();
            if (_currentSymbol != LexicalAnalyzer.tosy && _currentSymbol != LexicalAnalyzer.downtosy)
            {
                ReportError(ERR_EXPECTED_TO_OR_DOWNTO, "Ожидалось 'to' или 'downto'");
                return;
            }
            NextSymbol();

            ParseExpression();
            if (_currentSymbol != LexicalAnalyzer.dosy)
            {
                ReportError(ERR_EXPECTED_DO, "Ожидалось ключевое слово 'do'");
                return;
            }
            NextSymbol();

            ParseStatement();
        }

        private void ParseWhileStatement()
        {
            Console.WriteLine("    Анализ цикла while...");
            NextSymbol();
            ParseExpression();
            if (_currentSymbol != LexicalAnalyzer.dosy)
            {
                ReportError(ERR_EXPECTED_DO, "Ожидалось ключевое слово 'do'");
                return;
            }
            NextSymbol();

            ParseStatement();
        }

        private void ParseExpression()
        {
            Console.WriteLine("      Анализ выражения...");
            ParseSimpleExpression();
            if (IsRelationalOperator(_currentSymbol))
            {
                NextSymbol();
                ParseSimpleExpression();
            }
        }

        private void ParseSimpleExpression()
        {
            if (_currentSymbol == LexicalAnalyzer.plus || _currentSymbol == LexicalAnalyzer.minus)
            {
                NextSymbol();
            }

            ParseTerm();
            while (_currentSymbol == LexicalAnalyzer.plus ||
                   _currentSymbol == LexicalAnalyzer.minus ||
                   _currentSymbol == LexicalAnalyzer.orsy)
            {
                NextSymbol();
                ParseTerm();
            }
        }

        private void ParseTerm()
        {
            ParseFactor();
            while (_currentSymbol == LexicalAnalyzer.star ||
                   _currentSymbol == LexicalAnalyzer.slash ||
                   _currentSymbol == LexicalAnalyzer.andsy)
            {
                NextSymbol();
                ParseFactor();
            }
        }

        private void ParseFactor()
        {
            if (_currentSymbol == LexicalAnalyzer.ident ||
                _currentSymbol == LexicalAnalyzer.intc ||
                _currentSymbol == LexicalAnalyzer.floatc ||
                _currentSymbol == LexicalAnalyzer.charc)
            {
                NextSymbol();
            }
            else
            {
                ReportError(ERR_EXPECTED_EXPRESSION, "Ожидалось выражение");
                if (_currentSymbol != 0)
                {
                    NextSymbol();
                }
            }
        }

        private bool IsSimpleType(byte symbol)
        {
            return symbol == LexicalAnalyzer.integersy ||
                   symbol == LexicalAnalyzer.realsy ||
                   symbol == LexicalAnalyzer.charsy;
        }

        private bool IsRelationalOperator(byte symbol)
        {
            return symbol == LexicalAnalyzer.equal ||
                   symbol == LexicalAnalyzer.greater;
        }

        private string GetTypeName(byte symbol)
        {
            return symbol switch
            {
                LexicalAnalyzer.integersy => "integer",
                LexicalAnalyzer.realsy => "real",
                LexicalAnalyzer.charsy => "char",
                _ => "неизвестный тип"
            };
        }

        private void ReportError(byte errorCode, string message)
        {
            Console.WriteLine($"СИНТАКСИЧЕСКАЯ ОШИБКА: {message} в позиции {_currentPosition.LineNumber}:{_currentPosition.CharNumber}");
            InputOutput.AddError(errorCode, _currentPosition);
        }
    }
}
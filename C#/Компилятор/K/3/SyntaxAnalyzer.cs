using System;
using System.Collections.Generic;

namespace PascalLexer
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
        private const byte ERR_EXPECTED_WHILE = 21;
        private const byte ERR_EXPECTED_LEFTPAR = 22;
        private const byte ERR_EXPECTED_RIGHTPAR = 23;
        private const byte ERR_UNEXPECTED_SYMBOL = 24;

        private readonly LexicalAnalyzer lexer;
        private byte currentSymbol;
        private TextPosition currentPosition;
        private int symbolIndex;
        private readonly List<byte> tokens;

        public SyntaxAnalyzer(LexicalAnalyzer lexicalAnalyzer)
        {
            lexer = lexicalAnalyzer;
            tokens = lexicalAnalyzer.TokenCodes;
            symbolIndex = 0;
            currentSymbol = 0;
        }

        public void Analyze()
        {
            Console.WriteLine("\n--- СИНТАКСИЧЕСКИЙ АНАЛИЗ ---");
            NextSymbol();

            // Анализируем блок переменных (если есть)
            if (currentSymbol == LexicalAnalyzer.varsy)
            {
                ParseVariableDeclarations();
            }

            // Анализируем тело программы
            if (currentSymbol == LexicalAnalyzer.beginsy)
            {
                ParseCompoundStatement();
            }

            Console.WriteLine("\nСинтаксический анализ завершен.");
        }

        private void NextSymbol()
        {
            if (symbolIndex < tokens.Count)
            {
                currentSymbol = tokens[symbolIndex];
                currentPosition = new TextPosition(1, symbolIndex + 1); // Упрощенная позиция
                symbolIndex++;
            }
            else
            {
                currentSymbol = 0; // EOF
            }
        }

        private void ParseVariableDeclarations()
        {
            Console.WriteLine("Анализ секции переменных...");

            if (currentSymbol != LexicalAnalyzer.varsy)
            {
                ReportError(ERR_EXPECTED_VAR, "Ожидалось ключевое слово 'var'");
                return;
            }

            NextSymbol(); // пропускаем 'var'

            do
            {
                ParseVariableDeclaration();

                if (currentSymbol == LexicalAnalyzer.semicolon)
                {
                    NextSymbol();
                }
                else if (IsVariableStart())
                {
                    ReportError(ERR_EXPECTED_SEMICOLON, "Ожидалась точка с запятой после объявления переменной");
                    // Нейтрализация: продолжаем анализ
                }

            } while (IsVariableStart());
        }

        private void ParseVariableDeclaration()
        {
            Console.WriteLine("  Анализ объявления переменной...");

            // Список идентификаторов
            do
            {
                if (currentSymbol != LexicalAnalyzer.ident)
                {
                    ReportError(ERR_EXPECTED_IDENTIFIER, "Ожидался идентификатор переменной");
                    SkipToNextValidSymbol(new[] { LexicalAnalyzer.colon, LexicalAnalyzer.semicolon });
                    break;
                }

                NextSymbol(); // пропускаем идентификатор

                if (currentSymbol == LexicalAnalyzer.comma)
                {
                    NextSymbol();
                }
                else
                {
                    break;
                }
            } while (currentSymbol == LexicalAnalyzer.ident);

            // Двоеточие
            if (currentSymbol != LexicalAnalyzer.colon)
            {
                ReportError(ERR_EXPECTED_COLON, "Ожидалось двоеточие после списка переменных");
                SkipToNextValidSymbol(new[] { LexicalAnalyzer.integersy, LexicalAnalyzer.realsy,
                                     LexicalAnalyzer.charsy });
            }
            else
            {
                NextSymbol();
            }

            // Тип
            if (!IsSimpleType(currentSymbol))
            {
                ReportError(ERR_EXPECTED_TYPE, "Ожидался простой тип (integer, real, char)");
                SkipToNextValidSymbol(new[] { LexicalAnalyzer.semicolon });
            }
            else
            {
                Console.WriteLine($"    Найден тип: {GetTypeName(currentSymbol)}");
                NextSymbol();
            }
        }

        private void ParseCompoundStatement()
        {
            Console.WriteLine("Анализ составного оператора...");

            if (currentSymbol != LexicalAnalyzer.beginsy)
            {
                ReportError(ERR_EXPECTED_BEGIN, "Ожидалось ключевое слово 'begin'");
                return;
            }

            NextSymbol(); // пропускаем 'begin'

            do
            {
                ParseStatement();

                if (currentSymbol == LexicalAnalyzer.semicolon)
                {
                    NextSymbol();
                }
                else if (IsStatementStart())
                {
                    ReportError(ERR_EXPECTED_SEMICOLON, "Ожидалась точка с запятой между операторами");
                    // Нейтрализация: продолжаем анализ
                }

            } while (IsStatementStart());

            if (currentSymbol != LexicalAnalyzer.endsy)
            {
                ReportError(ERR_EXPECTED_END, "Ожидалось ключевое слово 'end'");
            }
            else
            {
                NextSymbol();
            }
        }

        private void ParseStatement()
        {
            Console.WriteLine("  Анализ оператора...");

            if (currentSymbol == LexicalAnalyzer.ident)
            {
                ParseAssignmentStatement();
            }
            else if (currentSymbol == LexicalAnalyzer.beginsy)
            {
                ParseCompoundStatement();
            }
            else if (currentSymbol == LexicalAnalyzer.forsy)
            {
                ParseForStatement();
            }
            else if (currentSymbol == LexicalAnalyzer.whilesy)
            {
                ParseWhileStatement();
            }
            else if (currentSymbol == LexicalAnalyzer.semicolon ||
                       currentSymbol == LexicalAnalyzer.endsy)
            {
                // Пустой оператор - допустимо
                Console.WriteLine("    Пустой оператор");
            }
            else
            {
                ReportError(ERR_UNEXPECTED_SYMBOL, "Неожиданный символ в операторе");
                NextSymbol(); // Нейтрализация: пропускаем неизвестный символ
            }
        }

        private void ParseAssignmentStatement()
        {
            Console.WriteLine("    Анализ оператора присваивания...");

            if (currentSymbol != LexicalAnalyzer.ident)
            {
                ReportError(ERR_EXPECTED_IDENTIFIER, "Ожидался идентификатор переменной");
                return;
            }

            NextSymbol(); // пропускаем идентификатор

            if (currentSymbol != LexicalAnalyzer.assign)
            {
                ReportError(ERR_EXPECTED_ASSIGN, "Ожидался оператор присваивания ':='");
                SkipToExpression();
            }
            else
            {
                NextSymbol();
            }

            ParseExpression();
        }

        private void ParseForStatement()
        {
            Console.WriteLine("    Анализ цикла for...");

            NextSymbol(); // пропускаем 'for'

            // Переменная цикла
            if (currentSymbol != LexicalAnalyzer.ident)
            {
                ReportError(ERR_EXPECTED_IDENTIFIER, "Ожидался идентификатор переменной цикла");
                SkipToNextValidSymbol(new[] { LexicalAnalyzer.assign });
            }
            else
            {
                NextSymbol();
            }

            // Присваивание
            if (currentSymbol != LexicalAnalyzer.assign)
            {
                ReportError(ERR_EXPECTED_ASSIGN, "Ожидался оператор ':=' в цикле for");
                SkipToExpression();
            }
            else
            {
                NextSymbol();
            }

            ParseExpression(); // Начальное значение

            // TO или DOWNTO
            if (currentSymbol != LexicalAnalyzer.tosy && currentSymbol != LexicalAnalyzer.downtosy)
            {
                ReportError(ERR_EXPECTED_TO_OR_DOWNTO, "Ожидалось 'to' или 'downto'");
                SkipToExpression();
            }
            else
            {
                NextSymbol();
            }

            ParseExpression(); // Конечное значение

            // DO
            if (currentSymbol != LexicalAnalyzer.dosy)
            {
                ReportError(ERR_EXPECTED_DO, "Ожидалось ключевое слово 'do'");
                SkipToStatement();
            }
            else
            {
                NextSymbol();
            }

            ParseStatement(); // Тело цикла
        }

        private void ParseWhileStatement()
        {
            Console.WriteLine("    Анализ цикла while...");

            NextSymbol(); // пропускаем 'while'

            ParseExpression(); // Условие

            if (currentSymbol != LexicalAnalyzer.dosy)
            {
                ReportError(ERR_EXPECTED_DO, "Ожидалось ключевое слово 'do' в цикле while");
                SkipToStatement();
            }
            else
            {
                NextSymbol();
            }

            ParseStatement(); // Тело цикла
        }

        private void ParseExpression()
        {
            Console.WriteLine("      Анализ выражения...");

            if (!IsExpressionStart())
            {
                ReportError(ERR_EXPECTED_EXPRESSION, "Ожидалось выражение");
                return;
            }

            ParseSimpleExpression();

            // Операторы сравнения
            if (IsRelationalOperator(currentSymbol))
            {
                NextSymbol();
                ParseSimpleExpression();
            }
        }

        private void ParseSimpleExpression()
        {
            // Унарный знак
            if (currentSymbol == LexicalAnalyzer.plus || currentSymbol == LexicalAnalyzer.minus)
            {
                NextSymbol();
            }

            ParseTerm();

            // Аддитивные операторы
            while (currentSymbol == LexicalAnalyzer.plus ||
                   currentSymbol == LexicalAnalyzer.minus ||
                   currentSymbol == LexicalAnalyzer.orsy)
            {
                NextSymbol();
                ParseTerm();
            }
        }

        private void ParseTerm()
        {
            ParseFactor();

            // Мультипликативные операторы
            while (currentSymbol == LexicalAnalyzer.star ||
                   currentSymbol == LexicalAnalyzer.slash ||
                   currentSymbol == LexicalAnalyzer.divsy ||
                   currentSymbol == LexicalAnalyzer.modsy ||
                   currentSymbol == LexicalAnalyzer.andsy)
            {
                NextSymbol();
                ParseFactor();
            }
        }

        private void ParseFactor()
        {
            if (currentSymbol == LexicalAnalyzer.ident)
            {
                NextSymbol();
            }
            else if (currentSymbol == LexicalAnalyzer.intc)
            {
                NextSymbol();
            }
            else if (currentSymbol == LexicalAnalyzer.floatc)
            {
                NextSymbol();
            }
            else if (currentSymbol == LexicalAnalyzer.charc)
            {
                NextSymbol();
            }
            else if (currentSymbol == LexicalAnalyzer.leftpar)
            {
                NextSymbol();
                ParseExpression();
                if (currentSymbol != LexicalAnalyzer.rightpar)
                {
                    ReportError(ERR_EXPECTED_RIGHTPAR, "Ожидалась закрывающая скобка");
                }
                else
                {
                    NextSymbol();
                }
            }
            else if (currentSymbol == LexicalAnalyzer.notsy)
            {
                NextSymbol();
                ParseFactor();
            }
            else
            {
                ReportError(ERR_EXPECTED_EXPRESSION, "Неожиданный символ в выражении");
                NextSymbol(); // Нейтрализация
            }
        }

        // Вспомогательные методы для проверки типов символов
        private bool IsVariableStart()
        {
            return currentSymbol == LexicalAnalyzer.ident;
        }

        private bool IsStatementStart()
        {
            return currentSymbol == LexicalAnalyzer.ident ||
                   currentSymbol == LexicalAnalyzer.beginsy ||
                   currentSymbol == LexicalAnalyzer.forsy ||
                   currentSymbol == LexicalAnalyzer.whilesy;
        }

        private bool IsExpressionStart()
        {
            return currentSymbol == LexicalAnalyzer.ident ||
                   currentSymbol == LexicalAnalyzer.intc ||
                   currentSymbol == LexicalAnalyzer.floatc ||
                   currentSymbol == LexicalAnalyzer.charc ||
                   currentSymbol == LexicalAnalyzer.leftpar ||
                   currentSymbol == LexicalAnalyzer.plus ||
                   currentSymbol == LexicalAnalyzer.minus ||
                   currentSymbol == LexicalAnalyzer.notsy;
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
                   symbol == LexicalAnalyzer.latergreater ||
                   symbol == LexicalAnalyzer.later ||
                   symbol == LexicalAnalyzer.greater ||
                   symbol == LexicalAnalyzer.laterequal ||
                   symbol == LexicalAnalyzer.greaterequal;
        }

        private string GetTypeName(byte typeSymbol)
        {
            return typeSymbol switch
            {
                LexicalAnalyzer.integersy => "integer",
                LexicalAnalyzer.realsy => "real",
                LexicalAnalyzer.charsy => "char",
                _ => "неизвестный тип"
            };
        }

        // Методы для нейтрализации ошибок
        private void SkipToNextValidSymbol(byte[] validSymbols)
        {
            while (currentSymbol != 0 && Array.IndexOf(validSymbols, currentSymbol) == -1)
            {
                NextSymbol();
            }
        }

        private void SkipToExpression()
        {
            while (currentSymbol != 0 && !IsExpressionStart())
            {
                NextSymbol();
            }
        }

        private void SkipToStatement()
        {
            while (currentSymbol != 0 && !IsStatementStart())
            {
                NextSymbol();
            }
        }

        private void ReportError(byte errorCode, string message)
        {
            Console.WriteLine($"СИНТАКСИЧЕСКАЯ ОШИБКА: {message}");
            InputOutput.Error(errorCode, currentPosition);
        }
    }
}
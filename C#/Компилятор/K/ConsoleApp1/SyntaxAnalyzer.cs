using System;
using System.Collections.Generic;

namespace PascalLexer
{
    class SyntaxAnalyzer
    {
        private readonly LexicalAnalyzer lexer_;
        private readonly List<byte> tokens_;
        private int position_;
        private byte current_token_;
        private readonly List<SyntaxError> errors_;

        public SyntaxAnalyzer(LexicalAnalyzer lexer)
        {
            lexer_ = lexer;
            tokens_ = lexer.TokenCodes;
            position_ = 0;
            current_token_ = 0;
            errors_ = new List<SyntaxError>();
        }

        public void Analyze()
        {
            Console.WriteLine("\n=== СИНТАКСИЧЕСКИЙ АНАЛИЗ ===\n");

            if (tokens_.Count == 0)
            {
                Console.WriteLine("Нет токенов для анализа");
                return;
            }

            NextToken();
            ParseProgram();

            PrintErrors();
        }

        private void NextToken()
        {
            if (position_ < tokens_.Count)
            {
                current_token_ = tokens_[position_];
                position_++;
            }
            else
            {
                current_token_ = 0;
            }
        }

        private void Error(string message)
        {
            errors_.Add(new SyntaxError(position_, message));
            Console.WriteLine($"[Ошибка на позиции {position_}]: {message}");
        }

        private bool Accept(byte expected)
        {
            if (current_token_ == expected)
            {
                NextToken();
                return true;
            }
            return false;
        }

        private void Expect(byte expected, string message)
        {
            if (!Accept(expected))
            {
                Error(message);
            }
        }

        private void ParseProgram()
        {
            // program -> PROGRAM ident ; block .
            if (Accept(LexicalAnalyzer.programsy))
            {
                Expect(LexicalAnalyzer.ident, "Ожидается имя программы");
                Expect(LexicalAnalyzer.semicolon, "Ожидается ';' после имени программы");
            }

            ParseBlock();
            Expect(LexicalAnalyzer.point, "Ожидается '.' в конце программы");
        }

        private void ParseBlock()
        {
            // block -> [var-section] compound-statement
            if (current_token_ == LexicalAnalyzer.varsy)
            {
                ParseVarSection();
            }

            ParseCompoundStatement();
        }

        private void ParseVarSection()
        {
            // var-section -> VAR var-declaration { ; var-declaration }
            Accept(LexicalAnalyzer.varsy);

            ParseVarDeclaration();

            while (Accept(LexicalAnalyzer.semicolon))
            {
                if (current_token_ == LexicalAnalyzer.ident)
                {
                    ParseVarDeclaration();
                }
                else
                {
                    break;
                }
            }
        }

        private void ParseVarDeclaration()
        {
            // var-declaration -> ident { , ident } : type
            if (!Accept(LexicalAnalyzer.ident))
            {
                Error("Ожидается имя переменной");
                return;
            }

            while (Accept(LexicalAnalyzer.comma))
            {
                Expect(LexicalAnalyzer.ident, "Ожидается имя переменной после ','");
            }

            Expect(LexicalAnalyzer.colon, "Ожидается ':' после списка переменных");
            ParseType();
        }

        private void ParseType()
        {
            // type -> INTEGER | REAL | CHAR
            if (current_token_ == LexicalAnalyzer.integersy ||
                current_token_ == LexicalAnalyzer.realsy ||
                current_token_ == LexicalAnalyzer.charsy)
            {
                NextToken();
            }
            else
            {
                Error("Ожидается тип переменной (integer, real, char)");
            }
        }

        private void ParseCompoundStatement()
        {
            // compound-statement -> BEGIN statement { ; statement } END
            Expect(LexicalAnalyzer.beginsy, "Ожидается 'begin'");

            ParseStatement();

            while (Accept(LexicalAnalyzer.semicolon))
            {
                if (current_token_ != LexicalAnalyzer.endsy)
                {
                    ParseStatement();
                }
            }

            Expect(LexicalAnalyzer.endsy, "Ожидается 'end'");
        }

        private void ParseStatement()
        {
            // statement -> assignment | compound-statement | while-statement | 
            //              for-statement | repeat-statement

            if (current_token_ == LexicalAnalyzer.ident)
            {
                ParseAssignment();
            }
            else if (current_token_ == LexicalAnalyzer.beginsy)
            {
                ParseCompoundStatement();
            }
            else if (current_token_ == LexicalAnalyzer.whilesy)
            {
                ParseWhileStatement();
            }
            else if (current_token_ == LexicalAnalyzer.forsy)
            {
                ParseForStatement();
            }
            else if (current_token_ == LexicalAnalyzer.repeatsy)
            {
                ParseRepeatStatement();
            }
            else if (current_token_ == LexicalAnalyzer.ifsy)
            {
                ParseIfStatement();
            }
            else
            {
                // Пустой оператор допустим
            }
        }

        private void ParseAssignment()
        {
            // assignment -> ident := expression
            Expect(LexicalAnalyzer.ident, "Ожидается идентификатор");
            Expect(LexicalAnalyzer.assign, "Ожидается ':='");
            ParseExpression();
        }

        private void ParseExpression()
        {
            // expression -> simple-expression [ relop simple-expression ]
            ParseSimpleExpression();

            if (IsRelationalOperator(current_token_))
            {
                NextToken();
                ParseSimpleExpression();
            }
        }

        private bool IsRelationalOperator(byte token)
        {
            return token == LexicalAnalyzer.equal ||
                   token == LexicalAnalyzer.latergreater ||
                   token == LexicalAnalyzer.later ||
                   token == LexicalAnalyzer.greater ||
                   token == LexicalAnalyzer.laterequal ||
                   token == LexicalAnalyzer.greaterequal;
        }

        private void ParseSimpleExpression()
        {
            // simple-expression -> [+ | -] term { addop term }
            if (current_token_ == LexicalAnalyzer.plus ||
                current_token_ == LexicalAnalyzer.minus)
            {
                NextToken();
            }

            ParseTerm();

            while (IsAddOperator(current_token_))
            {
                NextToken();
                ParseTerm();
            }
        }

        private bool IsAddOperator(byte token)
        {
            return token == LexicalAnalyzer.plus ||
                   token == LexicalAnalyzer.minus ||
                   token == LexicalAnalyzer.orsy;
        }

        private void ParseTerm()
        {
            // term -> factor { mulop factor }
            ParseFactor();

            while (IsMultOperator(current_token_))
            {
                NextToken();
                ParseFactor();
            }
        }

        private bool IsMultOperator(byte token)
        {
            return token == LexicalAnalyzer.star ||
                   token == LexicalAnalyzer.slash ||
                   token == LexicalAnalyzer.divsy ||
                   token == LexicalAnalyzer.modsy ||
                   token == LexicalAnalyzer.andsy;
        }

        private void ParseFactor()
        {
            // factor -> ident | number | ( expression ) | NOT factor
            if (current_token_ == LexicalAnalyzer.ident)
            {
                NextToken();
            }
            else if (current_token_ == LexicalAnalyzer.intc ||
                       current_token_ == LexicalAnalyzer.floatc)
            {
                NextToken();
            }
            else if (current_token_ == LexicalAnalyzer.charc)
            {
                NextToken();
            }
            else if (Accept(LexicalAnalyzer.leftpar))
            {
                ParseExpression();
                Expect(LexicalAnalyzer.rightpar, "Ожидается ')'");
            }
            else if (Accept(LexicalAnalyzer.notsy))
            {
                ParseFactor();
            }
            else
            {
                Error("Ожидается идентификатор, число или '('");
            }
        }

        private void ParseWhileStatement()
        {
            // while-statement -> WHILE expression DO statement
            Accept(LexicalAnalyzer.whilesy);
            ParseExpression();
            Expect(LexicalAnalyzer.dosy, "Ожидается 'do' после условия while");
            ParseStatement();
        }

        private void ParseForStatement()
        {
            // for-statement -> FOR ident := expression TO|DOWNTO expression DO statement
            Accept(LexicalAnalyzer.forsy);
            Expect(LexicalAnalyzer.ident, "Ожидается переменная цикла");
            Expect(LexicalAnalyzer.assign, "Ожидается ':='");
            ParseExpression();

            if (!Accept(LexicalAnalyzer.tosy) && !Accept(LexicalAnalyzer.downtosy))
            {
                Error("Ожидается 'to' или 'downto'");
            }

            ParseExpression();
            Expect(LexicalAnalyzer.dosy, "Ожидается 'do'");
            ParseStatement();
        }

        private void ParseRepeatStatement()
        {
            // repeat-statement -> REPEAT statement { ; statement } UNTIL expression
            Accept(LexicalAnalyzer.repeatsy);

            ParseStatement();

            while (Accept(LexicalAnalyzer.semicolon))
            {
                if (current_token_ != LexicalAnalyzer.untilsy)
                {
                    ParseStatement();
                }
            }

            Expect(LexicalAnalyzer.untilsy, "Ожидается 'until'");
            ParseExpression();
        }

        private void ParseIfStatement()
        {
            // if-statement -> IF expression THEN statement [ELSE statement]
            Accept(LexicalAnalyzer.ifsy);
            ParseExpression();
            Expect(LexicalAnalyzer.thensy, "Ожидается 'then'");
            ParseStatement();

            if (Accept(LexicalAnalyzer.elsesy))
            {
                ParseStatement();
            }
        }

        private void PrintErrors()
        {
            Console.WriteLine("\n=== РЕЗУЛЬТАТЫ СИНТАКСИЧЕСКОГО АНАЛИЗА ===");

            if (errors_.Count == 0)
            {
                Console.WriteLine("✓ Синтаксических ошибок не обнаружено");
                Console.WriteLine("Программа синтаксически корректна!");
            }
            else
            {
                Console.WriteLine($"✗ Обнаружено синтаксических ошибок: {errors_.Count}\n");

                for (int i = 0; i < errors_.Count; i++)
                {
                    Console.WriteLine($"{i + 1}. {errors_[i].Message}");
                }
            }
        }
    }

    struct SyntaxError
    {
        public int Position { get; }
        public string Message { get; }

        public SyntaxError(int position, string message)
        {
            Position = position;
            Message = message;
        }
    }
}
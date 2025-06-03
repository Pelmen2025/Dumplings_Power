using System.Collections.Generic;

namespace PascalLexer
{
    class Keywords
    {
        public Dictionary<byte, Dictionary<string, byte>> Keyword { get; } = new();

        public Keywords()
        {
            InitializeKeywords();
        }

        private void InitializeKeywords()
        {
            Keyword[2] = new Dictionary<string, byte>
            {
                ["do"] = LexicalAnalyzer.dosy,
                ["if"] = LexicalAnalyzer.ifsy,
                ["in"] = LexicalAnalyzer.insy,
                ["of"] = LexicalAnalyzer.ofsy,
                ["or"] = LexicalAnalyzer.orsy,
                ["to"] = LexicalAnalyzer.tosy
            };

            Keyword[3] = new Dictionary<string, byte>
            {
                ["end"] = LexicalAnalyzer.endsy,
                ["var"] = LexicalAnalyzer.varsy,
                ["div"] = LexicalAnalyzer.divsy,
                ["and"] = LexicalAnalyzer.andsy,
                ["not"] = LexicalAnalyzer.notsy,
                ["for"] = LexicalAnalyzer.forsy,
                ["mod"] = LexicalAnalyzer.modsy,
                ["nil"] = LexicalAnalyzer.nilsy,
                ["set"] = LexicalAnalyzer.setsy
            };

            Keyword[4] = new Dictionary<string, byte>
            {
                ["then"] = LexicalAnalyzer.thensy,
                ["else"] = LexicalAnalyzer.elsesy,
                ["case"] = LexicalAnalyzer.casesy,
                ["file"] = LexicalAnalyzer.filesy,
                ["goto"] = LexicalAnalyzer.gotosy,
                ["type"] = LexicalAnalyzer.typesy,
                ["with"] = LexicalAnalyzer.withsy,
                ["real"] = LexicalAnalyzer.realsy,
                ["char"] = LexicalAnalyzer.charsy
            };

            Keyword[5] = new Dictionary<string, byte>
            {
                ["begin"] = LexicalAnalyzer.beginsy,
                ["while"] = LexicalAnalyzer.whilesy,
                ["array"] = LexicalAnalyzer.arraysy,
                ["const"] = LexicalAnalyzer.constsy,
                ["label"] = LexicalAnalyzer.labelsy,
                ["until"] = LexicalAnalyzer.untilsy
            };

            Keyword[6] = new Dictionary<string, byte>
            {
                ["downto"] = LexicalAnalyzer.downtosy,
                ["packed"] = LexicalAnalyzer.packedsy,
                ["record"] = LexicalAnalyzer.recordsy,
                ["repeat"] = LexicalAnalyzer.repeatsy
            };

            Keyword[7] = new Dictionary<string, byte>
            {
                ["program"] = LexicalAnalyzer.programsy,
                ["integer"] = LexicalAnalyzer.integersy
            };

            Keyword[8] = new Dictionary<string, byte>
            {
                ["function"] = LexicalAnalyzer.functionsy
            };

            Keyword[9] = new Dictionary<string, byte>
            {
                ["procedure"] = LexicalAnalyzer.procedurensy
            };
        }

        public bool IsKeyword(string word, out byte code)
        {
            code = 0;
            if (string.IsNullOrEmpty(word))
                return false;

            byte length = (byte)word.Length;
            string lowerWord = word.ToLower();

            if (Keyword.TryGetValue(length, out var dict) && dict.TryGetValue(lowerWord, out code))
            {
                return true;
            }
            return false;
        }
    }
}
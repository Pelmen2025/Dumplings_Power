using PascalAnalyzer;
using System.Collections.Generic;

namespace PascalAnalyzer
{
    class Keywords
    {
        private readonly Dictionary<byte, Dictionary<string, byte>> _keywords = new();

        public Keywords()
        {
            InitializeKeywords();
        }

        private void InitializeKeywords()
        {
            _keywords[2] = new Dictionary<string, byte>
            {
                ["do"] = LexicalAnalyzer.dosy,
                ["to"] = LexicalAnalyzer.tosy,
                ["or"] = LexicalAnalyzer.orsy
            };

            _keywords[3] = new Dictionary<string, byte>
            {
                ["end"] = LexicalAnalyzer.endsy,
                ["var"] = LexicalAnalyzer.varsy,
                ["and"] = LexicalAnalyzer.andsy,
                ["for"] = LexicalAnalyzer.forsy
            };

            _keywords[4] = new Dictionary<string, byte>
            {
                ["real"] = LexicalAnalyzer.realsy,
                ["char"] = LexicalAnalyzer.charsy
            };

            _keywords[5] = new Dictionary<string, byte>
            {
                ["begin"] = LexicalAnalyzer.beginsy,
                ["while"] = LexicalAnalyzer.whilesy
            };

            _keywords[6] = new Dictionary<string, byte>
            {
                ["downto"] = LexicalAnalyzer.downtosy
            };

            _keywords[7] = new Dictionary<string, byte>
            {
                ["integer"] = LexicalAnalyzer.integersy
            };
        }

        public bool TryGetKeywordCode(string word, out byte code)
        {
            code = 0;
            if (string.IsNullOrEmpty(word))
            {
                return false;
            }

            byte length = (byte)word.Length;
            string lowerWord = word.ToLower();
            return _keywords.TryGetValue(length, out var dict) && dict.TryGetValue(lowerWord, out code);
        }
    }
}
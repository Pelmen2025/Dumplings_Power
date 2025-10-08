namespace PascalAnalyzer
{
    struct TextPosition
    {
        public int LineNumber { get; }
        public int CharNumber { get; }

        public TextPosition(int lineNumber, int charNumber)
        {
            LineNumber = lineNumber;
            CharNumber = charNumber;
        }
    }
}
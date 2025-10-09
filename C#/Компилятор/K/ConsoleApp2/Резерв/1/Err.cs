namespace PascalAnalyzer
{
    struct Err
    {
        public TextPosition Position { get; }
        public byte ErrorCode { get; }

        public Err(TextPosition position, byte errorCode)
        {
            Position = position;
            ErrorCode = errorCode;
        }
    }
}
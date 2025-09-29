namespace PascalLexer
{
    struct Err
    {
        public TextPosition Position { get; set; }
        public byte ErrorCode { get; set; }

        public Err(TextPosition position, byte errorCode)
        {
            Position = position;
            ErrorCode = errorCode;
        }
    }
}

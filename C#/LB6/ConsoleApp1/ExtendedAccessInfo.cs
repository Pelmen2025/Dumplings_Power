public class ExtendedAccessInfo : AccessInfo
{
    private string _userName;

    public ExtendedAccessInfo(string userName, bool hasStudentCard, bool hasPass)
        : base(hasStudentCard, hasPass)
    {
        this._userName = userName;
    }

    public ExtendedAccessInfo(ExtendedAccessInfo other)
        : base(other)
    {
        _userName = other._userName;
    }

    public void DisplayGreeting()
    {
        Console.WriteLine($"Здравствуйте, {_userName}!");
    }

    public void DisplayAccessStatus()
    {
        Console.WriteLine(HasAccess() ? "Доступ разрешён." : "Доступ запрещён.");
    }

    public override string ToString()
    {
        return $"Пользователь: {_userName}, " + base.ToString();
    }
}

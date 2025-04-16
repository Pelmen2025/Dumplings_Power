public class ExtendedAccessInfo : AccessInfo
{
    private string _userName;

    public ExtendedAccessInfo(string userName, bool hasStudentCard, bool hasPass)
        : base(hasStudentCard, hasPass)
    {
        UserName = userName;
    }

    public ExtendedAccessInfo(ExtendedAccessInfo other)
        : base(other)
    {
        UserName = other.UserName;
    }

    public string UserName
    {
        get { return _userName; }
        set { _userName = value; }
    }

    public void DisplayGreeting()
    {
        Console.WriteLine($"Здравствуйте, {UserName}!");
    }

    public void DisplayAccessStatus()
    {
        Console.WriteLine(HasAccess() ? "Доступ разрешён." : "Доступ запрещён.");
    }

    public override string ToString()
    {
        return $"Пользователь: {UserName}, " + base.ToString();
    }
}

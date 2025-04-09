public class ExtendedAccessInfo : AccessInfo
{
    private string userName;

    public ExtendedAccessInfo(string userName, bool hasStudentCard, bool hasPass)
        : base(hasStudentCard, hasPass)
    {
        this.userName = userName;
    }

    public ExtendedAccessInfo(ExtendedAccessInfo other)
        : base(other)
    {
        userName = other.userName;
    }

    public void DisplayGreeting()
    {
        Console.WriteLine($"Здравствуйте, {userName}!");
    }

    public void DisplayAccessStatus()
    {
        Console.WriteLine(HasAccess() ? "Доступ разрешён." : "Доступ запрещён.");
    }

    public override string ToString()
    {
        return $"Пользователь: {userName}, " + base.ToString();
    }
}

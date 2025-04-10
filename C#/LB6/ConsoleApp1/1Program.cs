using System;

class Program
{
    static void Main(string[] args)
    {
        Console.WriteLine("=== Проверка доступа ===");

        Console.Write("Введите ваше имя: ");
        string name = Console.ReadLine();

        bool hasStudentCard = InputValidator.GetBooleanInput("Есть ли у вас студенческий билет?");
        bool hasPass = InputValidator.GetBooleanInput("Есть ли у вас пропуск?");


        AccessInfo access = new AccessInfo(hasStudentCard, hasPass);
        Console.WriteLine("\n[Базовый класс]");
        Console.WriteLine(access.ToString());
        Console.WriteLine(access.HasAccess() ? "Доступ разрешён." : "Доступ запрещён.");


        AccessInfo copyAccess = new AccessInfo(access);
        Console.WriteLine("\n[Копия базового класса]");
        Console.WriteLine(copyAccess.ToString());


        ExtendedAccessInfo extendedAccess = new ExtendedAccessInfo(name, hasStudentCard, hasPass);
        Console.WriteLine("\n[Дочерний класс]");
        extendedAccess.DisplayGreeting();
        Console.WriteLine(extendedAccess.ToString());
        extendedAccess.DisplayAccessStatus();


        ExtendedAccessInfo copyExtended = new ExtendedAccessInfo(extendedAccess);
        Console.WriteLine("\n[Копия дочернего класса]");
        Console.WriteLine(copyExtended.ToString());
        copyExtended.DisplayAccessStatus();
    }
}

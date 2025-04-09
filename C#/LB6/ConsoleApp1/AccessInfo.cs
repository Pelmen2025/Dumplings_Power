using System;

public class AccessInfo
{
    protected bool hasStudentCard;
    protected bool hasPass;

    public AccessInfo(bool hasStudentCard, bool hasPass)
    {
        this.hasStudentCard = hasStudentCard;
        this.hasPass = hasPass;
    }

    public AccessInfo(AccessInfo other)
    {
        hasStudentCard = other.hasStudentCard;
        hasPass = other.hasPass;
    }

    public bool HasAccess()
    {
        return hasStudentCard || hasPass;
    }

    public override string ToString()
    {
        return $"Студенческий билет: {(hasStudentCard ? "да" : "нет")}, Пропуск: {(hasPass ? "да" : "нет")}";
    }
}

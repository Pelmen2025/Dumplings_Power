using System;

public class AccessInfo
{
    private bool _hasStudentCard;
    private bool _hasPass;

    public AccessInfo(bool hasStudentCard, bool hasPass)
    {
        this._hasStudentCard = hasStudentCard;
        this._hasPass = hasPass;
    }

    public AccessInfo(AccessInfo other)
    {
        _hasStudentCard = other._hasStudentCard;
        _hasPass = other._hasPass;
    }

    public bool HasAccess()
    {
        return _hasStudentCard || _hasPass;
    }

    public override string ToString()
    {
        return $"Студенческий билет: {(_hasStudentCard ? "да" : "нет")}, Пропуск: {(_hasPass ? "да" : "нет")}";
    }
}

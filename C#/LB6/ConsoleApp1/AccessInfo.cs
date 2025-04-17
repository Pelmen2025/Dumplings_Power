using System;

public class AccessInfo
{
    private bool _hasStudentCard;
    private bool _hasPass;

    public AccessInfo(bool hasStudentCard, bool hasPass)
    {
        HasStudentCard = hasStudentCard;
        HasPass = hasPass;
    }

    public AccessInfo(AccessInfo other)
    {
        HasStudentCard = other.HasStudentCard;
        HasPass = other.HasPass;
    }

    public bool HasStudentCard
    {
        get 
        { 
            return _hasStudentCard;
        }
        set 
        { 
            _hasStudentCard = value; 
        }
    }

    public bool HasPass
    {
        get 
        {
            return _hasPass;
        }
        set 
        { 
            _hasPass = value; 
        }
    }

    public bool HasAccess()
    {
        return HasStudentCard || HasPass;
    }

    public override string ToString()
    {
        return $"Студенческий билет: {(HasStudentCard ? "да" : "нет")}, Пропуск: {(HasPass ? "да" : "нет")}";
    }
}

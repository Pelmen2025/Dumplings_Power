using System;
using System.Collections.Generic;


public class FullName
{
    private string? _fam;
    private string? _name;
    private string? _otchestvo;

    public FullName(string? fam = null, string? name = null, string? otchestvo = null)
    {
        Fam = fam;
        Name = name;
        Otchestvo = otchestvo;
    }

    public string? Fam
    {
        get => _fam;
        set => _fam = string.IsNullOrWhiteSpace(value) ? null : value.Trim();
    }

    public string? Name
    {
        get => _name;
        set => _name = string.IsNullOrWhiteSpace(value) ? null : value.Trim();
    }

    public string? Otchestvo
    {
        get => _otchestvo;
        set => _otchestvo = string.IsNullOrWhiteSpace(value) ? null : value.Trim();
    }

    public override string ToString()
    {
        var parts = new List<string>();

        if (!string.IsNullOrEmpty(Fam)) parts.Add(Fam);
        if (!string.IsNullOrEmpty(Name)) parts.Add(Name);
        if (!string.IsNullOrEmpty(Otchestvo)) parts.Add(Otchestvo);

        return parts.Count > 0 ? string.Join(" ", parts) : "(имя не задано)";
    }
}
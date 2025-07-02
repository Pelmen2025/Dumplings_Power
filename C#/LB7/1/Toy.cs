using System;

[Serializable]
public class Toy
{
    private string _name;
    private double _price;
    private string _ageRange;

    public string Name
    {
        get { return _name; }
        set { _name = value; }
    }
    public double Price
    {
        get { return _price; }
        set { _price = value; }
    }
    public string AgeRange
    {
        get { return _ageRange; }
        set { _ageRange = value; }
    }
}
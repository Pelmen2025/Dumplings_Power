using System;

public class Photo
{
    public int ID { get; set; }
    public string Title { get; set; } = string.Empty;
    public DateTime DateTaken { get; set; }
    public long FileSize { get; set; }
    public string Resolution { get; set; } = string.Empty;

    public Photo() { }

    public Photo(int id, string title, DateTime dateTaken, long fileSize, string resolution)
    {
        ID = id;
        Title = title ?? string.Empty;
        DateTaken = dateTaken;
        FileSize = fileSize;
        Resolution = resolution ?? string.Empty;
    }

    public override string ToString()
    {
        return $"ID: {ID}, Title: {Title}, Date Taken: {DateTaken.ToShortDateString()}, Size: {FileSize} bytes, Resolution: {Resolution}";
    }
}
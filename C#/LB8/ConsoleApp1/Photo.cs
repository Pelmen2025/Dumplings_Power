using System;

public class Photo
{
    private int ID { get; set; }
    private string Title { get; set; } = string.Empty;
    private DateTime DateTaken { get; set; }
    private long FileSize { get; set; }
    private string Resolution { get; set; } = string.Empty;
    
    public Photo() { }

    public Photo(int id, string title, DateTime dateTaken, long fileSize, string resolution)
    {
        ID = id;
        Title = title ?? string.Empty;
        DateTaken = dateTaken;
        FileSize = fileSize;
        Resolution = resolution ?? string.Empty;
    }

    public int GetID() => ID;
    public void SetID(int id) => ID = id;

    public string GetTitle() => Title;
    public void SetTitle(string title) => Title = title ?? string.Empty;

    public DateTime GetDateTaken() => DateTaken;
    public void SetDateTaken(DateTime date) => DateTaken = date;

    public long GetFileSize() => FileSize;
    public void SetFileSize(long size) => FileSize = size;

    public string GetResolution() => Resolution;
    public void SetResolution(string resolution) => Resolution = resolution ?? string.Empty;

    public override string ToString()
    {
        return $"ID: {ID}, Title: {Title}, Date Taken: {DateTaken.ToShortDateString()}, " +
            $"Size: {FileSize} bytes, Resolution: {Resolution}";
    }
}
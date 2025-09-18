using System;

namespace PhotoCatalogApp
{
    public class Photo
    {
        private int _id { get; set; }
        private string _title { get; set; } = string.Empty;
        private DateTime _dateTaken { get; set; }
        private long _fileSize { get; set; }
        private string _resolution { get; set; } = string.Empty;

        public Photo()
        {
        }

        public Photo(int id, string title, DateTime dateTaken, long fileSize, string resolution)
        {
            _id = id;
            _title = title ?? string.Empty;
            _dateTaken = dateTaken;
            _fileSize = fileSize;
            _resolution = resolution ?? string.Empty;
        }

        public int Id
        {
            get => _id;
            set => _id = value;
        }

        public string Title
        {
            get => _title;
            set => _title = value ?? string.Empty;
        }

        public DateTime DateTaken
        {
            get => _dateTaken;
            set => _dateTaken = value;
        }

        public long FileSize
        {
            get => _fileSize;
            set => _fileSize = value;
        }

        public string Resolution
        {
            get => _resolution;
            set => _resolution = value ?? string.Empty;
        }

        public override string ToString()
        {
            return $"ID: {_id}, Title: {_title}, Date Taken: {_dateTaken.ToShortDateString()}, Size: {_fileSize} bytes, Resolution: {_resolution}";
        }
    }
}

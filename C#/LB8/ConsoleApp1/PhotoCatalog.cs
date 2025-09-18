using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace PhotoCatalogApp
{
    public class PhotoCatalog
    {
        private const string FilePath = "photos.bin";

        public List<Photo> LoadFromFile()
        {
            try
            {
                if (File.Exists(FilePath))
                {
                    var photos = new List<Photo>();
                    using (var fs = new FileStream(FilePath, FileMode.Open))
                    using (var reader = new BinaryReader(fs, Encoding.UTF8))
                    {
                        int count = reader.ReadInt32();
                        for (int i = 0; i < count; i++)
                        {
                            var photo = new Photo
                            {
                                Id = reader.ReadInt32(),
                                Title = reader.ReadString(),
                                DateTaken = new DateTime(reader.ReadInt64()),
                                FileSize = reader.ReadInt64(),
                                Resolution = reader.ReadString()
                            };
                            photos.Add(photo);
                        }
                    }
                    return photos;
                }
                else
                {
                    Console.WriteLine("Файл не найден. Создан новый каталог.");
                    return new List<Photo>();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Ошибка при загрузке файла: {ex.Message}");
                return new List<Photo>();
            }
        }

        public void SaveToFile(List<Photo> photos)
        {
            try
            {
                using (var fs = new FileStream(FilePath, FileMode.Create))
                using (var writer = new BinaryWriter(fs, Encoding.UTF8))
                {
                    writer.Write(photos.Count);
                    foreach (var photo in photos)
                    {
                        writer.Write(photo.Id);
                        writer.Write(photo.Title);
                        writer.Write(photo.DateTaken.Ticks);
                        writer.Write(photo.FileSize);
                        writer.Write(photo.Resolution);
                    }
                }
                Console.WriteLine("Каталог успешно сохранён.");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Ошибка при сохранении файла: {ex.Message}");
            }
        }

        public void View(List<Photo> photos)
        {
            if (photos.Count == 0)
            {
                Console.WriteLine("Каталог пуст.");
                return;
            }
            foreach (var photo in photos)
            {
                Console.WriteLine(photo.ToString());
            }
        }

        public bool DeleteById(List<Photo> photos, int id)
        {
            var photoToRemove = photos.FirstOrDefault(p => p.Id == id);
            if (photoToRemove != null)
            {
                photos.Remove(photoToRemove);
                return true;
            }
            return false;
        }

        public void Add(List<Photo> photos, Photo photo)
        {
            photos.Add(photo);
        }

        public List<Photo> GetPhotosAfterDate(List<Photo> photos, DateTime date)
        {
            return photos.Where(p => p.DateTaken > date).ToList();
        }

        public List<Photo> GetPhotosLargerThanSize(List<Photo> photos, long size)
        {
            return photos.Where(p => p.FileSize > size).ToList();
        }

        public int GetTotalPhotosCount(List<Photo> photos)
        {
            return photos.Count;
        }

        public double GetAverageFileSize(List<Photo> photos)
        {
            if (photos.Count == 0)
            {
                return 0;
            }
            return photos.Average(p => p.FileSize);
        }
    }
}

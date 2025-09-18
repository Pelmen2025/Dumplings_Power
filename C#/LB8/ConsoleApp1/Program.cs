using System;
using System.Collections.Generic;
using System.Linq;

namespace PhotoCatalogApp
{
    class Program
    {
        static void Main(string[] args)
        {
            PhotoCatalog catalog = new PhotoCatalog();
            List<Photo> photos = catalog.LoadFromFile();

            if (photos.Count == 0)
            {
                InitializeTestData(catalog, photos);
            }

            bool running = true;

            while (running)
            {
                Console.WriteLine("\nМеню фотокаталога:");
                Console.WriteLine("1. Просмотреть все фото");
                Console.WriteLine("2. Добавить новое фото");
                Console.WriteLine("3. Удалить фото по ID");
                Console.WriteLine("4. Запрос: фото после определённой даты");
                Console.WriteLine("5. Запрос: фото больше указанного размера");
                Console.WriteLine("6. Запрос: общее количество фото");
                Console.WriteLine("7. Запрос: средний размер файла");
                Console.WriteLine("8. Сохранить и выйти");

                Console.Write("Введите ваш выбор: ");
                string choice = Console.ReadLine();

                try
                {
                    switch (choice)
                    {
                        case "1":
                            catalog.View(photos);
                            break;
                        case "2":
                            AddPhoto(catalog, photos);
                            break;
                        case "3":
                            DeletePhoto(catalog, photos);
                            break;
                        case "4":
                            QueryPhotosAfterDate(catalog, photos);
                            break;
                        case "5":
                            QueryPhotosLargerThanSize(catalog, photos);
                            break;
                        case "6":
                            Console.WriteLine($"Всего фото: {catalog.GetTotalPhotosCount(photos)}");
                            break;
                        case "7":
                            Console.WriteLine($"Средний размер файла: {catalog.GetAverageFileSize(photos)} байт");
                            break;
                        case "8":
                            catalog.SaveToFile(photos);
                            running = false;
                            break;
                        default:
                            Console.WriteLine("Неверный выбор. Попробуйте снова.");
                            break;
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine($"Произошла ошибка: {ex.Message}");
                }
            }
        }

        static void InitializeTestData(PhotoCatalog catalog, List<Photo> photos)
        {
            photos.Clear();
            photos.Add(new Photo(1, "Sunset Beach", new DateTime(2023, 6, 15), 5242880, "1920x1080"));
            photos.Add(new Photo(2, "Mountain View", new DateTime(2024, 1, 20), 8388608, "2560x1440"));
            photos.Add(new Photo(3, "City Night", new DateTime(2025, 3, 10), 2097152, "1280x720"));
            photos.Add(new Photo(4, "Forest Path", new DateTime(2022, 11, 5), 10485760, "3840x2160"));
            photos.Add(new Photo(5, "Portrait", new DateTime(2024, 7, 30), 3145728, "1600x900"));
            catalog.SaveToFile(photos);
            Console.WriteLine("Тестовые данные инициализированы и сохранены в photos.bin");
        }

        static void AddPhoto(PhotoCatalog catalog, List<Photo> photos)
        {
            Console.Write("Введите ID: ");
            int id = int.Parse(Console.ReadLine());
            Console.Write("Введите название: ");
            string title = Console.ReadLine();
            Console.Write("Введите дату съёмки (гггг-мм-дд): ");
            DateTime date = DateTime.Parse(Console.ReadLine());
            Console.Write("Введите размер файла (в байтах): ");
            long size = long.Parse(Console.ReadLine());
            Console.Write("Введите разрешение (например, 1920x1080): ");
            string resolution = Console.ReadLine();
            Photo newPhoto = new Photo(id, title, date, size, resolution);
            catalog.Add(photos, newPhoto);
            Console.WriteLine("Фото добавлено.");
        }

        static void DeletePhoto(PhotoCatalog catalog, List<Photo> photos)
        {
            Console.Write("Введите ID для удаления: ");
            int id = int.Parse(Console.ReadLine());
            if (catalog.DeleteById(photos, id))
            {
                Console.WriteLine("Фото удалено.");
            }
            else
            {
                Console.WriteLine("Фото с таким ID не найдено.");
            }
        }

        static void QueryPhotosAfterDate(PhotoCatalog catalog, List<Photo> photos)
        {
            Console.Write("Введите дату (гггг-мм-дд): ");
            DateTime date = DateTime.Parse(Console.ReadLine());
            var result = catalog.GetPhotosAfterDate(photos, date);
            Console.WriteLine("Фото после указанной даты:");
            foreach (var photo in result)
            {
                Console.WriteLine(photo.ToString());
            }
        }

        static void QueryPhotosLargerThanSize(PhotoCatalog catalog, List<Photo> photos)
        {
            Console.Write("Введите минимальный размер (в байтах): ");
            long size = long.Parse(Console.ReadLine());
            var result = catalog.GetPhotosLargerThanSize(photos, size);
            Console.WriteLine("Фото больше указанного размера:");
            foreach (var photo in result)
            {
                Console.WriteLine(photo.ToString());
            }
        }
    }
}
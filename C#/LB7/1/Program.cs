
namespace LabTasks
{
    public class MainClass
    {
        public static void Main(string[] args)
        {
            while (true)
            {
                Console.WriteLine("Выберите задание (1-10) или 0 для выхода:");
                if (!int.TryParse(Console.ReadLine(), out int choice))
                {
                    Console.WriteLine("Некорректный ввод. Попробуйте снова.");
                    continue;
                }

                if (choice == 0)
                {
                    Console.WriteLine("Выход из программы.");
                    break;
                }

                switch (choice)
                {
                    case 1:
                        TasksClass.Task1();
                        break;
                    case 2:
                        TasksClass.Task2();
                        break;
                    case 3:
                        TasksClass.Task3();
                        break;
                    case 4:
                        TasksClass.Task4();
                        break;
                    case 5:
                        TasksClass.Task5();
                        break;
                    case 6:
                        TasksClass.Task6();
                        break;
                    case 7:
                        TasksClass.Task7();
                        break;
                    //case 8:
                    //    TasksClass.Task8();
                    //    break;
                    case 9:
                        TasksClass.Task9();
                        break;
                    case 10:
                        TasksClass.Task10();
                        break;
                    default:
                        Console.WriteLine("Неверный выбор. Пожалуйста, выберите задание от 1 до 10 или 0 для выхода.");
                        break;
                }
            }
        }
    }
}

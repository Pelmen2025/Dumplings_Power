// Задание 8
public static void Task8()
{
    HashSet<string> trcList = new HashSet<string> { "ТРЦ1", "ТРЦ2", "ТРЦ3" };
    Dictionary<string, HashSet<string>> studentsTRC = new Dictionary<string, HashSet<string>>
            {
                { "Student1", new HashSet<string> { "ТРЦ1", "ТРЦ3" } },
                { "Student2", new HashSet<string> { "ТРЦ2", "ТРЦ3" } },
                { "Student3", new HashSet<string> { "ТРЦ1", "ТРЦ2" } }
            };

    var allVisited = trcList.Where(trc => studentsTRC.All(student => student.Value.Contains(trc))).ToList();
    var someVisited = trcList.Where(trc => studentsTRC.Any(student => student.Value.Contains(trc))).ToList();
    var noneVisited = trcList.Where(trc => studentsTRC.All(student => !student.Value.Contains(trc))).ToList();

    Console.WriteLine($"Все студенты посещали: {string.Join(", ", allVisited)}");
    Console.WriteLine($"Некоторые студенты посещали: {string.Join(", ", someVisited)}");
    Console.WriteLine($"Никто не посещал: {string.Join(", ", noneVisited)}");
}
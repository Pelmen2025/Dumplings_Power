using test2;

internal class Test2
{
    private static void Main(string[] args)
    {
        Console.WriteLine("Введите 2 числа!");
        int x, y;
        Summa summa = new Summa();
        Summa summa1 = new Summa(10,10);
        Summa summa2 = new Summa(summa);

        summa.Print();
        summa1.Print();
        Console.WriteLine(summa.X);
        //Console.WriteLine(summa.Y);
        summa.X = 10;
        summa.Y = 15;
        Console.WriteLine(summa.ToString());

        //x = int.Parse(Console.ReadLine());
        //Вызвать метод проверки из класа проверки
        //y = Convert.ToInt32(Console.ReadLine());
        //Console.WriteLine("Сумма = "+ (x + y));

    }
}
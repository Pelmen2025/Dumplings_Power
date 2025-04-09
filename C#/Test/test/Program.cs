using System.Diagnostics;

internal class Program
{
    private static void Main(string[] args)
    {
        Console.WriteLine("Hello, World!");
        int x,y;
        string s;
        x = int.Parse(Console.ReadLine());
        y = Convert.ToInt32(Console.ReadLine());

        if (x > y)
        {
            Console.WriteLine("x > y");
        }
        else
        {
            Console.WriteLine("x < y");
        }

        switch (x)
        {
            case 0:
                {
                    y = 1;
                    s = null;
                    break;
                }
            case 1: y = 2; break;
            default: y = 0; break;
        }

        for (int i = 0; i < 10; i++)
        {
            
        }

        //Пока правда
        while (x > 0)
        {
            x--;
        }

        // Пока ложь
        do
        {
            // Действия
        } while (y > 0);

        int[] ar = new int[10];
        int[] ar1 = { 1, 2, 3 };

        Random random = new Random();
        for(int i =0;i<=ar.Length; i++)
        {
            ar[i] = random.Next(0,101);
        }
        int[,] ar2 = new int[10, 10];
        for (int i=0; i <= ar2.GetLength(0); i++)
            for (int j=0; j<= ar2.GetLength(1); j++)
            {
                ar2[i, j] = random.Next(0, 101);
            }



        //s = Console.ReadLine();
    }
}
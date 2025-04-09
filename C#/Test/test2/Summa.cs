namespace test2
{
    class Summa
    {
        ///Поля
        private int x;
        int y;
        ///Конструкторы
        //По умолчанию
        public Summa()
        {
            x = 0;
            y = 0;
        }

        //С параметром
        public Summa(int x, int y)
        {
            // x>0 y >0
            this.x = x;
            this.y = y;
        }

        //Копирование
        public Summa(Summa summa)
        {
            this.x = summa.x;
            this.y = summa.y;
        }

        ///Свойства
        public int X
        {
            get { return x; }
            set { x = value; }
            // set { if (value > 0) x = value; }
        }

        public int Y
        {
            get { return y; }
            set { y = value; }
        }

        ///Методы
        public int S()
        {
            return x + y;
        }

        public void Print()
        {
            Console.WriteLine("x + y = " + S());
        }

        public override string ToString()
        {
            return "x = " + x + ", y = " + y;
        }

    }
}

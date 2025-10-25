namespace T
{
    public class Time
    {
        private byte _hours;
        private byte _minutes;

        public byte Hours
        {
            get => _hours;
            set
            {
                if (value > 23)
                {
                    throw new ArgumentOutOfRangeException(nameof(value),
                        "Часы должны быть в диапазоне от 0 до 23.");
                }
                _hours = value;
            }
        }


        public byte Minutes
        {
            get => _minutes;
            set
            {
                if (value > 59)
                {
                    throw new ArgumentOutOfRangeException(nameof(value),
                        "Минуты должны быть в диапазоне от 0 до 59.");
                }
                _minutes = value;
            }
        }


        public Time()
        {
            _hours = 0;
            _minutes = 0;
        }


        public Time(byte hours, byte minutes)
        {
            Hours = hours;
            Minutes = minutes;
        }

        /// Возвращает строковое представление времени в формате "ЧЧ:ММ".
        public override string ToString()
        {
            return $"{_hours:D2}:{_minutes:D2}";
        }

        /// Вычитает одну минуту из времени.
        public void SubtractMinute()
        {
            if (_minutes > 0)
            {
                _minutes--;
            }
            else
            {
                _minutes = 59;
                if (_hours > 0)
                {
                    _hours--;
                }
                else
                {
                    _hours = 23;
                }
            }
        }

        /// Унарный оператор декремента (вычитает одну минуту).
        public static Time operator --(Time t)
        {
            if (t == null)
            {
                throw new ArgumentNullException(nameof(t));
            }
            Time result = new Time(t._hours, t._minutes);
            result.SubtractMinute();
            return result;
        }

        /// Неявное преобразование в int (возвращает общее количество минут).
        public static implicit operator int(Time t)
        {
            if (t == null)
            {
                throw new ArgumentNullException(nameof(t));
            }
            return t._hours * 60 + t._minutes;
        }
        /// Оператор "меньше" (сравнивает общее количество минут).
        public static bool operator <(Time t1, Time t2)
        {
            if (t1 == null || t2 == null)
            {
                throw new ArgumentNullException(
                    t1 == null ? nameof(t1) : nameof(t2));
            }
            int minutes1 = t1;
            int minutes2 = t2;
            return minutes1 < minutes2;
        }

        /// Оператор "больше" (сравнивает общее количество минут).
        public static bool operator >(Time t1, Time t2)
        {
            if (t1 == null || t2 == null)
            {
                throw new ArgumentNullException(
                    t1 == null ? nameof(t1) : nameof(t2));
            }
            int minutes1 = t1;
            int minutes2 = t2;
            return minutes1 > minutes2;
        }
    }
}
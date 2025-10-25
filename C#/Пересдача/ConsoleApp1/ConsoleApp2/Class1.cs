namespace G
{
    
    public class LineSegment
    {
        private double _start;
        private double _end;

       
        public double Start
        {
            get => _start;
            set => _start = value;
        }

        
        public double End
        {
            get => _end;
            set => _end = value;
        }

        
        public LineSegment()
        {
            _start = 0.0;
            _end = 0.0;
        }

        
        public LineSegment(double start, double end)
        {
            this._start = start;
            this._end = end;
        }

        /// Вычисляет длину отрезка.
        public double CalculateLength()
        {
            return Math.Abs(_end - _start);
        }

        /// Возвращает строковое представление отрезка.
        public override string ToString()
        {
            return $"[{_start}; {_end}], длина: {CalculateLength():F2}";
        }

        /// Неявное преобразование LineSegment в double.
        public static implicit operator double(LineSegment segment)
        {
            if (segment == null)
            {
                throw new ArgumentNullException(nameof(segment));
            }
            return segment._end;
        }

        /// Сдвигает отрезок на заданное число (правосторонняя операция).
        public static LineSegment operator +(LineSegment segment, int d)
        {
            if (segment == null)
            {
                throw new ArgumentNullException(nameof(segment));
            }
            return new LineSegment(segment._start + d, segment._end + d);
        }

        /// Сдвигает отрезок на заданное число (левосторонняя операция).
        public static LineSegment operator +(int d, LineSegment segment)
        {
            if (segment == null)
            {
                throw new ArgumentNullException(nameof(segment));
            }
            return new LineSegment(segment._start + d, segment._end + d);
        }
    }
}
open System

// Определение типа для комплексных чисел. Комплексное число состоит из действительной и мнимой части.
type Complex = { Real: float; Imag: float }

// Функция для сложения двух комплексных чисел.
let add (z1: Complex) (z2: Complex) =
    { Real = z1.Real + z2.Real; Imag = z1.Imag + z2.Imag }  // Складываем действительные и мнимые части по отдельности.

// Функция для вычитания двух комплексных чисел.
let subtract (z1: Complex) (z2: Complex) =
    { Real = z1.Real - z2.Real; Imag = z1.Imag - z2.Imag }  // Вычитаем действительные и мнимые части по отдельности.

// Функция для умножения двух комплексных чисел.
let multiply (z1: Complex) (z2: Complex) =
    { Real = z1.Real * z2.Real - z1.Imag * z2.Imag;   // Формула умножения комплексных чисел: (a + bi)(c + di) = ac - bd + (ad + bc)i
      Imag = z1.Real * z2.Imag + z1.Imag * z2.Real }   // Это мнимая часть: ad + bc

// Функция для деления двух комплексных чисел.
let divide (z1: Complex) (z2: Complex) =
    let denominator = z2.Real * z2.Real + z2.Imag * z2.Imag  // Нахождение знаменателя для деления.
    { Real = (z1.Real * z2.Real + z1.Imag * z2.Imag) / denominator;  // Числитель для действительной части.
      Imag = (z1.Imag * z2.Real - z1.Real * z2.Imag) / denominator }  // Числитель для мнимой части.

// Функция для возведения комплексного числа в целое число n.
let pow (z: Complex) (n: int) =
    let rec power acc baseVal  exp =  // Рекурсивная функция для вычисления степени.
        if exp = 0 then acc  // Если степень равна нулю, возвращаем 1 (единичное комплексное число).
        else power (multiply acc baseVal ) baseVal  (exp - 1)  // Рекурсивно умножаем базу на аккмулятор.
    power { Real = 1.0; Imag = 0.0 } z n  // Изначально аккумулятор равен 1 + 0i.

// Основная программа, точка входа.
[<EntryPoint>]
let main argv =
    // Запрос у пользователя данных для первого комплексного числа.
    printfn "Введите действительную и мнимую части первого комплексного числа"
    printf "Введите действительную часть: "
    let real1 = Console.ReadLine() |> float  // Чтение и преобразование ввода в число.
    printf "Введите мнимую часть: "
    let imag1 = Console.ReadLine() |> float  // Чтение и преобразование ввода в число.
    let z1 = { Real = real1; Imag = imag1 }  // Создание первого комплексного числа.

    // Запрос у пользователя данных для второго комплексного числа.
    printfn "Введите действительную и мнимую части второго комплексного числа: "
    printf "Введите действительную часть: "
    let real2 = Console.ReadLine() |> float  // Чтение и преобразование ввода в число.
    printf "Введите мнимую часть: "
    let imag2 = Console.ReadLine() |> float  // Чтение и преобразование ввода в число.
    let z2 = { Real = real2; Imag = imag2 }  // Создание второго комплексного числа.

    printf "Введите степень, в которую нужно возвести первое число: "
    let n = Console.ReadLine() |> int  // Читаем ввод и преобразуем в число.

    // Выполнение операций с комплексными числами.
    let sum = add z1 z2  // Сложение двух чисел.
    let difference = subtract z1 z2  // Вычитание двух чисел.
    let product = multiply z1 z2  // Умножение двух чисел.
    let quotient = divide z1 z2  // Деление двух чисел.
    let powerResult = pow z1 n  // Возводим первое число в степень n.

    // Вывод результатов операций.
    printfn "Сумма: %f + %fi" sum.Real sum.Imag  // Печать результата сложения.
    printfn "Разность: %f + %fi" difference.Real difference.Imag  // Печать результата вычитания.
    printfn "Произведение: %f + %fi" product.Real product.Imag  // Печать результата умножения.
    printfn "Частное: %f + %fi" quotient.Real quotient.Imag  // Печать результата деления.
    printfn "Результат возведения в степень: %f + %fi" powerResult.Real powerResult.Imag

    0  // Возвращаем 0, чтобы программа завершилась без ошибок.

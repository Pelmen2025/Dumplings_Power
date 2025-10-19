open System

// Функция для запроса у пользователя количества строк с проверкой ввода
let rec readInt () =
    printf "Введите количество строк: " // Запрос количества строк
    match Int32.TryParse(Console.ReadLine()) with // Попытка преобразовать ввод в целое число
    | (true, value) when value > 0 -> value // Если успешно и число положительное, возвращаем его
    | _ -> 
        printfn "Ошибка! Введите положительное число." // Сообщение об ошибке
        readInt () // Повторный запрос ввода

// Функция для запроса списка строк
let rec readLines count acc =
    if count = 0 then List.rev acc // Когда все строки введены, возвращаем список в правильном порядке
    else
        printf "Введите строку: " // Запрос ввода строки
        let line = Console.ReadLine() // Чтение строки
        readLines (count - 1) (line :: acc) // Рекурсивный вызов, добавление строки в список

// Основная программа
[<EntryPoint>]
let main argv =
    let count = readInt () // Запрос количества строк
    let lines = readLines count [] // Чтение строк в список
    
    // Подсчёт строк с чётной длиной с использованием List.fold
    let evenLengthCount =
        lines |> List.fold (fun acc line -> if String.length line % 2 = 0 then acc + 1 else acc) 0
    //List.fold — это функция свертки (fold), которая проходит по всем элементам списка, 
    //начиная с начального значения (в данном случае 0), и накапливает результат
    printfn "Количество строк с чётной длиной: %d" evenLengthCount // Вывод результата
    0

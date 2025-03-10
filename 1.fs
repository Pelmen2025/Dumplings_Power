open System

// Функция, которая преобразует список вещественных чисел в список целых
let truncateNumbers (nums: list<float>) : list<int> =
    List.map int nums  

// Функция для чтения списка чисел с клавиатуры
let readNumbersFromInput () : list<float> =
    // Просим пользователя ввести числа через пробел
    printf "Введите числа через пробел: " 
    // Читаем ввод с консоли
    let input = Console.ReadLine()

    // Функция для разделения строки на слова вручную
    let rec splitWords (s: string) (current: string) (acc: list<string>) =
        match s with
        | "" -> 
            if current = "" then acc else current :: acc
        | _ when s.[0] = ' ' || s.[0] = '\t' -> 
            if current = "" then splitWords s.[1..] "" acc
            else splitWords s.[1..] "" (current :: acc)
        | _ -> 
            splitWords s.[1..] (current + string s.[0]) acc
    
    // Разбиваем строку на слова (списки чисел)
    match input with 
    | null | "" -> []  // Если строка пуста, возвращаем пустой список
    | _ ->
        splitWords input "" [] 
        |> List.fold (fun acc str ->
            match Double.TryParse(str) with
            | (true, num) -> num :: acc
            | _ -> acc
        ) []
        |> List.rev

[<EntryPoint>]
let main argv =
    // Вызываем функцию для чтения чисел с клавиатуры
    let numbers = readNumbersFromInput()  
    match numbers with
    | [] -> 
        // Если список пуст, выводим сообщение об ошибке
        printfn "Ошибка: введены некорректные данные или пустая строка."
    | _ ->
        // Преобразуем числа в целые
        let truncatedNumbers = truncateNumbers numbers |> List.rev  
        // Выводим результат преобразования
        printfn "Результат: %A" truncatedNumbers  
    0   // Завершаем программу с кодом 0 (успех)

open System

// Функция, которая преобразует список вещественных чисел в список целых
let truncateNumbers (nums: list<float>) : list<int> =
    List.map int nums  

// Функция для чтения списка чисел с клавиатуры
let readNumbersFromInput () : list<float> =
    printf "Введите числа через пробел: " 
    let input = Console.ReadLine()
    match input with 
    | null -> []  
    | _ ->
        input.Split([|' '; '\t'|], StringSplitOptions.RemoveEmptyEntries)
        |> List.fold (fun acc str ->
            match Double.TryParse(str) with
            | (true, num) -> num :: acc
            | _ -> acc
        ) []
        |> List.rev

[<EntryPoint>]
let main argv =
    let numbers = readNumbersFromInput()  
    match numbers with
    | [] -> 
        printfn "Ошибка: введены некорректные данные или пустая строка."
    | _ ->
        let truncatedNumbers = truncateNumbers numbers  
        printfn "Результат: %A" truncatedNumbers  
    0   

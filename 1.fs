open System

// Функция для удаления дробной части
let truncateNumbers (nums: list<float>) : list<int> =
    nums |> List.map int

// Функция для безопасного ввода списка чисел
let rec inputNumbers () : list<float> =
    printf "Введите список вещественных чисел через пробел: "
    let input = Console.ReadLine()
    match input with
    | null | "" ->
        printfn "Ошибка: пустой ввод. Попробуйте снова."
        inputNumbers()
    | _ ->
        let parts = input.Split([|' '|], StringSplitOptions.RemoveEmptyEntries) |> List.ofArray
        let parsedNumbers = 
            parts 
            |> List.fold (fun acc str ->
                match Double.TryParse(str) with
                | (true, num) -> num :: acc
                | _ -> 
                    printfn "Ошибка: '%s' не является числом." str
                    acc
            ) []
            |> List.rev  
        
        if List.isEmpty parsedNumbers then
            printfn "Ошибка: введите хотя бы одно корректное число."
            inputNumbers()
        else
            parsedNumbers

[<EntryPoint>]
let main argv =
    let numbers = inputNumbers()
    let truncatedNumbers = truncateNumbers numbers
    printfn "Результат: %A" truncatedNumbers
    0

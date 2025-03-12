open System

// Функция для запроса у пользователя количества строк с проверкой ввода
let rec readInt prompt =
    printf "%s" prompt
    match Int32.TryParse(Console.ReadLine()) with
    | (true, value) when value > 0 -> value
    | _ ->
        printfn "Ошибка! Введите положительное число."
        readInt prompt

// Функция для запроса списка строк
let rec readLines count acc =
    if count = 0 then List.rev acc
    else
        printf "Введите строку: "
        let line = Console.ReadLine()
        readLines (count - 1) (line :: acc)

// Основная программа
[<EntryPoint>]
let main argv =
    let count = readInt "Введите количество строк: "
    let lines = readLines count []
    
    let evenLengthCount =
        lines |> List.fold (fun acc line -> if String.length line % 2 = 0 then acc + 1 else acc) 0
    
    printfn "Количество строк с чётной длиной: %d" evenLengthCount
    0

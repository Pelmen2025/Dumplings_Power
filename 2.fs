open System

// Функция для проверки чётности длины строки
let isEvenLength (s: string) = s.Length % 2 = 0

// Функция для подсчёта строк с чётной длиной
let countEvenLengthStrings (lines: list<string>) =
    List.fold (fun acc line -> if isEvenLength line then acc + 1 else acc) 0 lines

// Основная функция взаимодействия с пользователем
[<EntryPoint>]
let main argv =
    printf "Введите количество строк: " 
    let countLines =
        match System.Int32.TryParse(Console.ReadLine()) with
        | (true, n) when n > 0 -> n
        | _ -> printfn "Ошибка ввода! Введите положительное число."; 0
    
    let rec readLines acc n index =
        if n = 0 then acc
        else 
            printf "Введите строку %d: " (index + 1)
            readLines (Console.ReadLine() :: acc) (n - 1) (index + 1)
    
    if countLines > 0 then
        let lines = readLines [] countLines 0 |> List.rev 
        let count = countEvenLengthStrings lines
        printfn "Количество строк с чётной длиной: %d" count
    
    0

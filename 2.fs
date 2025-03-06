open System

// Функция для подсчёта строк с чётной длиной (игнорирует пустые строки)
let countEvenLengthStrings (lines: list<string>) =
    List.fold (fun acc line -> 
        if not (String.IsNullOrWhiteSpace(line)) && line.Length % 2 = 0 then acc + 1 
        else acc) 0 lines

// Основная функция взаимодействия с пользователем
[<EntryPoint>]
let main argv =
    printf "Введите количество строк: " 
    let countLines =
        match System.Int32.TryParse(Console.ReadLine()) with
        | (true, n) when n > 0 -> 
            n
        | _ -> 
            printfn "Ошибка ввода! Введите положительное число."; 0
    
    // Рекурсивная функция для ввода строк
    let rec readLines acc n index =
        if n = 0 then acc
        else 
            printf "Введите строку %d: " (index + 1)
            let line = Console.ReadLine()
            if String.IsNullOrWhiteSpace(line) then
                printfn "Ошибка! Строка не должна быть пустой или состоять только из пробелов."
                readLines acc n index 
            else
                readLines (line :: acc) (n - 1) (index + 1)
    
    // Если количество строк больше 0, начинаем ввод строк
    if countLines > 0 then
        let lines = readLines [] countLines 0 |> List.rev 
        let count = countEvenLengthStrings lines
        printfn "Количество строк с чётной длиной: %d" count
    0

open System

// Функция для запроса у пользователя количества строк с проверкой ввода
let rec readInt () =
    printf "Введите количество строк: " 
    match Int32.TryParse(Console.ReadLine()) with 
    | (true, value) when value > 0 -> value 
    | _ -> 
        printfn "Ошибка! Введите положительное число." 
        readInt () 

// Функция для запроса строк с ленивым вычислением
let rec readLines count =
    seq {
        if count > 0 then
            printf "Введите строку: "
            yield Console.ReadLine()
            yield! readLines (count - 1)
    }

// Основная программа
[<EntryPoint>]
let main argv =
    let count = readInt () 
    let lines = readLines count 
    
    let evenLengthCount =
        lines
        |> Seq.fold (fun acc line -> if String.length line % 2 = 0 then acc + 1 else acc) 0
    
    printfn "Количество строк с чётной длиной: %d" evenLengthCount 
    0

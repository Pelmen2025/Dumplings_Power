open System
open System.IO

// Функция, которая преобразует список вещественных чисел в список целых
let truncateNumbers (nums: list<float>) : list<int> =
    nums |> List.map int  

// Функция для чтения списка чисел из файла
let readNumbersFromFile (filePath: string) : list<float> =
    if File.Exists(filePath) then
        File.ReadAllLines(filePath) 
        |> List.ofArray              
        |> List.choose (fun str ->   
            match Double.TryParse(str) with
            | (true, num) -> Some num
            | _ -> None)  
    else
        printfn "Ошибка: файл '%s' не найден." filePath
        []

[<EntryPoint>]
let main argv =
    let filePath = "D:\Byz\Языки программирования\Программы\2 Курс\1 Семестр\1 Лаба\ConsoleApp2\ConsoleApp1\List.map"  
    let numbers = readNumbersFromFile filePath  
    match numbers with
    | [] -> printfn "Ошибка: файл пуст или содержит некорректные данные."
    | _ ->
        let truncatedNumbers = truncateNumbers numbers  
        printfn "Результат: %A" truncatedNumbers  
    0  

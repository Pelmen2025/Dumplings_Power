open System

/// Лениво считываем вещественные числа из ввода пользователя 
let rec readFloatSeq () =
    seq {
        printf "Введите число (или оставьте пустым для завершения): "
        let input = Console.ReadLine()
        match input with
        | "" -> ()  
        | _ ->
            match Double.TryParse(input) with
            | (true, value) -> 
                yield value
                yield! readFloatSeq () 
            | (false, _) -> 
                printfn "Ошибка: введите корректное число!"
                yield! readFloatSeq () 
    }

/// Функция для преобразования элементов последовательности в целые числа
let truncateSeq (floatSeq: seq<float>) = floatSeq |> Seq.map int

/// Главная функция
[<EntryPoint>]
let main argv =
    let floatNumbers = readFloatSeq () 
    let intNumbers = truncateSeq floatNumbers 
    printfn "Список целых чисел: %A" (intNumbers |> Seq.toList) 
    0

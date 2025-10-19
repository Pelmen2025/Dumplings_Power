open System

/// Функция для считывания списка вещественных чисел из ввода пользователя
let rec readFloatList acc =
    printf "Введите число (или оставте пустой для завершения ввода): "
    let input = Console.ReadLine()
    match input with
    | "" -> List.rev acc
    | _ -> 
        match Double.TryParse(input) with
        | (true, value) -> readFloatList (value :: acc)
        | (false, _) ->
            printfn "Ошибка: введите корректное число!"
            readFloatList acc

/// Функция для отброса дробной части у всех чисел в списке
let truncateList floatList = List.map int floatList

/// Главная функция
[<EntryPoint>]
let main argv =
    let floatNumbers = readFloatList []
    let intNumbers = truncateList floatNumbers
    printfn "Список целых чисел: %A" intNumbers
    0 

let sumOfDigits arr =
    let rec digitSum n = 
        if n = 0 then 0
        else (n % 10) + digitSum (n / 10)
    
    arr |> Array.map (fun n -> digitSum (abs n))

[<EntryPoint>]
let main argv =
    let numbers = [|123; 456; 789; |]
    let result = sumOfDigits numbers
    printfn "Исходный массив: %A" numbers
    printfn "Сумма цифр для каждого числа: %A" result
    0 
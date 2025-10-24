module EvenBoolsProgram

open System

let getEvenBools (numbers: int list) : bool list =
    numbers |> List.map (fun x -> x % 2 = 0)

[<EntryPoint>]
let main argv =

    let inputNumbers = [1; 2; 3; 4; 5; 6; 7; 8; 9; 10]
    let result = getEvenBools inputNumbers
    
    printfn "Исходный список: %A" inputNumbers
    
    printfn "Чётность (true — чётное, false — нечётное): %A" result
    
    0
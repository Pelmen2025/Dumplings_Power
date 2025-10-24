let chetnoilinet (numbers: int list) : bool list =
    numbers |> List.map (fun x -> x % 2 = 0)

[<EntryPoint>]
let main argv =
    let numbers = [1; 2; 3; 4; 5; 6; 7; 8; 9; 10]
    let result = chetnoilinet numbers
    
    printfn "Исходный список: %A" numbers
    printfn "Чётность (true — чётное, false — нечётное): %A" result
    
    0
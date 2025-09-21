open System

// Определение типа бинарного дерева
type Tree<'T> =
    | Empty
    | Node of 'T * Tree<'T> * Tree<'T>

// Функция fold для дерева - свертка дерева к единственному значению
let rec foldTree f acc tree =
    match tree with
    | Empty -> acc
    | Node(value, left, right) ->
        let leftResult = foldTree f acc left      // Обрабатываем левое поддерево
        let currentResult = f leftResult value    // Применяем функцию к текущему значению
        foldTree f currentResult right           // Обрабатываем правое поддерево

// Функция для определения количества цифр в числе (значности)
let getDigitCount number =
    if number = 0 then 1
    else
        number
        |> abs
        |> string
        |> String.length

// Функция для запроса значности от пользователя
let rec getDigitCountFromUser () =
    printf "Введите требуемую значность (количество цифр): "
    match Console.ReadLine() |> Int32.TryParse with
    | (true, value) when value > 0 -> value
    | _ ->
        printfn "Ошибка! Введите положительное число."
        getDigitCountFromUser()

// Функция для подсчета элементов заданной значности
let countElementsWithDigitCount targetDigitCount tree =
    foldTree (fun acc value ->
        if getDigitCount value = targetDigitCount then
            acc + 1
        else
            acc
    ) 0 tree

// Функция для красивого вывода дерева
let rec printTree tree indent =
    match tree with
    | Empty -> ()
    | Node(value, left, right) ->
        printTree right (indent + "    ")
        printfn "%s%d (%d-значное)" indent value (getDigitCount value)
        printTree left (indent + "    ")

// Пример создания дерева с числами разной значности
let createSampleTree () =
    Node(123,                          
        Node(7,                         
            Node(45, Empty, Empty),     
            Node(1234, Empty, Empty)  
        ),
        Node(89,                       
            Node(5, Empty, Empty),     
            Node(678,                  
                Node(12, Empty, Empty), 
                Node(9999, Empty, Empty) 
            )
        )
    )



// Основная функция
[<EntryPoint>]
let main _ =
    printfn "=== Подсчет элементов заданной значности в дереве ==="
    
    // Создаем дерево с числами разной значности
    let tree = createSampleTree()
    
    printfn "\nДерево с указанием значности каждого числа:"
    printTree tree ""
    
    // Запрашиваем конкретную значность для подсчета
    let targetDigitCount = getDigitCountFromUser()
    
    // Используем fold для подсчета элементов заданной значности
    let result = countElementsWithDigitCount targetDigitCount tree
    
    printfn "\nРезультат: в дереве %d элемент(ов) %d-значных" result targetDigitCount
    
    // Демонстрация работы fold
    printfn "\nДемонстрация работы foldTree:"
    printfn "fold применяется к каждому узлу, накапливая количество"
    printfn "элементов с требуемой значностью."
    
    0
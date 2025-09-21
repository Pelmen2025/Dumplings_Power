open System

// Определение типа бинарного дерева
type Tree<'T> =
    | Empty
    | Node of 'T * Tree<'T> * Tree<'T>

// Функция map для дерева - преобразует каждый элемент дерева
let rec mapTree f tree =
    match tree with
    | Empty -> Empty
    | Node(value, left, right) -> 
        Node(f value, mapTree f left, mapTree f right)

// Функция для добавления цифры в начало числа
let prependDigit digit number =
    let digitStr = string digit
    let numberStr = string (abs number)
    let newNumberStr = digitStr + numberStr
    let result = int newNumberStr
    if number < 0 then -result else result

// Функция для запроса цифры от пользователя
let rec getDigit () =
    printf "Введите цифру для добавления в начало (0-9): "
    match Console.ReadLine() with
    | input when input.Length = 1 && Char.IsDigit(input.[0]) ->
        int (string input.[0])
    | _ ->
        printfn "Ошибка! Введите одну цифру от 0 до 9."
        getDigit()

// Функция для красивого вывода дерева
let rec printTree tree indent =
    match tree with
    | Empty -> ()
    | Node(value, left, right) ->
        printTree right (indent + "    ")
        printfn "%s%d" indent value
        printTree left (indent + "    ")

// Пример создания дерева
let createSampleTree () =
    Node(15,
        Node(7,
            Node(3, Empty, Empty),
            Node(12, Empty, Empty)
        ),
        Node(25,
            Node(18, Empty, Empty),
            Node(30, Empty, Empty)
        )
    )

// Основная функция
[<EntryPoint>]
let main _ =
    printfn "=== Построение нового дерева с добавлением цифры в начало чисел ==="
    
    // Создаем исходное дерево
    let originalTree = createSampleTree()
    
    printfn "\nИсходное дерево:"
    printTree originalTree ""
    
    // Запрашиваем цифру для добавления
    let digit = getDigit()
    
    // Применяем map для создания нового дерева
    let newTree = mapTree (prependDigit digit) originalTree
    
    printfn "\nНовое дерево (цифра %d добавлена в начало каждого числа):" digit
    printTree newTree ""
    
    printfn "\nПример преобразования:"
    printfn "15 -> %d" (prependDigit digit 15)
    printfn "7 -> %d" (prependDigit digit 7)
    printfn "25 -> %d" (prependDigit digit 25)
    
    0
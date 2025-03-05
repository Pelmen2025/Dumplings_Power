open System
open System.IO

// Функция для проверки чётности длины строки
let isEvenLength (s: string) = s.Length % 2 = 0

// Функция для обработки списка строк и подсчёта строк с чётной длиной
let countEvenLengthStrings (lines: string list) =
    List.fold (fun acc line -> if isEvenLength line then acc + 1 else acc) 0 lines

// Функция для чтения файла с проверкой существования
let readLinesFromFile (filePath: string) =
    if File.Exists(filePath) then
        Some (File.ReadLines(filePath) |> Seq.toList)
    else
        None

// Основная функция взаимодействия с пользователем
[<EntryPoint>]
let main argv =
    let filePath = "D:\Byz\Языки программирования\Программы\2 Курс\1 Семестр\Лабы\ConsoleApp2\ConsoleApp2\List.fold"
    match readLinesFromFile filePath with
    | Some lines ->
        let count = countEvenLengthStrings lines
        printfn "Количество строк с чётной длиной: %d" count
    | None ->
        printfn "Ошибка: файл не найден. Проверьте путь и попробуйте снова."
    0

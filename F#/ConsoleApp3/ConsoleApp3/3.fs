open System
open System.IO

/// Запрашивает ввод пути к каталогу и проверяет его существование
let rec getValidDirectory () =
    printf "Введите путь к каталогу: "
    let path = Console.ReadLine()
    if Directory.Exists(path) then path
    else
        printfn "Ошибка: указанного каталога не существует. Попробуйте снова."
        getValidDirectory ()

/// Запрашивает ввод имени файла
let getFileName () =
    printf "Введите имя файла для поиска: "
    Console.ReadLine()

/// Создаёт последовательность файлов в каталоге с отложенными вычислениями
let getFiles directory =
    seq {
        for file in Directory.EnumerateFiles(directory) do
            yield Path.GetFileName(file)
    }

/// Проверяет наличие файла в последовательности
let fileExists fileName fileSeq =
    fileSeq |> Seq.exists (fun file -> file = fileName)

[<EntryPoint>]
let main _ =
    let directory = getValidDirectory()
    let fileName = getFileName()
    let filesSeq = getFiles directory 
    
    if fileExists fileName filesSeq then
        printfn "Файл '%s' найден в каталоге '%s'" fileName directory
    else
        printfn "Файл '%s' отсутствует в каталоге '%s'" fileName directory
    
    0
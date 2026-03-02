#include <windows.h>   // Для работы с потоками, мьютексами и семафорами в WinAPI
#include <iostream>    // Для cout
#include <cstdio>      // Для работы с файлами через C-функции (fgetc, fputc)
#include <cctype>      // Для функций работы с символами (toupper)
#include <cstring>     // Для memset

using namespace std;

const int RAZMER_BUFERA = 50;       // Размер кольцевого буфера
const int KOLVO_SHIFROVSHIKOV = 4;  // Количество потоков-шифровальщиков

char bufer[RAZMER_BUFERA];          // Кольцевой буфер для передачи данных между потоками
int zapisano = 0;                   // Кол-во элементов, записанных, но ещё не обработанных шифровальщиками
int gotovo = 0;                      // Кол-во элементов, готовых к записи в файл
int pos_zapis = 0;                   // Индекс для записи в буфер
int pos_chten = 0;                   // Индекс для чтения из буфера

HANDLE semPustye;   // Семафор для отслеживания пустых ячеек буфера
HANDLE semSyrye;    // Семафор для отслеживания элементов, которые готовы для шифровальщика
HANDLE semGotovye;  // Семафор для отслеживания элементов, готовых к записи в файл
HANDLE mutex;       // Мьютекс для синхронизации доступа к буферу

FILE* failIn = NULL;   // Входной файл
FILE* failOut = NULL;  // Выходной файл

// Поток-чтение из файла
DWORD WINAPI PotokChitatel(LPVOID)
{
    char c;
    while ((c = fgetc(failIn)) != EOF)  // Чтение символов из файла до конца
    {
        WaitForSingleObject(semPustye, INFINITE); // Ждем, пока есть место в буфере
        WaitForSingleObject(mutex, INFINITE);     // Захватываем мьютекс для безопасной записи
        bufer[pos_zapis] = c;                     // Кладем символ в буфер
        pos_zapis = (pos_zapis + 1) % RAZMER_BUFERA; // Кольцевой переход
        zapisano++;                               // Увеличиваем счетчик "сырых" данных
        ReleaseMutex(mutex);                       // Освобождаем мьютекс
        ReleaseSemaphore(semSyrye, 1, NULL);      // Увеличиваем семафор "сырых" данных
    }
    // После окончания чтения разрешаем шифровальщикам завершить работу
    for (int i = 0; i < KOLVO_SHIFROVSHIKOV; i++)
    {
        ReleaseSemaphore(semSyrye, 1, NULL);
    }
    cout << "Чтение из файла завершено\n";
    return 0;
}

// Поток-шифровальщик
DWORD WINAPI PotokShifrovshik(LPVOID)
{
    while (true)
    {
        // Ждем, пока появится символ для шифрования, таймаут 5 секунд
        if (WaitForSingleObject(semSyrye, 5000) == WAIT_TIMEOUT)
        {
            break;  // Если данных нет долго, поток завершает работу
        }

        WaitForSingleObject(mutex, INFINITE); // Захват мьютекса для безопасной обработки буфера
        if (zapisano == 0)
        {
            ReleaseMutex(mutex);
            break;
        }
        int idx = (pos_chten + gotovo) % RAZMER_BUFERA; // Индекс текущего символа для шифрования
        char c = bufer[idx];
        c = toupper(c);                     // Переводим в верхний регистр
        if (c >= 'A' && c <= 'Z')
        {
            c = (c == 'Z') ? 'A' : c + 1;   // Цезарь-шифр +1 по алфавиту
        }
        bufer[idx] = c;                     // Записываем обратно в буфер
        gotovo++;                            // Увеличиваем количество готовых к записи
        zapisano--;                          // Уменьшаем количество "сырых" данных
        ReleaseMutex(mutex);                 // Освобождаем мьютекс
        ReleaseSemaphore(semGotovye, 1, NULL); // Сигнализируем писателю
    }
    return 0;
}

// Поток-запись в файл
DWORD WINAPI PotokPisatel(LPVOID)
{
    while (true)
    {
        // Ждем готовые символы к записи, таймаут 5 секунд
        if (WaitForSingleObject(semGotovye, 5000) == WAIT_TIMEOUT && gotovo == 0)
        {
            break; // Если ничего не готово, завершаем работу
        }
        WaitForSingleObject(mutex, INFINITE); // Захват мьютекса для безопасного чтения из буфера
        if (gotovo == 0)
        {
            ReleaseMutex(mutex);
            continue;
        }
        char c = bufer[pos_chten];                // Берем символ из буфера
        pos_chten = (pos_chten + 1) % RAZMER_BUFERA; // Кольцевой переход
        gotovo--;                                 // Уменьшаем количество готовых символов
        ReleaseMutex(mutex);                      // Освобождаем мьютекс
        fputc(c, failOut);                        // Записываем символ в файл
        ReleaseSemaphore(semPustye, 1, NULL);     // Освобождаем место для новых символов
    }
    cout << "Запись в файл завершена\n";
    return 0;
}

int main()
{
    setlocale(LC_ALL, "Russian");      // Для корректного вывода сообщений на русском
    memset(bufer, 0, sizeof(bufer));   // Очистка буфера
    cout << "Шифрование текста (семафоры)\n\n";

    // Открытие файлов
    errno_t e1 = fopen_s(&failIn, "input.txt", "r");
    errno_t e2 = fopen_s(&failOut, "output.txt", "w");
    if (e1 || !failIn || e2 || !failOut)
    {
        cout << "Ошибка с файлами!\n";
        return 1;
    }

    // Создание семафоров
    semPustye = CreateSemaphore(NULL, RAZMER_BUFERA, RAZMER_BUFERA, NULL); // Изначально все ячейки пусты
    semSyrye = CreateSemaphore(NULL, 0, RAZMER_BUFERA, NULL);              // Сырая продукция пока 0
    semGotovye = CreateSemaphore(NULL, 0, RAZMER_BUFERA, NULL);            // Готовые к записи пока 0
    mutex = CreateMutex(NULL, FALSE, NULL);                                 // Мьютекс для синхронизации

    // Создание потоков
    HANDLE h[6];
    h[0] = CreateThread(NULL, 0, PotokChitatel, NULL, 0, NULL); // Поток-чтение
    for (int i = 1; i <= KOLVO_SHIFROVSHIKOV; i++)
    {
        h[i] = CreateThread(NULL, 0, PotokShifrovshik, NULL, 0, NULL); // Потоки-шифровальщики
    }
    h[5] = CreateThread(NULL, 0, PotokPisatel, NULL, 0, NULL); // Поток-запись

    WaitForMultipleObjects(6, h, TRUE, INFINITE); // Ожидание завершения всех потоков

    // Закрытие всех дескрипторов и файлов
    for (int i = 0; i < 6; i++) CloseHandle(h[i]);
    CloseHandle(semPustye); CloseHandle(semSyrye); CloseHandle(semGotovye); CloseHandle(mutex);
    fclose(failIn); fclose(failOut);

    return 0;
}
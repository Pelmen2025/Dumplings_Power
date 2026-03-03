#include <windows.h>   // WinAPI: потоки, семафоры, мьютексы
#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

// Размер кольцевого буфера
const int M = 50;

// Количество потоков-обработчиков (worker)
const int N = 4;

// Кольцевой буфер
char bufer[M];

// Индекс записи (reader пишет сюда)
int in = 0;

// Индекс чтения для writer (финальная запись в файл)
int writer_out = 0;

// Индекс чтения для worker (дешифрование)
int out = 0;

// Количество символов в буфере, ожидающих обработки
int buf_count = 0;

// Количество уже обработанных (дешифрованных),
// но ещё не записанных в файл символов
int de_count = 0;

// Флаг завершения чтения файла
bool done_reading = false;

// Семафоры и мьютекс
HANDLE semEmpty; // количество свободных ячеек в буфере
HANDLE semFull;  // количество считанных, но ещё не обработанных символов
HANDLE semDone;  // количество обработанных символов
HANDLE mutex;    // защита общих переменных и буфера

FILE* fin;   // входной файл
FILE* fout;  // выходной файл

// ---------------- ПОТОК ЧТЕНИЯ (producer) ----------------
DWORD WINAPI reader(LPVOID)
{
    int c;

    // Читаем символы из файла до EOF
    while ((c = fgetc(fin)) != EOF)
    {
        // Ждём, пока есть свободное место в буфере
        WaitForSingleObject(semEmpty, INFINITE);

        // Захватываем мьютекс для работы с общими данными
        WaitForSingleObject(mutex, INFINITE);

        // Записываем символ в буфер
        bufer[in] = (char)c;

        // Сдвигаем индекс записи по кольцу
        in = (in + 1) % M;

        // Увеличиваем количество необработанных символов
        buf_count++;

        // Освобождаем мьютекс
        ReleaseMutex(mutex);

        // Увеличиваем счётчик semFull (появился новый символ для обработки)
        ReleaseSemaphore(semFull, 1, NULL);
    }

    // После окончания чтения
    WaitForSingleObject(mutex, INFINITE);
    done_reading = true;  // сигнал всем, что больше данных не будет
    ReleaseMutex(mutex);

    // Разблокируем worker-потоки (если они ждут semFull)
    ReleaseSemaphore(semFull, N, NULL);

    cout << "Чтение из файла завершено\n";
    return 0;
}

// ---------------- ПОТОК-ОБРАБОТЧИК (worker) ----------------
DWORD WINAPI worker(LPVOID)
{
    while (true)
    {
        // Ждём появления символа для обработки
        // Таймаут 3 секунды — защита от вечного ожидания
        if (WaitForSingleObject(semFull, 3000) == WAIT_TIMEOUT)
        {
            break;
        }

        WaitForSingleObject(mutex, INFINITE);

        // Если в буфере нет данных
        if (buf_count == 0)
        {
            // И чтение уже завершено — выходим
            if (done_reading)
            {
                ReleaseMutex(mutex);
                break;
            }

            ReleaseMutex(mutex);
            continue;
        }

        // Берём символ для дешифрования
        char ch = bufer[out];

        // Дешифрование:
        // Сдвиг буквы на -1 (циклически)
        if (ch >= 'A' && ch <= 'Z')
        {
            ch = (ch == 'A') ? 'Z' : ch - 1;
        }
        else if (ch >= 'a' && ch <= 'z')
        {
            ch = (ch == 'a') ? 'z' : ch - 1;
        }

        // Записываем дешифрованный символ обратно в буфер
        bufer[out] = ch;

        // Сдвигаем индекс чтения
        out = (out + 1) % M;

        // Уменьшаем количество необработанных
        buf_count--;

        // Увеличиваем количество обработанных
        de_count++;

        ReleaseMutex(mutex);

        // Сообщаем writer-потоку,
        // что появился готовый символ
        ReleaseSemaphore(semDone, 1, NULL);
    }

    return 0;
}

// ---------------- ПОТОК ЗАПИСИ (consumer) ----------------
DWORD WINAPI writer(LPVOID)
{
    while (true)
    {
        // Ждём появления обработанного символа
        if (WaitForSingleObject(semDone, 3000) == WAIT_TIMEOUT)
        {
            WaitForSingleObject(mutex, INFINITE);

            // Проверка: нет обработанных символов
            // и чтение завершено
            bool finished = (de_count == 0 && done_reading);

            ReleaseMutex(mutex);

            if (finished) break;

            continue;
        }

        WaitForSingleObject(mutex, INFINITE);

        // Берём готовый символ
        char ch = bufer[writer_out];

        // Сдвигаем индекс записи в файл
        writer_out = (writer_out + 1) % M;

        // Уменьшаем число обработанных символов
        de_count--;

        ReleaseMutex(mutex);

        // Записываем в выходной файл
        fputc(ch, fout);

        // Освобождаем одну ячейку буфера
        ReleaseSemaphore(semEmpty, 1, NULL);
    }

    cout << "Запись в файл завершена\n";
    return 0;
}

// ---------------- ГЛАВНАЯ ФУНКЦИЯ ----------------
int main()
{
    setlocale(LC_ALL, "Russian");

    cout << "Задание 3 - Дешифрование текста\n\n";

    // Обнуляем буфер
    memset(bufer, 0, sizeof(bufer));

    // Открываем входной файл
    if (fopen_s(&fin, "output.txt", "r") || !fin)
    {
        cout << "Нет файла output.txt\n";
        system("pause");
        return 1;
    }

    // Создаём выходной файл
    if (fopen_s(&fout, "decrypted.txt", "w") || !fout)
    {
        cout << "Не могу создать decrypted.txt\n";
        fclose(fin);
        system("pause");
        return 1;
    }

    // Инициализация семафоров
    semEmpty = CreateSemaphore(NULL, M, M, NULL); // изначально M свободных мест
    semFull = CreateSemaphore(NULL, 0, M, NULL); // изначально нет данных
    semDone = CreateSemaphore(NULL, 0, M, NULL); // изначально нет обработанных

    // Мьютекс для защиты критических секций
    mutex = CreateMutex(NULL, FALSE, NULL);

    // Массив дескрипторов потоков:
    // 1 reader + 4 worker + 1 writer = 6
    HANDLE h[6];

    h[0] = CreateThread(NULL, 0, reader, NULL, 0, NULL);

    for (int i = 1; i <= N; i++)
    {
        h[i] = CreateThread(NULL, 0, worker, NULL, 0, NULL);
    }

    h[5] = CreateThread(NULL, 0, writer, NULL, 0, NULL);

    // Ждём завершения всех потоков
    WaitForMultipleObjects(6, h, TRUE, INFINITE);

    // Закрываем дескрипторы потоков
    for (int i = 0; i < 6; i++)
        CloseHandle(h[i]);

    // Освобождаем объекты синхронизации
    CloseHandle(semEmpty);
    CloseHandle(semFull);
    CloseHandle(semDone);
    CloseHandle(mutex);

    fclose(fin);
    fclose(fout);

    return 0;
}
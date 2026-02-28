#include <windows.h>   // Работа с потоками, событиями и мьютексами WinAPI
#include <stdio.h>
#include <iostream>
#include <fstream>     // Работа с файлами
#include <locale.h>

using namespace std;

// --- Объекты синхронизации ---

HANDLE hEventBufferSvoboden;          // Событие: буфер свободен для записи
HANDLE hEventChitanieGotovo;          // Событие: чтение завершено
HANDLE hEventDeshifrovanieGotovo;     // Событие: дешифрование завершено
HANDLE hMutexDostupKBufferu;          // Мьютекс для защиты доступа к буферу

// --- Глобальные данные ---

const int buf_size = 20;              // Размер буфера
char buffer[buf_size];                // Общий буфер между потоками
char key[100];                        // Ключ шифрования
int k = 0;                            // Длина ключа
int dlina_buffer = 0;                 // Количество считанных байт
bool konecFayla = false;              // Флаг конца файла

// ===================== ПОТОК ЧТЕНИЯ =====================
DWORD WINAPI chitayFayl(LPVOID param)
{
    ifstream fayl("output.txt");  // Открываем входной файл

    if (!fayl)
    {
        cout << "Ошибка открытия файла\n";
        return 0;
    }

    while (true)
    {
        // Ждём, пока буфер станет свободным
        WaitForSingleObject(hEventBufferSvoboden, INFINITE);

        // Захватываем мьютекс (эксклюзивный доступ к buffer)
        WaitForSingleObject(hMutexDostupKBufferu, INFINITE);

        dlina_buffer = 0;

        // Читаем числа (коды символов) из файла в буфер
        while (dlina_buffer < buf_size && fayl.good())
        {
            int code;
            if (fayl >> code)
            {
                buffer[dlina_buffer] = (char)(unsigned char)code;
                dlina_buffer++;
            }
            else
            {
                break;
            }
        }

        // Если ничего не прочитали и достигнут конец файла
        if (dlina_buffer == 0 && fayl.eof())
        {
            konecFayla = true;  // Ставим флаг завершения
            ReleaseMutex(hMutexDostupKBufferu);
            SetEvent(hEventChitanieGotovo);  // Сообщаем следующему потоку
            break;
        }

        ReleaseMutex(hMutexDostupKBufferu);

        // Сообщаем потоку дешифрования, что данные готовы
        SetEvent(hEventChitanieGotovo);
    }

    fayl.close();
    return 0;
}

// ===================== ПОТОК ДЕШИФРОВАНИЯ =====================
DWORD WINAPI deshifruyText(LPVOID param)
{
    while (true)
    {
        // Ждём окончания чтения
        WaitForSingleObject(hEventChitanieGotovo, INFINITE);

        // Захватываем буфер
        WaitForSingleObject(hMutexDostupKBufferu, INFINITE);

        // Если файл закончился — завершаем поток
        if (konecFayla)
        {
            ReleaseMutex(hMutexDostupKBufferu);
            SetEvent(hEventDeshifrovanieGotovo);
            break;
        }

        int n = dlina_buffer;

        // Вычисляем количество блоков по длине ключа
        int blocks = (n + k - 1) / k;

        for (int b = 0; b < blocks; b++)
        {
            int start = b * k;
            int end = (b + 1) * k;

            if (end > n) end = n;

            // --- 1 этап: XOR с ключом ---
            for (int i = 0; i < end - start; i++)
            {
                buffer[start + i] ^= key[i];
            }

            // --- 2 этап: реверс блока ---
            for (int i = 0; i < (end - start) / 2; i++)
            {
                char temp = buffer[start + i];
                buffer[start + i] = buffer[end - 1 - i];
                buffer[end - 1 - i] = temp;
            }
        }

        ReleaseMutex(hMutexDostupKBufferu);

        // Сообщаем потоку записи, что данные готовы
        SetEvent(hEventDeshifrovanieGotovo);
    }

    return 0;
}

// ===================== ПОТОК ЗАПИСИ =====================
DWORD WINAPI pishiVFayl(LPVOID param)
{
    ofstream vyhod("deshifrovanny.txt", ios::binary);

    if (!vyhod)
    {
        cout << "Ошибка открытия выходного файла\n";
        return 0;
    }

    while (true)
    {
        // Ждём завершения дешифрования
        WaitForSingleObject(hEventDeshifrovanieGotovo, INFINITE);

        // Захватываем буфер
        WaitForSingleObject(hMutexDostupKBufferu, INFINITE);

        // Если достигнут конец файла — выходим
        if (konecFayla)
        {
            ReleaseMutex(hMutexDostupKBufferu);
            break;
        }

        // Записываем расшифрованные данные
        vyhod.write(buffer, dlina_buffer);

        ReleaseMutex(hMutexDostupKBufferu);

        // Освобождаем буфер для нового чтения
        SetEvent(hEventBufferSvoboden);
    }

    vyhod.close();
    return 0;
}

// ===================== MAIN =====================
int main()
{
    setlocale(LC_ALL, "Russian");

    cout << "Введите ключ: ";
    cin.getline(key, 100);

    k = strlen(key);

    if (k == 0)
    {
        cout << "Ключ пустой!\n";
        return 1;
    }

    // Создание событий (auto-reset)
    hEventBufferSvoboden = CreateEvent(NULL, FALSE, TRUE, NULL);
    hEventChitanieGotovo = CreateEvent(NULL, FALSE, FALSE, NULL);
    hEventDeshifrovanieGotovo = CreateEvent(NULL, FALSE, FALSE, NULL);

    // Создание мьютекса
    hMutexDostupKBufferu = CreateMutex(NULL, FALSE, NULL);

    // Создание потоков
    HANDLE hChitay = CreateThread(NULL, 0, chitayFayl, NULL, 0, NULL);
    HANDLE hDeshifr = CreateThread(NULL, 0, deshifruyText, NULL, 0, NULL);
    HANDLE hPishi = CreateThread(NULL, 0, pishiVFayl, NULL, 0, NULL);

    HANDLE potoki[3] = { hChitay, hDeshifr, hPishi };

    // Ожидаем завершения всех потоков
    WaitForMultipleObjects(3, potoki, TRUE, INFINITE);

    // Закрываем дескрипторы
    CloseHandle(hChitay);
    CloseHandle(hDeshifr);
    CloseHandle(hPishi);

    CloseHandle(hEventBufferSvoboden);
    CloseHandle(hEventChitanieGotovo);
    CloseHandle(hEventDeshifrovanieGotovo);
    CloseHandle(hMutexDostupKBufferu);

    cout << "Дешифрование завершено. Результат в deshifrovanny.txt\n";

    return 0;
}
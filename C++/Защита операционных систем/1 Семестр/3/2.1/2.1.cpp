#include <windows.h>   // WinAPI (потоки, события, мьютексы)
#include <stdio.h>
#include <iostream>
#include <fstream>     // Работа с файлами
#include <locale.h>    // Локализация

using namespace std;

// --- Объекты синхронизации ---

HANDLE hEventBufferSvoboden;        // Событие: буфер свободен для записи
HANDLE hEventChitanieGotovo;        // Событие: чтение из файла завершено
HANDLE hEventShifrovanieGotovo;     // Событие: шифрование завершено
HANDLE hMutexDostupKBufferu;        // Мьютекс для защиты доступа к буферу

// --- Глобальные данные ---

const int buf_size = 20;            // Размер буфера
char buffer[buf_size];              // Общий буфер (разделяется между потоками)
char key[100];                      // Ключ шифрования
int k = 0;                          // Длина ключа
int dlina_buffer = 0;               // Реальное количество считанных байт
bool konecFayla = false;            // Флаг конца файла


// ==================== ПОТОК 1: ЧТЕНИЕ ФАЙЛА ====================

DWORD WINAPI chitayFayl(LPVOID param)
{
    ifstream fayl("input.txt", ios::binary);  // Открываем файл в бинарном режиме

    if (!fayl)
    {
        cout << "Ошибка открытия файла\n";
        return 0;
    }

    while (true)
    {
        // Ждём, пока буфер освободится
        WaitForSingleObject(hEventBufferSvoboden, INFINITE);

        // Захватываем мьютекс (эксклюзивный доступ к buffer)
        WaitForSingleObject(hMutexDostupKBufferu, INFINITE);

        // Читаем данные в буфер
        fayl.read(buffer, buf_size);

        // Получаем реальное количество прочитанных байт
        dlina_buffer = fayl.gcount();

        // Если ничего не прочитано — конец файла
        if (dlina_buffer == 0)
        {
            konecFayla = true;

            ReleaseMutex(hMutexDostupKBufferu);

            // Сигнал потоку шифрования
            SetEvent(hEventChitanieGotovo);

            break;
        }

        ReleaseMutex(hMutexDostupKBufferu);

        // Сообщаем, что чтение завершено
        SetEvent(hEventChitanieGotovo);
    }

    fayl.close();
    return 0;
}


// ==================== ПОТОК 2: ШИФРОВАНИЕ ====================

DWORD WINAPI shifruyText(LPVOID param)
{
    while (true)
    {
        // Ждём сигнал от чтения
        WaitForSingleObject(hEventChitanieGotovo, INFINITE);

        // Захватываем мьютекс
        WaitForSingleObject(hMutexDostupKBufferu, INFINITE);

        // Если файл закончился — завершаем работу
        if (konecFayla)
        {
            ReleaseMutex(hMutexDostupKBufferu);

            // Сообщаем потоку записи о завершении
            SetEvent(hEventShifrovanieGotovo);

            break;
        }

        int n = dlina_buffer;

        // Количество блоков длиной k
        int blocks = (n + k - 1) / k;

        for (int b = 0; b < blocks; b++)
        {
            int start = b * k;          // Начало блока
            int end = (b + 1) * k;      // Конец блока

            if (end > n) end = n;

            // --- 1. Разворот блока ---
            for (int i = 0; i < (end - start) / 2; i++)
            {
                char temp = buffer[start + i];
                buffer[start + i] = buffer[end - 1 - i];
                buffer[end - 1 - i] = temp;
            }

            // --- 2. XOR с ключом ---
            for (int i = 0; i < end - start; i++)
            {
                buffer[start + i] ^= key[i];
            }
        }

        ReleaseMutex(hMutexDostupKBufferu);

        // Сообщаем потоку записи
        SetEvent(hEventShifrovanieGotovo);
    }

    return 0;
}


// ==================== ПОТОК 3: ЗАПИСЬ В ФАЙЛ ====================

DWORD WINAPI pishiVFayl(LPVOID param)
{
    ofstream vyhod("output.txt");

    if (!vyhod)
    {
        cout << "Ошибка открытия выходного файла\n";
        return 0;
    }

    while (true)
    {
        // Ждём завершения шифрования
        WaitForSingleObject(hEventShifrovanieGotovo, INFINITE);

        // Захватываем мьютекс
        WaitForSingleObject(hMutexDostupKBufferu, INFINITE);

        if (konecFayla)
        {
            ReleaseMutex(hMutexDostupKBufferu);
            break;
        }

        // Записываем байты как числа (0–255)
        for (int i = 0; i < dlina_buffer; i++)
        {
            vyhod << (int)(unsigned char)buffer[i] << " ";
        }

        ReleaseMutex(hMutexDostupKBufferu);

        // Сообщаем, что буфер снова свободен
        SetEvent(hEventBufferSvoboden);
    }

    vyhod.close();
    return 0;
}


// ==================== MAIN ====================

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

    // Создание событий
    hEventBufferSvoboden = CreateEvent(NULL, FALSE, TRUE, NULL);
    // FALSE — авто-сброс
    // TRUE — изначально буфер свободен

    hEventChitanieGotovo = CreateEvent(NULL, FALSE, FALSE, NULL);
    hEventShifrovanieGotovo = CreateEvent(NULL, FALSE, FALSE, NULL);

    // Создание мьютекса
    hMutexDostupKBufferu = CreateMutex(NULL, FALSE, NULL);

    // Создание потоков
    HANDLE hChitay = CreateThread(NULL, 0, chitayFayl, NULL, 0, NULL);
    HANDLE hShifr = CreateThread(NULL, 0, shifruyText, NULL, 0, NULL);
    HANDLE hPishi = CreateThread(NULL, 0, pishiVFayl, NULL, 0, NULL);

    HANDLE potoki[3] = { hChitay, hShifr, hPishi };

    // Ждём завершения всех потоков
    WaitForMultipleObjects(3, potoki, TRUE, INFINITE);

    // Освобождаем ресурсы
    CloseHandle(hChitay);
    CloseHandle(hShifr);
    CloseHandle(hPishi);

    CloseHandle(hEventBufferSvoboden);
    CloseHandle(hEventChitanieGotovo);
    CloseHandle(hEventShifrovanieGotovo);
    CloseHandle(hMutexDostupKBufferu);

    cout << "Шифрование завершено. Результат в output.txt\n";

    return 0;
}

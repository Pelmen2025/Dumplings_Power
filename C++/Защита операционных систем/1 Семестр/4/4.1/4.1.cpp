#include <windows.h>   // Потоки, семафоры, мьютексы (WinAPI)
#include <iostream>    // cout
#include <cstdio>      // fopen_s, fgets, fputc
#include <cstring>     // strlen, memcpy, memmove

using namespace std;

// Размер общего буфера (пула)
const int n = 1024;

// Количество потоков-производителей (печати)
const int m = 3;

// Общий буфер (пул), куда потоки пишут символы
char pul[n];

// Сколько свободных байт в буфере
int svobodno = n;

// Сколько байт занято (заполнено)
int zapolneno = 0;

// Семафор свободных мест
HANDLE semSvobodnye;

// Семафор заполненных мест
HANDLE semZapolnennye;

// Мьютекс для защиты доступа к буферу
HANDLE mutexPul;

// Выходной файл (куда записывает поток управления)
FILE* vyhodnoj_fail;

// -------------------------
// Поток-производитель
// -------------------------
DWORD WINAPI potok_pechati(LPVOID param)
{
    // Получаем номер потока (1, 2, 3...)
    intptr_t nomer_ptr = (intptr_t)param;
    int nomer = (int)nomer_ptr;

    // Формируем имя входного файла: input1.txt, input2.txt...
    char imya_faila[20];
    sprintf_s(imya_faila, sizeof(imya_faila), "input%d.txt", nomer);

    FILE* vhod_fail = NULL;

    // Открываем входной файл
    errno_t err = fopen_s(&vhod_fail, imya_faila, "r");
    if (err != 0 || !vhod_fail)
    {
        cout << "Не могу открыть " << imya_faila << endl;
        return 0;
    }

    char stroka[100];

    // Читаем файл построчно
    while (fgets(stroka, sizeof(stroka), vhod_fail))
    {
        char* ptr = stroka;               // указатель на текущую позицию строки
        int dlina = (int)strlen(stroka);  // длина строки

        // Захватываем мьютекс перед работой с буфером
        WaitForSingleObject(mutexPul, INFINITE);

        while (dlina > 0)
        {
            // Если нет свободного места — ждём
            while (svobodno < 1)
            {
                ReleaseMutex(mutexPul); // освобождаем мьютекс
                WaitForSingleObject(semSvobodnye, INFINITE); // ждём свободное место
                WaitForSingleObject(mutexPul, INFINITE); // снова захватываем мьютекс
            }

            // Сколько реально можем записать
            int mogu_zapisat = (dlina < svobodno) ? dlina : svobodno;

            // Копируем данные в буфер
            memcpy(pul + (n - svobodno), ptr, mogu_zapisat);

            // Обновляем счётчики
            svobodno -= mogu_zapisat;
            zapolneno += mogu_zapisat;

            dlina -= mogu_zapisat;
            ptr += mogu_zapisat;

            // Увеличиваем семафор заполненных байт
            ReleaseSemaphore(semZapolnennye, mogu_zapisat, NULL);
        }

        // Освобождаем мьютекс
        ReleaseMutex(mutexPul);
    }

    fclose(vhod_fail);

    cout << "Поток печати " << nomer << " завершился\n";
    return 0;
}

// -------------------------
// Поток-потребитель
// -------------------------
DWORD WINAPI potok_upravleniya(LPVOID)
{
    while (true)
    {
        // Ждём хотя бы один заполненный байт
        WaitForSingleObject(semZapolnennye, INFINITE);

        // Захватываем мьютекс для доступа к буферу
        WaitForSingleObject(mutexPul, INFINITE);

        if (zapolneno == 0)
        {
            ReleaseMutex(mutexPul);
            continue;
        }

        // Берём первый символ из буфера
        char c = pul[0];

        // Пишем его в выходной файл
        fputc(c, vyhodnoj_fail);

        // Сдвигаем буфер влево (удаляем первый символ)
        memmove(pul, pul + 1, zapolneno - 1);

        zapolneno--;
        svobodno++;

        // Увеличиваем количество свободных мест
        ReleaseSemaphore(semSvobodnye, 1, NULL);

        ReleaseMutex(mutexPul);
    }

    return 0;
}

// -------------------------
// Главная функция
// -------------------------
int main()
{
    setlocale(LC_ALL, "Russian");

    cout << "Объем пула: " << n << ", потоков печати: " << m << endl << endl;

    // Создаём выходной файл
    errno_t err_out = fopen_s(&vyhodnoj_fail, "output_print.txt", "w");
    if (err_out != 0 || !vyhodnoj_fail)
    {
        cout << "Не могу создать output_print.txt\n";
        return 1;
    }

    // Создаём семафоры:
    // semSvobodnye — изначально n свободных мест
    semSvobodnye = CreateSemaphore(NULL, n, n, NULL);

    // semZapolnennye — изначально 0 заполненных мест
    semZapolnennye = CreateSemaphore(NULL, 0, n, NULL);

    // Создаём мьютекс
    mutexPul = CreateMutex(NULL, FALSE, NULL);

    // Массив потоков (m производителей + 1 потребитель)
    HANDLE potoki[m + 1];

    // Создаём потоки-производители
    for (int i = 0; i < m; i++)
    {
        potoki[i] = CreateThread(
            NULL,
            0,
            potok_pechati,
            (LPVOID)(intptr_t)(i + 1),
            0,
            NULL
        );
    }

    // Создаём поток-потребитель
    potoki[m] = CreateThread(
        NULL,
        0,
        potok_upravleniya,
        NULL,
        0,
        NULL
    );

    // Ждём завершения только потоков-производителей
    WaitForMultipleObjects(m, potoki, TRUE, INFINITE);

    cout << "Все потоки печати завершились. Завершаем программу.\n";

    // Закрываем дескрипторы потоков
    for (int i = 0; i < m + 1; i++)
    {
        CloseHandle(potoki[i]);
    }

    // Закрываем синхронизирующие объекты
    CloseHandle(semSvobodnye);
    CloseHandle(semZapolnennye);
    CloseHandle(mutexPul);

    fclose(vyhodnoj_fail);

    return 0;
}
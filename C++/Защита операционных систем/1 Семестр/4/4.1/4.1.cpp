#include <windows.h>   // WinAPI: потоки, мьютексы, семафоры
#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

// Размер общего буфера (пула)
const int n = 1024;

// Количество потоков-производителей
const int m = 3;

// Общий буфер (пул)
char pul[n];

// Сколько свободного места в буфере
int svobodno = n;

// Сколько символов занято (заполнено)
int zapolnеnо = 0;

// Семафор свободных ячеек
HANDLE semSvobodnye;

// Семафор заполненных ячеек
HANDLE semZapolnennye;

// Мьютекс для защиты доступа к буферу
HANDLE mutexPul;

// Указатель на выходной файл
FILE* vyhodnoj_fail;


// ==========================
// Поток-производитель
// Читает файл inputX.txt и кладет данные в буфер
// ==========================
DWORD WINAPI potok_pechati(LPVOID param)
{
    // Получаем номер потока
    intptr_t nomer_ptr = (intptr_t)param;
    int nomer = (int)nomer_ptr;

    // Формируем имя файла: input1.txt, input2.txt ...
    char imya_faila[20];
    sprintf_s(imya_faila, sizeof(imya_faila), "input%d.txt", nomer);

    FILE* vhod_fail = NULL;
    errno_t err = fopen_s(&vhod_fail, imya_faila, "r");

    // Проверка открытия файла
    if (err != 0 || !vhod_fail)
    {
        cout << "Не могу открыть " << imya_faila << endl;
        return 0;
    }

    char stroka[100];

    // Читаем файл построчно
    while (fgets(stroka, sizeof(stroka), vhod_fail))
    {
        char* ptr = stroka;               // указатель на текущую позицию в строке
        int dlina = (int)strlen(stroka);  // длина строки

        // Захватываем мьютекс (входим в критическую секцию)
        WaitForSingleObject(mutexPul, INFINITE);

        // Пока вся строка не записана в буфер
        while (dlina > 0)
        {
            // Если нет свободного места
            while (svobodno < 1)
            {
                // Освобождаем мьютекс
                ReleaseMutex(mutexPul);

                // Ждём, пока появится свободное место
                WaitForSingleObject(semSvobodnye, INFINITE);

                // Снова захватываем мьютекс
                WaitForSingleObject(mutexPul, INFINITE);
            }

            // Сколько можем записать за раз
            int mogu_zapisat = (dlina < svobodno) ? dlina : svobodno;

            // Копируем данные в буфер
            memcpy(pul + (n - svobodno), ptr, mogu_zapisat);

            // Обновляем счётчики
            svobodno -= mogu_zapisat;
            zapolnеnо += mogu_zapisat;

            dlina -= mogu_zapisat;
            ptr += mogu_zapisat;

            // Сообщаем, что появились заполненные элементы
            ReleaseSemaphore(semZapolnennye, mogu_zapisat, NULL);
        }

        // Освобождаем мьютекс
        ReleaseMutex(mutexPul);
    }

    fclose(vhod_fail);
    cout << "Поток печати " << nomer << " завершился\n";
    return 0;
}


// ==========================
// Поток-потребитель
// Берёт символы из буфера и пишет в output_print.txt
// ==========================
DWORD WINAPI potok_upravleniya(LPVOID)
{
    while (true)
    {
        // Ждём, пока появится хотя бы один заполненный элемент
        WaitForSingleObject(semZapolnennye, INFINITE);

        // Захватываем мьютекс
        WaitForSingleObject(mutexPul, INFINITE);

        // Если вдруг буфер пуст
        if (zapolnеnо == 0)
        {
            ReleaseMutex(mutexPul);
            continue;
        }

        // Берём первый символ из буфера
        char c = pul[0];

        // Записываем его в выходной файл
        fputc(c, vyhodnoj_fail);

        // Сдвигаем буфер влево (удаляем первый символ)
        memmove(pul, pul + 1, zapolnеnо - 1);

        // Обновляем счётчики
        zapolnеnо--;
        svobodno++;

        // Сообщаем, что появилась 1 свободная ячейка
        ReleaseSemaphore(semSvobodnye, 1, NULL);

        // Освобождаем мьютекс
        ReleaseMutex(mutexPul);
    }

    return 0;
}


// ==========================
// Главная функция
// ==========================
int main()
{
    setlocale(LC_ALL, "Russian");

    cout << "Объем пула: " << n
        << ", потоков печати: " << m << endl << endl;

    // Открываем выходной файл
    errno_t err_out = fopen_s(&vyhodnoj_fail, "output_print.txt", "w");

    if (err_out != 0 || !vyhodnoj_fail)
    {
        cout << "Не могу создать output_print.txt\n";
        return 1;
    }

    // Создаём семафор свободных ячеек
    // Начальное значение = n (всё свободно)
    semSvobodnye = CreateSemaphore(NULL, n, n, NULL);

    // Семафор заполненных ячеек
    // Начальное значение = 0 (пусто)
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

    // Закрываем семафоры и мьютекс
    CloseHandle(semSvobodnye);
    CloseHandle(semZapolnennye);
    CloseHandle(mutexPul);

    fclose(vyhodnoj_fail);

    return 0;
}
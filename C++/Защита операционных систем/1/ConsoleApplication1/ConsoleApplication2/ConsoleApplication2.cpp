#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <locale.h>

using namespace std;

const int m = 10, n = 10;
float mtx[m][n];       // матрица
float row_avgs[m];     // массив средних значений строк
int row_numbers[m];    // номера строк
int col_numbers[n];    // номера столбцов

// Функция потока: создаёт строку матрицы
DWORD WINAPI create_row(LPVOID param)
{
    int* prow_num = (int*)param;   // получаем номер строки
    int row_num = *prow_num;

    // Уникальная инициализация генератора для каждого потока
    srand(time(NULL) + GetCurrentThreadId() + row_num * 13);

    for (int j = 0; j < n; j++)
        mtx[row_num][j] = (float)(rand() % 100 + 1);  // от 1 до 100

    return 0;
}

// Функция для подсчёта среднего значения строки
DWORD WINAPI row_avg(LPVOID param)
{
    int* prow_num = (int*)param;
    int row_num = *prow_num;

    float sum = 0;
    for (int j = 0; j < n; j++)
        sum += mtx[row_num][j];

    row_avgs[row_num] = sum / n;  // среднее арифметическое

    return 0;
}

int main()
{
    setlocale(LC_ALL, "RUS");

    // Инициализируем основной генератор
    srand(time(NULL));

    // --- ЭТАП 1: создание строк матрицы в потоках ---
    HANDLE hRowThread[m];
    DWORD dwRowThreadID[m];

    cout << "Создание матрицы..." << endl;

    for (int i = 0; i < m; i++)
    {
        row_numbers[i] = i;
        hRowThread[i] = CreateThread(
            NULL,
            0,
            create_row,
            &row_numbers[i],
            0,
            &dwRowThreadID[i]
        );

        if (hRowThread[i] == NULL)
        {
            cout << "Поток строки № " << i
                << " не был создан\nОшибка: "
                << GetLastError() << endl;
        }
    }

    // ждём окончания заполнения строк
    WaitForMultipleObjects(m, hRowThread, TRUE, INFINITE);

    for (int i = 0; i < m; i++)
        CloseHandle(hRowThread[i]);

    cout << "Матрица создана." << endl;

    // Вывод матрицы для проверки (все элементы)
    cout << "\nСозданная матрица:" << endl;
    for (int i = 0; i < m; i++)
    {
        cout << "Строка " << i << ": ";
        for (int j = 0; j < n; j++)
        {
            cout << mtx[i][j] << " ";
        }
        cout << endl;
    }

    // --- ЭТАП 2: подсчёт средних значений строк в потоках ---
    HANDLE hAvgThread[m];
    DWORD dwAvgThreadID[m];

    cout << "\nВычисление средних значений строк..." << endl;

    // Запускаем потоки для вычисления средних значений ПОСЛЕ заполнения матрицы
    for (int i = 0; i < m; i++)
    {
        hAvgThread[i] = CreateThread(
            NULL,
            0,
            row_avg,
            &row_numbers[i],
            0,
            &dwAvgThreadID[i]
        );

        if (hAvgThread[i] == NULL)
        {
            cout << "Поток вычисления среднего для строки № " << i
                << " не был создан\nОшибка: "
                << GetLastError() << endl;
        }
    }

    // Ждем завершения всех потоков вычисления средних
    WaitForMultipleObjects(m, hAvgThread, TRUE, INFINITE);

    for (int i = 0; i < m; i++)
        CloseHandle(hAvgThread[i]);

    // Вывод средних значений
    cout << "\nСредние значения строк:" << endl;
    for (int i = 0; i < m; i++)
    {
        cout << "Строка " << i << ": " << row_avgs[i] << endl;
    }

    // --- ЭТАП 3: поиск строки с максимальным средним значением ---
    int max_row_num = 0;
    float max_avg = row_avgs[0];

    for (int i = 1; i < m; i++)
    {
        if (row_avgs[i] > max_avg)
        {
            max_avg = row_avgs[i];
            max_row_num = i;
        }
    }

    cout << "\n=== РЕЗУЛЬТАТ ===" << endl;
    cout << "Строка с максимальным средним значением: № " << max_row_num
        << " (среднее значение = " << max_avg << ")" << endl;

    return 0;
}
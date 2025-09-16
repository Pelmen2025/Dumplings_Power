#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <locale.h>

using namespace std;

const int m = 10, n = 20;
float mtx[m][n];       // матрица
float col_sums[n];     // массив сумм
int row_numbers[m];    // номера строк

// Функция потока: создаёт строку матрицы
DWORD WINAPI create_row(LPVOID param)
{
    int* prow_num = (int*)param;   // получаем номер строки
    int row_num = *prow_num;

    for (int j = 0; j < n; j++)
        mtx[row_num][j] = (float)(rand() % 100);

    return 0;
}

// Функция для подсчёта суммы столбца
DWORD WINAPI col_sum(LPVOID param)
{
    int* pcol_num = (int*)param;
    int col_num = *pcol_num;

    col_sums[col_num] = 0;
    for (int i = 0; i < m; i++)
        col_sums[col_num] += mtx[i][col_num];

    return 0;
}

int main()
{
    setlocale(LC_ALL, "RUS");

    // --- ЭТАП 1: создание строк матрицы в потоках ---
    HANDLE hRowThread[m];
    DWORD dwRowThreadID[m];

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

    // --- ЭТАП 2: подсчёт сумм столбцов (как было в примере) ---
    HANDLE hColThread[n];
    DWORD dwColThreadID[n];
    int col_numbers[n];

    for (int i = 0; i < n; i++)
    {
        col_sums[i] = 0;
        col_numbers[i] = i;

        hColThread[i] = CreateThread(
            NULL,
            0,
            col_sum,
            &col_numbers[i],
            0,
            &dwColThreadID[i]
        );
    }

    WaitForMultipleObjects(n, hColThread, TRUE, INFINITE);

    for (int i = 0; i < n; i++)
        CloseHandle(hColThread[i]);

    // --- ЭТАП 3: поиск минимальной суммы ---
    int num_min = 0;
    float min = col_sums[0];
    for (int i = 1; i < n; i++)
    {
        if (col_sums[i] < min)
        {
            min = col_sums[i];
            num_min = i;
        }
    }

    cout << "Столбец с минимальной суммой: № " << num_min
        << " (сумма = " << min << ")" << endl;

    return 0;
}

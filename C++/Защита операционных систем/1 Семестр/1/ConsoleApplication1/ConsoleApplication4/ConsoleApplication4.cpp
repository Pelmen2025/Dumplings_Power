#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <locale.h>
using namespace std;

const int N = 4;
float mtx[N][N];           // исходная матрица
float minors[N];           // миноры для разложения по первой строке
int col_indices[N];        // индексы столбцов для потоков

// Функция для вычисления определителя 3×3
float det3x3(float m[3][3])
{
    return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) -
        m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
        m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
}

// Функция потока - вычисляет минор для заданного столбца
DWORD WINAPI calculate_minor(LPVOID param)
{
    int* pcol = (int*)param;
    int col = *pcol;

    // Создаем подматрицу 3×3 (вычеркиваем первую строку и col-й столбец)
    float submatrix[3][3];
    int sub_i = 0;

    for (int i = 1; i < N; i++)  // начинаем со второй строки
    {
        int sub_j = 0;
        for (int j = 0; j < N; j++)
        {
            if (j != col)  // пропускаем столбец col
            {
                submatrix[sub_i][sub_j] = mtx[i][j];
                sub_j++;
            }
        }
        sub_i++;
    }

    // Вычисляем определитель 3×3 и сохраняем с учетом знака
    float sign = (col % 2 == 0) ? 1.0f : -1.0f;
    minors[col] = sign * mtx[0][col] * det3x3(submatrix);

    return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
    setlocale(LC_ALL, "RUS");

    // Описание переменных для работы с потоками
    HANDLE hThread[N];
    DWORD dwThreadID[N];

    // Заполнение матрицы 4×4 заданными значениями
    float values[N][N] = {
        {9, 3, 4, 10},
        {1, 6, 8, 8},
        {8, 6, 5, 5},
        {9, 3, 9, 1}
    };

    cout << "Исходная матрица 4 на 4:\n";
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            mtx[i][j] = values[i][j];
            cout.width(6);
            cout << mtx[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    // Инициализация массивов
    for (int i = 0; i < N; i++)
    {
        minors[i] = 0;
        col_indices[i] = i;
    }

    // Запуск потоков для вычисления миноров
    for (int i = 0; i < N; i++)
    {
        hThread[i] = CreateThread(
            NULL,
            0,
            calculate_minor,
            &col_indices[i],
            0,
            &dwThreadID[i]
        );

        if (hThread[i] == NULL)
        {
            cout << "Поток № " << i << " не был создан\n"
                << "Ошибка: " << GetLastError() << endl;
            return 1;
        }
    }

    // Ожидание завершения всех потоков
    WaitForMultipleObjects(N, hThread, TRUE, INFINITE);

    // Суммируем все миноры для получения определителя
    float determinant = 0;
    for (int i = 0; i < N; i++)
    {
        determinant += minors[i];
    }

    // Вывод результата
    cout << "Определитель матрицы = " << determinant << endl;

    // Закрытие потоков
    for (int i = 0; i < N; i++)
        CloseHandle(hThread[i]);

    return 0;
}
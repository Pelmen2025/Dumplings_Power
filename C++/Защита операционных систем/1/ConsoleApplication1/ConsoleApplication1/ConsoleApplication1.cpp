#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <locale.h>
#include <iomanip>
#include <ctime>
using namespace std;

const int m = 10, n = 10;
float mtx[m][n];       // матрица
float col_sums[n];     // массив сумм
int col_numbers[n];    // номера столбцов
int row_numbers[m];    // номера строк

// Функция потока для создания строки матрицы
DWORD WINAPI create_row(LPVOID param)
{
    // Получаем значение параметра — номер строки
    int* prow_num = (int*)param;
    int row_num = *prow_num;

    // Уникальная инициализация генератора для каждого потока
    srand(time(NULL) + GetCurrentThreadId() + row_num * 13);

    // Небольшая задержка для дополнительной рандомизации
    Sleep(row_num * 2);

    // Создаем строку матрицы
    for (int j = 0; j < n; j++)
        mtx[row_num][j] = (float)(rand() % 100 + 1); // от 1 до 100

    return 0;
}

// Функция потока для вычисления суммы столбца
DWORD WINAPI col_sum(LPVOID param)
{
    // Получаем значение параметра — номер столбца
    int* pcol_num = (int*)param;
    int col_num = *pcol_num;

    // Находим искомую сумму
    col_sums[col_num] = 0;
    for (int i = 0; i < m; i++)
        col_sums[col_num] += mtx[i][col_num];

    return 0;
}

// Функция для вывода матрицы
void print_matrix()
{
    cout << "\nСозданная матрица:" << endl;
    for (int i = 0; i < m; i++)
    {
        cout << "Строка " << i << ": ";
        for (int j = 0; j < n; j++)
        {
            cout << (int)mtx[i][j] << " ";
        }
        cout << endl;
    }
}

int _tmain(int argc, _TCHAR* argv[])
{
    setlocale(LC_ALL, "RUS");

    // Инициализируем основной генератор
    srand(time(NULL));

    // Описание переменных для работы с потоками
    HANDLE hRowThread[m];    // массив дескрипторов потоков для строк
    HANDLE hColThread[n];    // массив дескрипторов потоков для столбцов
    DWORD dwRowThreadID[m];  // массив идентификаторов потоков для строк
    DWORD dwColThreadID[n];  // массив идентификаторов потоков для столбцов

    // Заполнение массивов номеров
    for (int i = 0; i < m; i++)
        row_numbers[i] = i;

    for (int i = 0; i < n; i++)
    {
        col_sums[i] = 0;
        col_numbers[i] = i;
    }

    cout << "Создание матрицы параллельно по строкам...\n";

    // Параллельное создание строк матрицы
    for (int i = 0; i < m; i++)
    {
        hRowThread[i] = CreateThread(
            NULL,                  // атрибуты безопасности по умолчанию
            0,                     // размер стека по умолчанию
            create_row,            // функция потока
            &row_numbers[i],       // параметр (номер строки)
            0,                     // флаги создания
            &dwRowThreadID[i]      // идентификатор потока
        );

        // Проверка — создан ли поток
        if (hRowThread[i] == NULL)
        {
            cout << "Поток для строки № " << i
                << " не был создан\n"
                << "Ошибка: " << GetLastError() << endl;
        }
    }

    // Ожидание завершения всех потоков создания строк
    WaitForMultipleObjects(m, hRowThread, TRUE, INFINITE);

    cout << "Создание матрицы завершено.\n";

    // Вывод созданной матрицы
    print_matrix();

    cout << "\nВычисление сумм столбцов параллельно...\n";

    // Параллельное вычисление сумм столбцов
    for (int i = 0; i < n; i++)
    {
        hColThread[i] = CreateThread(
            NULL,                  // атрибуты безопасности по умолчанию
            0,                     // размер стека по умолчанию
            col_sum,               // функция потока
            &col_numbers[i],       // параметр (номер столбца)
            0,                     // флаги создания
            &dwColThreadID[i]      // идентификатор потока
        );

        // Проверка — создан ли поток
        if (hColThread[i] == NULL)
        {
            cout << "Поток для столбца № " << i
                << " не был создан\n"
                << "Ошибка: " << GetLastError() << endl;
        }
    }

    // Ожидание завершения всех потоков вычисления сумм
    WaitForMultipleObjects(n, hColThread, TRUE, INFINITE);

    // Вывод сумм столбцов
    cout << "\nСуммы столбцов:" << endl;
    for (int j = 0; j < n; j++)
    {
        cout << "Столбец " << j << ": " << (int)col_sums[j] << endl;
    }

    // Находим номер столбца с минимальной суммой
    int num_min = 0;
    float min = col_sums[0];
    for (int i = 1; i < n; i++)
    {
        if (min > col_sums[i])
        {
            min = col_sums[i];
            num_min = i;
        }
    }

    // Вывод результата
    cout << "\nИскомый столбец № " << num_min
        << " с минимальной суммой = " << min << endl;

    // Закрытие потоков
    for (int i = 0; i < m; i++)
        CloseHandle(hRowThread[i]);

    for (int i = 0; i < n; i++)
        CloseHandle(hColThread[i]);

    return 0;
}
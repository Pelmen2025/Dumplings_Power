#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <locale.h>
#include <iomanip>
#include <ctime>
using namespace std;

const int n = 10, m = 8; // n строк, m столбцов
float mtx[n][m];       // матрица
int row_numbers[n];    // номера строк для создания
int col_numbers[m];    // номера столбцов для сортировки

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
    for (int j = 0; j < m; j++)
        mtx[row_num][j] = (float)(rand() % 100 + 1); // от 1 до 100

    return 0;
}

// Функция потока для сортировки столбца методом обмена (пузырьковая сортировка)
DWORD WINAPI sort_column(LPVOID param)
{
    // Получаем номер столбца для сортировки
    int* pcol_num = (int*)param;
    int col_num = *pcol_num;

    // Сортировка столбца методом обмена по возрастанию
    for (int i = 0; i < n - 1; i++)
    {
        for (int j = 0; j < n - 1 - i; j++)
        {
            if (mtx[j][col_num] > mtx[j + 1][col_num])
            {
                // Обмен элементов
                float temp = mtx[j][col_num];
                mtx[j][col_num] = mtx[j + 1][col_num];
                mtx[j + 1][col_num] = temp;
            }
        }
    }

    return 0;
}

// Функция для вывода матрицы
void print_matrix(const char* title)
{
    cout << "\n" << title << ":" << endl;
    for (int i = 0; i < n; i++)
    {
        cout << "Строка " << i << ": ";
        for (int j = 0; j < m; j++)
        {
            cout << setw(3) << (int)mtx[i][j] << " ";
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
    HANDLE hRowThread[n];    // массив дескрипторов потоков для строк
    HANDLE hColThread[m];    // массив дескрипторов потоков для столбцов
    DWORD dwRowThreadID[n];  // массив идентификаторов потоков для строк
    DWORD dwColThreadID[m];  // массив идентификаторов потоков для столбцов

    // Заполнение массивов номеров
    for (int i = 0; i < n; i++)
        row_numbers[i] = i;

    for (int j = 0; j < m; j++)
        col_numbers[j] = j;

    cout << "Создание матрицы параллельно по строкам...\n";

    // === ЭТАП 1: Параллельное создание строк матрицы ===
    for (int i = 0; i < n; i++)
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
    WaitForMultipleObjects(n, hRowThread, TRUE, INFINITE);

    // Закрытие дескрипторов потоков создания строк
    for (int i = 0; i < n; i++)
        CloseHandle(hRowThread[i]);

    cout << "Создание матрицы завершено.\n";

    // Вывод исходной матрицы
    print_matrix("Исходная матрица");

    cout << "\nСортировка столбцов параллельно методом обмена...\n";

    // === ЭТАП 2: Параллельная сортировка столбцов ===
    for (int j = 0; j < m; j++)
    {
        hColThread[j] = CreateThread(
            NULL,                  // атрибуты безопасности по умолчанию
            0,                     // размер стека по умолчанию
            sort_column,           // функция потока
            &col_numbers[j],       // параметр (номер столбца)
            0,                     // флаги создания
            &dwColThreadID[j]      // идентификатор потока
        );

        // Проверка — создан ли поток
        if (hColThread[j] == NULL)
        {
            cout << "Поток для сортировки столбца № " << j
                << " не был создан\n"
                << "Ошибка: " << GetLastError() << endl;
        }
    }

    // Ожидание завершения всех потоков сортировки
    WaitForMultipleObjects(m, hColThread, TRUE, INFINITE);

    // Закрытие дескрипторов потоков сортировки
    for (int j = 0; j < m; j++)
        CloseHandle(hColThread[j]);

    cout << "Сортировка столбцов завершена.\n";

    // Вывод отсортированной матрицы
    print_matrix("Матрица после сортировки столбцов по возрастанию");

    cout << "\n=== РЕЗУЛЬТАТ ===" << endl;
    cout << "Все столбцы матрицы отсортированы по возрастанию методом обмена." << endl;
    cout << "Каждый столбец обрабатывался в отдельном потоке параллельно." << endl;

    return 0;
}
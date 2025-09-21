#include <windows.h>
#include <iostream>
#include <tchar.h>
#include <locale.h>

using namespace std;

const int m = 10, n = 20;
float mtx[m][n];       // матрица
float col_sums[n];     // массив сумм
int col_numbers[n];    // номера столбцов

// Функция потока
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

int _tmain(int argc, _TCHAR* argv[])
{
    setlocale(LC_ALL, "RUS");
    // Описание переменных для работы с потоками
    HANDLE hThread[n];       // массив из n дескрипторов потоков
    DWORD dwThreadID[n];     // массив из n идентификаторов потоков

    // Создание матрицы
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            mtx[i][j] = (float)(rand() % 100); // чтобы числа были поменьше

    // Заполнение массивов исходными значениями
    for (int i = 0; i < n; i++)
    {
        col_sums[i] = 0;
        col_numbers[i] = i;
    }

    // Запуск потоков
    for (int i = 0; i < n; i++)
    {
        hThread[i] = CreateThread(
            NULL,                  // атрибуты безопасности по умолчанию
            0,                     // размер стека по умолчанию
            col_sum,               // функция потока
            &col_numbers[i],       // параметр (номер столбца)
            0,                     // флаги создания
            &dwThreadID[i]         // идентификатор потока
        );

        // Проверка — создан ли поток
        if (hThread[i] == NULL)
        {
            cout << "Поток № " << i
                << " не был создан\n"
                << "Ошибка: " << GetLastError() << endl;
        }
    }

    // Ожидание завершения всех потоков
    WaitForMultipleObjects(n, hThread, TRUE, INFINITE);

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
    cout << "Искомый столбец № " << num_min
        << " с суммой = " << min << endl;

    // Закрытие потоков
    for (int i = 0; i < n; i++)
        CloseHandle(hThread[i]);

    return 0;
}

#include <windows.h>   // Потоки, семафоры, мьютексы (WinAPI)
#include <iostream>    // Ввод-вывод
#include <locale.h>    // Локализация
#include <cstring>     // strcmp, strcpy_s

using namespace std;

// Структура одной заявки
struct zapis
{
    char material[20];   // Название материала
    int kolichestvo;     // Количество материала
};

// Глобальные переменные
int n;          // Количество цехов (производителей)
int k;          // Количество заявок от каждого цеха
int buf_size;   // Размер буфера (n * k)

zapis* buffer;   // Общий буфер заявок
int tek_pos = 0; // Текущая позиция (работаем как стек)

// Итоговые массивы (суммирование материалов)
char itog_nazvaniya[100][20];
int itog_kol[100];
int itog_kolvo = 0;  // Количество уникальных материалов

// Объекты синхронизации
HANDLE hSemPustye;    // Семафор пустых ячеек
HANDLE hSemZanyatye;  // Семафор занятых ячеек
HANDLE hMutex;        // Мьютекс для защиты буфера


// ==========================
// Поток "Цех" (производитель)
// ==========================
DWORD WINAPI ceh(LPVOID param)
{
    int nomer = *(int*)param;  // Получаем номер цеха
    delete (int*)param;        // Освобождаем память

    for (int i = 0; i < k; i++)
    {
        // Ждём свободную ячейку
        WaitForSingleObject(hSemPustye, INFINITE);

        // Входим в критическую секцию
        WaitForSingleObject(hMutex, INFINITE);

        cout << "Цех " << nomer << ": введите название материала: ";
        cin >> buffer[tek_pos].material;

        cout << "Цех " << nomer << ": введите количество: ";

        while (!(cin >> buffer[tek_pos].kolichestvo))
        {
            cout << "Ошибка! Введите число: ";
            cin.clear();
            cin.ignore(1000, '\n');
        }

        tek_pos++;  // Добавили элемент

        ReleaseMutex(hMutex);                 // Выход из критической секции
        ReleaseSemaphore(hSemZanyatye, 1, NULL); // Увеличиваем занятые
    }

    return 0;
}


// ==========================
// Поток "Обработчик" (потребитель)
// ==========================
DWORD WINAPI obrabotchik(LPVOID)
{
    int vsego = n * k;
    int obrabotano = 0;

    while (obrabotano < vsego)
    {
        WaitForSingleObject(hSemZanyatye, INFINITE);
        WaitForSingleObject(hMutex, INFINITE);

        tek_pos--;  // Забираем последний элемент (LIFO)

        char* mat = buffer[tek_pos].material;
        int kol = buffer[tek_pos].kolichestvo;

        bool est = false;

        // Проверка, есть ли уже такой материал
        for (int i = 0; i < itog_kolvo; i++)
        {
            if (strcmp(itog_nazvaniya[i], mat) == 0)
            {
                itog_kol[i] += kol;
                est = true;
                break;
            }
        }

        // Если новый материал — добавляем
        if (!est)
        {
            strcpy_s(itog_nazvaniya[itog_kolvo],
                sizeof(itog_nazvaniya[itog_kolvo]),
                mat);

            itog_kol[itog_kolvo] = kol;
            itog_kolvo++;
        }

        obrabotano++;

        ReleaseMutex(hMutex);
        ReleaseSemaphore(hSemPustye, 1, NULL);
    }

    return 0;
}


// ==========================
// Главная функция
// ==========================
int main()
{
    setlocale(LC_ALL, "");

    cout << "Введите количество цехов: ";
    cin >> n;

    cout << "Введите количество заявок для каждого цеха: ";
    cin >> k;

    buf_size = n * k;
    buffer = new zapis[buf_size];

    HANDLE* hCeh = new HANDLE[n];
    HANDLE hObr;

    // Создание семафоров
    hSemPustye = CreateSemaphore(NULL, buf_size, buf_size, NULL);
    hSemZanyatye = CreateSemaphore(NULL, 0, buf_size, NULL);

    // Создание мьютекса
    hMutex = CreateMutex(NULL, FALSE, NULL);

    // Запуск потоков-цехов
    for (int i = 0; i < n; i++)
    {
        int* p = new int;
        *p = i + 1;

        hCeh[i] = CreateThread(NULL, 0, ceh, p, 0, NULL);
    }

    // Запуск обработчика
    hObr = CreateThread(NULL, 0, obrabotchik, NULL, 0, NULL);

    // Ожидание завершения цехов
    WaitForMultipleObjects(n, hCeh, TRUE, INFINITE);

    // Ожидание завершения обработчика
    WaitForSingleObject(hObr, INFINITE);

    // Итог
    cout << "\nИтоговая заявка:\n";
    for (int i = 0; i < itog_kolvo; i++)
    {
        cout << itog_nazvaniya[i] << " : " << itog_kol[i] << endl;
    }

    // Освобождение ресурсов
    for (int i = 0; i < n; i++)
        CloseHandle(hCeh[i]);

    delete[] hCeh;

    CloseHandle(hObr);
    CloseHandle(hSemPustye);
    CloseHandle(hSemZanyatye);
    CloseHandle(hMutex);

    delete[] buffer;

    return 0;
}
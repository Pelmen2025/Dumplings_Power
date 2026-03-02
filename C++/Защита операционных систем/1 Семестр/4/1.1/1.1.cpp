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

zapis* buffer;  // Общий буфер заявок
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

    // Каждый цех создаёт k заявок
    for (int i = 0; i < k; i++)
    {
        // Ждём свободную ячейку
        WaitForSingleObject(hSemPustye, INFINITE);

        // Входим в критическую секцию
        WaitForSingleObject(hMutex, INFINITE);

        cout << "Цех " << nomer << ": введите название материала: ";
        cin >> buffer[tek_pos].material;

        cout << "Цех " << nomer << ": введите количество: ";

        // Проверка корректности ввода числа
        while (!(cin >> buffer[tek_pos].kolichestvo))
        {
            cout << "Ошибка! Введите число: ";
            cin.clear();
            cin.ignore(1000, '\n');
        }

        tek_pos++;  // Добавили элемент в буфер

        // Выход из критической секции
        ReleaseMutex(hMutex);

        // Увеличиваем количество занятых ячеек
        ReleaseSemaphore(hSemZanyatye, 1, NULL);
    }

    return 0;
}


// ==========================
// Поток "Обработчик" (потребитель)
// ==========================
DWORD WINAPI obrabotchik(LPVOID)
{
    int vsego = n * k;   // Сколько всего заявок будет создано
    int obrabotano = 0;  // Сколько уже обработано

    while (obrabotano < vsego)
    {
        // Ждём, пока появится занятая ячейка
        WaitForSingleObject(hSemZanyatye, INFINITE);

        // Входим в критическую секцию
        WaitForSingleObject(hMutex, INFINITE);

        tek_pos--;  // Забираем последний элемент (LIFO)

        char* mat = buffer[tek_pos].material;
        int kol = buffer[tek_pos].kolichestvo;

        bool est = false;

        // Проверяем, есть ли уже такой материал в итоговом списке
        for (int i = 0; i < itog_kolvo; i++)
        {
            if (strcmp(itog_nazvaniya[i], mat) == 0)
            {
                itog_kol[i] += kol;  // Увеличиваем сумму
                est = true;
                break;
            }
        }

        // Если материала нет — добавляем новый
        if (!est)
        {
            strcpy_s(itog_nazvaniya[itog_kolvo],
                sizeof(itog_nazvaniya[itog_kolvo]),
                mat);

            itog_kol[itog_kolvo] = kol;
            itog_kolvo++;
        }

        obrabotano++;  // Увеличиваем счётчик обработанных

        // Выход из критической секции
        ReleaseMutex(hMutex);

        // Освобождаем одну пустую ячейку
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

    buf_size = n * k;              // Общий размер буфера
    buffer = new zapis[buf_size];  // Выделение памяти

    HANDLE* hCeh = new HANDLE[n];  // Массив потоков-цехов
    HANDLE hObr;                   // Поток-обработчик

    // Создание семафоров
    hSemPustye = CreateSemaphore(NULL, buf_size, buf_size, NULL);
    // Изначально все ячейки пустые

    hSemZanyatye = CreateSemaphore(NULL, 0, buf_size, NULL);
    // Изначально занятых ячеек нет

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

    // Ожидание завершения всех цехов
    WaitForMultipleObjects(n, hCeh, TRUE, INFINITE);

    // Ожидание завершения обработчика
    WaitForSingleObject(hObr, INFINITE);

    // Вывод итоговой сводки
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
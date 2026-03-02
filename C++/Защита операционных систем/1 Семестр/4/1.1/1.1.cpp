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
<<<<<<< HEAD
char itog_nazvaniya[100][20];  // Массив до 100 строк (названий материалов),
                               // каждое название — массив из 20 символов
int itog_kol[100];   // Общее количество этого материала
=======
char itog_nazvaniya[100][20];
int itog_kol[100];
>>>>>>> origin/main
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

<<<<<<< HEAD
        // Указатель на название материала в текущем элементе буфера
        char* mat = buffer[tek_pos].material;

        // Копия количества материала из текущего элемента
        int kol = buffer[tek_pos].kolichestvo;

        bool est = false; // Флаг, показывающий, найден ли уже этот материал в итоговом массиве
=======
        char* mat = buffer[tek_pos].material;
        int kol = buffer[tek_pos].kolichestvo;

        bool est = false;
>>>>>>> origin/main

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
<<<<<<< HEAD
        if (!est)  // Если материал не найден в итоговом массиве (новый)
        {
            // Копируем название нового материала в массив итоговых названий
=======
        if (!est)
        {
>>>>>>> origin/main
            strcpy_s(itog_nazvaniya[itog_kolvo],
                sizeof(itog_nazvaniya[itog_kolvo]),
                mat);

<<<<<<< HEAD
            // Сохраняем количество для этого нового материала
            itog_kol[itog_kolvo] = kol;

            // Увеличиваем счётчик уникальных материалов
=======
            itog_kol[itog_kolvo] = kol;
>>>>>>> origin/main
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
<<<<<<< HEAD
    buffer = new zapis[buf_size];  // Выделяем память под буфер заявок размером buf_size
=======
    buffer = new zapis[buf_size];  // Выделение памяти
>>>>>>> origin/main

    HANDLE* hCeh = new HANDLE[n];  // Массив потоков-цехов
    HANDLE hObr;                   // Поток-обработчик

    // Создание семафоров
<<<<<<< HEAD
    hSemPustye = CreateSemaphore(
        NULL,      // lpSemaphoreAttributes: атрибуты безопасности (NULL — стандартные)
        buf_size,  // lInitialCount: начальное значение семафора (сколько свободных ячеек сразу)
        buf_size,  // lMaximumCount: максимальное значение семафора (не более buf_size)
        NULL       // lpName: имя семафора (NULL — неименованный семафор)
    );
=======
    hSemPustye = CreateSemaphore(NULL, buf_size, buf_size, NULL);
>>>>>>> origin/main
    // Изначально все ячейки пустые

    hSemZanyatye = CreateSemaphore(NULL, 0, buf_size, NULL);
    // Изначально занятых ячеек нет

    // Создание мьютекса
    hMutex = CreateMutex(NULL, FALSE, NULL);

    // Запуск потоков-цехов
    for (int i = 0; i < n; i++)
    {
<<<<<<< HEAD
        int* p = new int;         // выделяем память для хранения номера цеха
        *p = i + 1;               // присваиваем номер цеха (1, 2, 3, ...)

        hCeh[i] = CreateThread(NULL, 0, ceh, p, 0, NULL);   // Создаём поток
=======
        int* p = new int;
        *p = i + 1;

        hCeh[i] = CreateThread(NULL, 0, ceh, p, 0, NULL);
>>>>>>> origin/main
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
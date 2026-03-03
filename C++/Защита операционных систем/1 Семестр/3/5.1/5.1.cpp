#include <windows.h>   // WinAPI: потоки, мьютексы
#include <stdio.h>
#include <iostream>
#include <locale.h>

using namespace std;

// Узел односвязного списка (элемент цепочки в хеш-таблице)
struct Uzol
{
    int chislo;                // значение
    Uzol* sleduyuschiy;        // указатель на следующий элемент
};

// Класс-монитор для синхронизации доступа к строкам хеш-таблицы
class CMonitor
{
public:
    int* occupated_rows;       // массив флагов занятости строк
    HANDLE* mutexyStrok;       // массив мьютексов (по одному на строку)
    int kol_strok;             // количество строк таблицы

    // Конструктор: создаёт мьютекс для каждой строки
    CMonitor(int kol)
    {
        kol_strok = kol;

        occupated_rows = new int[kol];      // выделяем массив флагов
        mutexyStrok = new HANDLE[kol];      // выделяем массив мьютексов

        for (int i = 0; i < kol; i++)
        {
            occupated_rows[i] = 0;          // изначально строка свободна
            mutexyStrok[i] = CreateMutex(NULL, FALSE, NULL); // создаём мьютекс
        }
    }

    // Деструктор: освобождаем ресурсы
    ~CMonitor()
    {
        for (int i = 0; i < kol_strok; i++)
        {
            CloseHandle(mutexyStrok[i]);    // закрываем мьютексы
        }

        delete[] occupated_rows;
        delete[] mutexyStrok;
    }

    // Захват строки (блокировка мьютекса)
    void OcupateRow(int row_number)
    {
        WaitForSingleObject(mutexyStrok[row_number], INFINITE); // ждём освобождения
        occupated_rows[row_number] = 1;                         // помечаем как занятую
    }

    // Освобождение строки
    void FreeRow(int row_number)
    {
        occupated_rows[row_number] = 0;     // помечаем как свободную
        ReleaseMutex(mutexyStrok[row_number]); // освобождаем мьютекс
    }
};

// Глобальная хеш-таблица (массив указателей на списки)
Uzol** hashTablitsa;

// Простая хеш-функция: остаток от деления
int hashFunktsiya(int x, int osnovanie)
{
    return x % osnovanie;
}

// Параметры, передаваемые в поток
struct PotokParam
{
    CMonitor* monitor; // указатель на монитор
    int osnovanie;     // размер таблицы
    int id;            // id потока
};

// Функция потока (WinAPI требует такой сигнатуры)
DWORD WINAPI generiruyIPishi(LPVOID param)
{
    PotokParam* p = (PotokParam*)param;

    CMonitor* monitor = p->monitor;
    int osnovanie = p->osnovanie;
    int id = p->id;

    // Инициализация генератора случайных чисел
    srand(GetTickCount() + id * 100);

    // Каждый поток генерирует 10 чисел
    for (int i = 0; i < 10; i++)
    {
        int x = rand() % 100;                     // случайное число
        int k = hashFunktsiya(x, osnovanie);      // вычисляем индекс строки

        monitor->OcupateRow(k);                   // блокируем строку

        // Добавляем элемент в начало списка (метод цепочек)
        Uzol* novyy = new Uzol;
        novyy->chislo = x;
        novyy->sleduyuschiy = hashTablitsa[k];
        hashTablitsa[k] = novyy;

        monitor->FreeRow(k);                      // освобождаем строку
    }

    return 0;
}

int main()
{
    setlocale(LC_ALL, "Russian");

    int osnovanie;

    cout << "Введите основание хеш-функции (количество строк в таблице): ";
    cin >> osnovanie;

    // Проверка корректности ввода
    if (osnovanie <= 0)
    {
        cout << "Основание должно быть больше 0!" << endl;
        return 1;
    }

    int kol_strok = osnovanie;

    // Выделяем память под хеш-таблицу
    hashTablitsa = new Uzol * [kol_strok];

    // Инициализация строк NULL
    for (int i = 0; i < kol_strok; i++)
    {
        hashTablitsa[i] = NULL;
    }

    // Создаём монитор
    CMonitor monitor(kol_strok);

    int n = 2;                   // количество потоков
    HANDLE potoki[2];            // массив дескрипторов потоков
    PotokParam paramy[2];        // параметры потоков

    // Создание потоков
    for (int i = 0; i < n; i++)
    {
        paramy[i].monitor = &monitor;       // даём потоку адрес объекта синхронизации (общий для всех)
        paramy[i].osnovanie = osnovanie;    // передаём размер хеш-таблицы (чтобы знать, на что делать %)
        paramy[i].id = i;                   // уникальный номер потока (0–4), нужен для разной рандомизации

        potoki[i] = CreateThread(
            NULL,
            0,
            generiruyIPishi,
            (LPVOID)&paramy[i],
            0,
            NULL
        );
    }

    // Ожидаем завершения всех потоков
    WaitForMultipleObjects(n, potoki, TRUE, INFINITE);

    // Закрываем дескрипторы потоков
    for (int i = 0; i < n; i++)
    {
        CloseHandle(potoki[i]);
    }

    // Вывод хеш-таблицы
    cout << "Хеш-таблица:" << endl;

    for (int i = 0; i < kol_strok; i++)
    {
        cout << "Строка " << i << ": ";

        Uzol* tek = hashTablitsa[i];

        while (tek != NULL)
        {
            cout << tek->chislo << " ";
            tek = tek->sleduyuschiy;
        }

        cout << endl;
    }

    // Освобождение памяти списков
    for (int i = 0; i < kol_strok; i++)
    {
        Uzol* tek = hashTablitsa[i];

        while (tek != NULL)
        {
            Uzol* sleduyuschiy = tek->sleduyuschiy;
            delete tek;
            tek = sleduyuschiy;
        }
    }

    delete[] hashTablitsa;

    return 0;
}
#include <windows.h>   // Работа с потоками и мьютексами WinAPI
#include <stdio.h>
#include <iostream>
#include <locale.h>

using namespace std;

// Структура узла односвязного списка
struct Uzol
{
    int chislo;                 // Значение, которое хранится в узле
    Uzol* sleduyuschiy;         // Указатель на следующий элемент списка
};

// Глобальная хеш-таблица из 10 "строк" (корзин)
// Каждая строка — это начало связного списка
Uzol* hashTablitsa[10];

// Массив мьютексов — по одному на каждую строку таблицы
// Нужны для синхронизации доступа потоков
HANDLE hMutexStroki[10];

// Простая хеш-функция
// Возвращает остаток от деления на 10
// Определяет, в какую строку таблицы попадёт число
int hashFunktsiya(int x)
{
    return x % 10;
}

// Функция потока (обязательная сигнатура для CreateThread)
DWORD WINAPI generiruyIPishi(LPVOID param)
{
    // Получаем идентификатор потока
    int id = (int)param;

    // Инициализация генератора случайных чисел
    // Добавляем id, чтобы у потоков были разные последовательности
    srand(GetTickCount() + id * 100);

    // Каждый поток добавляет 10 случайных чисел
    for (int i = 0; i < 10; i++)
    {
        int x = rand() % 100;           // Генерация числа от 0 до 99
        int k = hashFunktsiya(x);       // Определение строки таблицы

        // Захватываем мьютекс конкретной строки
        // Блокируем доступ другим потокам к этой строке
        WaitForSingleObject(hMutexStroki[k], INFINITE);

        // Создаём новый узел
        Uzol* novyy = new Uzol;
        novyy->chislo = x;

        // Вставка в начало списка (односвязный список)
        novyy->sleduyuschiy = hashTablitsa[k];
        hashTablitsa[k] = novyy;

        // Освобождаем мьютекс
        ReleaseMutex(hMutexStroki[k]);
    }

    return 0;   // Завершение потока
}

int main()
{
    // Установка русской локали (для корректного вывода текста)
    setlocale(LC_ALL, "Russian");

    // Инициализация хеш-таблицы и мьютексов
    for (int i = 0; i < 10; i++)
    {
        hashTablitsa[i] = NULL;                     // Пока списки пустые
        hMutexStroki[i] = CreateMutex(NULL, FALSE, NULL);  // Создаём мьютекс
    }

    int n = 5;              // Количество потоков
    HANDLE potoki[5];       // Массив дескрипторов потоков

    // Создание потоков
    for (int i = 0; i < n; i++)
    {
        potoki[i] = CreateThread(
            NULL,               // Атрибуты безопасности
            0,                  // Размер стека (по умолчанию)
            generiruyIPishi,    // Функция потока
            (LPVOID)i,          // Параметр (id потока)
            0,                  // Флаги запуска
            NULL                // Идентификатор потока (не используется)
        );
    }

    // Ожидание завершения всех потоков
    WaitForMultipleObjects(n, potoki, TRUE, INFINITE);

    // Закрываем дескрипторы потоков
    for (int i = 0; i < n; i++)
    {
        CloseHandle(potoki[i]);
    }

    // Закрываем мьютексы
    for (int i = 0; i < 10; i++)
    {
        CloseHandle(hMutexStroki[i]);
    }

    // Вывод содержимого хеш-таблицы
    cout << "Хеш-таблица:" << endl;

    for (int i = 0; i < 10; i++)
    {
        cout << "Строка " << i << ": ";
        Uzol* tek = hashTablitsa[i];

        // Проходим по связанному списку
        while (tek != NULL)
        {
            cout << tek->chislo << " ";
            tek = tek->sleduyuschiy;
        }

        cout << endl;
    }

    // Освобождение памяти (удаление всех узлов)
    for (int i = 0; i < 10; i++)
    {
        Uzol* tek = hashTablitsa[i];

        while (tek != NULL)
        {
            Uzol* sleduyuschiy = tek->sleduyuschiy;
            delete tek;                 // Освобождаем текущий узел
            tek = sleduyuschiy;         // Переходим к следующему
        }
    }

    return 0;
}
#include <windows.h>  // Библиотека для работы с WinAPI (потоки, события, дескрипторы)
#include <iostream>   // Библиотека для ввода и вывода (cin, cout)
#include <locale.h>   // Библиотека для поддержки русского языка в консоли
#include <cmath>      // Библиотека математических функций (нужна для проверки результата через exp(x))

using namespace std;

// --- ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ---
// Они сделаны глобальными, чтобы все потоки имели к ним общий доступ.
double x;                   // Значение аргумента x, которое вводит пользователь
int nomer_chlena;           // Номер текущего члена ряда (n), который сейчас вычисляется
double resultat_factorial;  // Сюда первый поток запишет результат вычисления факториала (n!)
double resultat_stepeni;    // Сюда второй поток запишет результат вычисления степени (x^n)
double summa_ryada;         // Итоговая сумма ряда Маклорена

// --- ДЕСКРИПТОРЫ СОБЫТИЙ (СВЕТОФОРЫ ДЛЯ ПОТОКОВ) ---
// События нужны для синхронизации. Поток может "спать" до тех пор, пока событие не сработает.
HANDLE sobytie_start_factorial; // Сигнал для потока факториала: "Начинай считать!"
HANDLE sobytie_start_stepeni;   // Сигнал для потока степени: "Начинай считать!"
HANDLE sobytie_factorial_gotov; // Сигнал от потока факториала: "Я закончил, результат готов"
HANDLE sobytie_stepeni_gotov;   // Сигнал от потока степени: "Я закончил, результат готов"


// =========================================================================
// ПОТОК 1: Функция вычисления факториала (знаменателя: n!)
// =========================================================================
DWORD WINAPI vychislenie_factoriala(LPVOID param)
{
    while (true) // Бесконечный цикл, поток живет и ждет новых задач
    {
        // Поток засыпает и ждет сигнала "sobytie_start_factorial"
        // INFINITE означает, что ждать он будет бесконечно долго, пока не дадут сигнал
        WaitForSingleObject(sobytie_start_factorial, INFINITE);

        // Специальное условие для выхода из бесконечного цикла (завершение работы потока)
        // В main() мы установим nomer_chlena = -1, когда все вычисления закончатся
        if (nomer_chlena == -1)
        {
            break;
        }

        // --- Начало полезной работы ---
        double factorial = 1.0;
        for (int i = 1; i <= nomer_chlena; i++)
        {
            factorial *= i; // Вычисление факториала
        }

        resultat_factorial = factorial; // Сохраняем результат в глобальную переменную

        // Сбрасываем (выключаем) сигнал старта, так как мы его уже отработали.
        // Это нужно, потому что мы используем события с ручным сбросом (Manual Reset)
        ResetEvent(sobytie_start_factorial);

        // Зажигаем зеленый свет для управляющего потока: "Факториал посчитан!"
        SetEvent(sobytie_factorial_gotov);
    }
    return 0; // Завершение потока
}


// =========================================================================
// ПОТОК 2: Функция вычисления степени (числителя: x^n)
// =========================================================================
// Работает по абсолютно такой же логике, как и поток факториала
DWORD WINAPI vychislenie_stepeni(LPVOID param)
{
    while (true)
    {
        // Ждем сигнала к началу вычисления степени
        WaitForSingleObject(sobytie_start_stepeni, INFINITE);

        // Проверка на сигнал завершения программы
        if (nomer_chlena == -1)
        {
            break;
        }

        // --- Начало полезной работы ---
        double stepen = 1.0;
        for (int i = 0; i < nomer_chlena; i++)
        {
            stepen *= x; // Умножаем x само на себя n раз
        }

        resultat_stepeni = stepen; // Сохраняем результат

        // Сбрасываем сигнал старта
        ResetEvent(sobytie_start_stepeni);

        // Сигнализируем о том, что степень посчитана
        SetEvent(sobytie_stepeni_gotov);
    }
    return 0;
}


// =========================================================================
// ПОТОК 3: Управляющий поток (Считает сумму ряда)
// =========================================================================
DWORD WINAPI summa_chlenov_ryada(LPVOID param)
{
    // Получаем количество членов ряда из параметра потока (передано из main)
    int kolichestvo_chlenov = *(int*)param;

    // Формула Маклорена для e^x начинается с 1: e^x = 1 + x + x^2/2! + x^3/3! ...
    summa_ryada = 1.0;

    for (int chlen = 1; chlen <= kolichestvo_chlenov; chlen++)
    {
        nomer_chlena = chlen; // Устанавливаем глобальную переменную n для рабочих потоков

        // Даем команду (зеленый свет) рабочим потокам начать вычисления
        SetEvent(sobytie_start_factorial);
        SetEvent(sobytie_start_stepeni);

        // Массив из двух событий, завершения которых мы будем ждать
        HANDLE sobytiya_dlya_ozhidaniya[2] = { sobytie_factorial_gotov, sobytie_stepeni_gotov };

        // Управляющий поток "засыпает" и ждет, пока ОБА рабочих потока не закончат работу.
        // 2 - количество ожидаемых событий
        // TRUE - ждать нужно ВСЕ события (и факториал, и степень)
        // INFINITE - ждать бесконечно
        WaitForMultipleObjects(2, sobytiya_dlya_ozhidaniya, TRUE, INFINITE);

        // Когда мы оказались на этой строке, значит оба потока закончили вычисления.
        // Вычисляем текущий член ряда (x^n / n!)
        double tekushchiy_chlen = resultat_stepeni / resultat_factorial;

        summa_ryada += tekushchiy_chlen; // Прибавляем к общей сумме
        cout << "Член " << chlen << ": " << tekushchiy_chlen << endl; // Выводим промежуточный результат

        // Выключаем сигналы готовности перед следующей итерацией цикла (ручной сброс)
        ResetEvent(sobytie_factorial_gotov);
        ResetEvent(sobytie_stepeni_gotov);
    }
    return 0; // Когда цикл закончится, управляющий поток завершается
}


// =========================================================================
// ГЛАВНЫЙ ПОТОК: Инициализация, запуск и очистка
// =========================================================================
int main()
{
    setlocale(LC_ALL, "Russian"); // Включаем русский язык в консоли

    cout << "Программа вычисления значения функции e^x по ряду Маклорена" << endl;

    cout << "Введите значение аргумента x: ";
    cin >> x;

    int kolichestvo_chlenov;
    cout << "Введите количество членов ряда для вычисления: ";
    cin >> kolichestvo_chlenov;

    // --- СОЗДАНИЕ СОБЫТИЙ ---
    // CreateEvent(АтрибутыБезопасности, РучнойСброс, НачальноеСостояние, Имя)
    // TRUE во втором параметре значит "Manual-reset" (нужно вручную вызывать ResetEvent)
    // FALSE в третьем параметре значит, что изначально событие неактивно (красный свет)
    sobytie_start_factorial = CreateEvent(NULL, TRUE, FALSE, NULL);
    sobytie_start_stepeni = CreateEvent(NULL, TRUE, FALSE, NULL);
    sobytie_factorial_gotov = CreateEvent(NULL, TRUE, FALSE, NULL);
    sobytie_stepeni_gotov = CreateEvent(NULL, TRUE, FALSE, NULL);

    // --- СОЗДАНИЕ ПОТОКОВ ---
    // Создаем поток, который будет крутиться в функции vychislenie_factoriala
    HANDLE potok_factorial = CreateThread(NULL, 0, vychislenie_factoriala, NULL, 0, NULL);

    // Создаем поток для функции vychislenie_stepeni
    HANDLE potok_stepeni = CreateThread(NULL, 0, vychislenie_stepeni, NULL, 0, NULL);

    // Создаем управляющий поток. Передаем ему адрес переменной kolichestvo_chlenov
    HANDLE potok_summa = CreateThread(NULL, 0, summa_chlenov_ryada, &kolichestvo_chlenov, 0, NULL);

    // Главный поток программы "засыпает" и ждет, пока поток суммы (potok_summa) полностью отработает
    WaitForSingleObject(potok_summa, INFINITE);

    // --- ВЫВОД РЕЗУЛЬТАТОВ ---
    // Сюда мы попадем только когда сумма будет полностью подсчитана
    cout << "\nРезультат вычисления:" << endl;
    cout << "e^" << x << " ~ " << summa_ryada << endl;

    // Сравниваем нашу сумму с эталонным значением из стандартной библиотеки (функция exp)
    cout << "Точное значение (через exp): " << exp(x) << endl;

    // --- ЗАВЕРШЕНИЕ РАБОЧИХ ПОТОКОВ ---
    // Так как рабочие потоки крутятся в while(true), нам нужно их убить корректно.
    nomer_chlena = -1; // Сигнал для выхода из цикла (break) в потоках

    // Будим рабочие потоки, чтобы они дошли до проверки if (nomer_chlena == -1)
    SetEvent(sobytie_start_factorial);
    SetEvent(sobytie_start_stepeni);

    // Ждем, пока рабочие потоки закроются
    WaitForSingleObject(potok_factorial, INFINITE);
    WaitForSingleObject(potok_stepeni, INFINITE);

    // --- ОЧИСТКА ПАМЯТИ ---
    // Очень важное правило WinAPI: открыл дескриптор (HANDLE) -> обязательно закрой его (CloseHandle)
    CloseHandle(potok_summa);
    CloseHandle(potok_factorial);
    CloseHandle(potok_stepeni);
    CloseHandle(sobytie_start_factorial);
    CloseHandle(sobytie_start_stepeni);
    CloseHandle(sobytie_factorial_gotov);
    CloseHandle(sobytie_stepeni_gotov);

    return 0; // Конец программы
}
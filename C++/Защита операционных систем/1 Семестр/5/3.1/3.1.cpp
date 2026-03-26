#include <windows.h> // Подключаем библиотеку для работы с Windows API (потоки, события, дескрипторы)
#include <iostream>  // Подключаем библиотеку для ввода/вывода (cin, cout)
#include <locale.h>  // Подключаем библиотеку для локализации (поддержка русского языка в консоли)

using namespace std;

// --- Глобальные переменные ---
// Сделаны глобальными, чтобы все потоки имели к ним простой доступ без передачи сложных структур.
float* massiv;         // Указатель на массив, который будем сортировать
int razmer_massiva;    // Размер этого массива
HANDLE* sobitiya;      // Массив дескрипторов (handle) событий для синхронизации потоков

// Структура для передачи параметров в каждый отдельный поток
struct ParametriPotoka
{
    int nomer_prohoda; // Номер прохода сортировки (0 для первого потока, 1 для второго и т.д.)
};

// --- Функция потока (выполняет один проход алгоритма "пузырька") ---
DWORD WINAPI prohod_sortirovki(LPVOID param)
{
    // Преобразуем переданный указатель обратно в тип структуры параметров
    ParametriPotoka* parametri = (ParametriPotoka*)param;
    int nomer_prohoda = parametri->nomer_prohoda;

    // В сортировке пузырьком с каждым проходом нужно делать на 1 сравнение меньше,
    // так как самые большие элементы "всплывают" в конец массива.
    int kolichestvo_sravneniy = razmer_massiva - 1 - nomer_prohoda;

    // Проходим по элементам массива
    for (int i = 0; i < kolichestvo_sravneniy; i++)
    {
        // Если текущий элемент больше следующего, меняем их местами
        if (massiv[i] > massiv[i + 1])
        {
            float temp = massiv[i];
            massiv[i] = massiv[i + 1];
            massiv[i + 1] = temp;
        }

        // --- КЛЮЧЕВОЙ МОМЕНТ МНОГОПОТОЧНОСТИ ---
        // Когда текущий поток доходит ровно до середины своих сравнений,
        // он подает сигнал (SetEvent), чтобы разрешить запуск СЛЕДУЮЩЕГО потока.
        // Это позволяет потокам работать внахлест (конвейером).
        // Проверка "nomer_prohoda < razmer_massiva - 2" гарантирует, что мы не попытаемся
        // подать сигнал для потока, которого не существует (выход за пределы массива событий).
        if (i == (kolichestvo_sravneniy / 2) && nomer_prohoda < razmer_massiva - 2)
        {
            SetEvent(sobitiya[nomer_prohoda + 1]); // Зажигаем "зеленый свет" для следующего потока
        }
    }
    return 0; // Поток завершает работу
}

int main()
{
    // Включаем поддержку русского языка в консоли
    setlocale(LC_ALL, "Russian");

    // --- Ввод данных ---
    cout << "Введите размер массива: ";
    cin >> razmer_massiva;

    // Выделяем память под массив чисел с плавающей точкой
    massiv = new float[razmer_massiva];

    cout << "Введите элементы массива:" << endl;
    for (int i = 0; i < razmer_massiva; i++)
    {
        cout << "Элемент " << i + 1 << ": ";
        cin >> massiv[i];
    }

    // Вывод исходного массива
    cout << "\nИсходный массив:" << endl;
    for (int i = 0; i < razmer_massiva; i++)
    {
        cout << massiv[i] << " ";
    }
    cout << endl;

    // --- Инициализация синхронизации (Событий) ---
    // Для массива из N элементов нужно N-1 проходов сортировки
    sobitiya = new HANDLE[razmer_massiva - 1];
    for (int i = 0; i < razmer_massiva - 1; i++)
    {
        // Создаем событие. 
        // 2-й параметр (true) = ручной сброс состояния события (ResetEvent нужен вручную)
        // 3-й параметр (false) = начальное состояние "не сигнальное" (красный свет)
        sobitiya[i] = CreateEvent(NULL, true, false, NULL);
    }

    // Запускаем цепную реакцию: устанавливаем 0-е событие в сигнальное состояние ("зеленый свет"),
    // чтобы главный поток мог создать и запустить самый ПЕРВЫЙ поток сортировки.
    SetEvent(sobitiya[0]);

    // Выделяем память под дескрипторы (handles) потоков и их параметры
    HANDLE* potoki = new HANDLE[razmer_massiva - 1];
    ParametriPotoka* parametri = new ParametriPotoka[razmer_massiva - 1];

    // --- Главный цикл диспетчеризации потоков ---
    for (int i = 0; i < razmer_massiva - 1; i++)
    {
        parametri[i].nomer_prohoda = i; // Сохраняем номер прохода для потока

        // Главный поток ЖДЕТ, пока событие sobitiya[i] не станет сигнальным.
        // Для i=0 оно уже сигнальное (мы сделали это выше).
        // Для остальных - главный поток "зависнет" здесь, пока предыдущий поток не дойдет 
        // до середины массива и не вызовет SetEvent(sobitiya[nomer_prohoda + 1]).
        WaitForSingleObject(sobitiya[i], INFINITE);

        // Как только получен сигнал, создаем и запускаем поток
        potoki[i] = CreateThread(NULL, 0, prohod_sortirovki, &parametri[i], 0, NULL);

        // Сбрасываем событие обратно в несигнальное состояние (снова включаем "красный свет")
        ResetEvent(sobitiya[i]);
    }

    // После того как все потоки были созданы, главный поток должен дождаться 
    // полного завершения работы ВСЕХ созданных потоков сортировки.
    // TRUE означает "ждать все", INFINITE - "ждать бесконечно долго".
    WaitForMultipleObjects(razmer_massiva - 1, potoki, TRUE, INFINITE);

    // --- Вывод результатов ---
    cout << "\nОтсортированный массив:" << endl;
    for (int i = 0; i < razmer_massiva; i++)
    {
        cout << massiv[i] << " ";
    }
    cout << endl;

    // --- Освобождение ресурсов (Уборка мусора) ---
    // Закрываем системные дескрипторы для предотвращения утечек памяти ядра Windows
    for (int i = 0; i < razmer_massiva - 1; i++)
    {
        CloseHandle(potoki[i]);
        CloseHandle(sobitiya[i]);
    }

    // Освобождаем динамическую память, выделенную через new
    delete[] massiv;
    delete[] sobitiya;
    delete[] potoki;
    delete
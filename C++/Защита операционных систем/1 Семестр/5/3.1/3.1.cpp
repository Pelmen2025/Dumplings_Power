#include <windows.h>  // Для WinAPI: потоки, события, мьютексы
#include <iostream>   // Для вывода в консоль
#include <fstream>    // Для работы с файлами
#include <locale.h>   // Для русского языка

using namespace std;

#define MAX_FILES 10 // Максимальное количество файлов (и потоков-чтецов)

// --- ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ (Общая память для всех потоков) ---
int chisla_iz_failov[MAX_FILES]; // Массив, где хранятся текущие прочитанные числа от каждого потока
bool faili_konec[MAX_FILES];     // Флаги: достигнут ли конец файла (true - если чисел больше нет)
ifstream faili[MAX_FILES];       // Массив файловых потоков для чтения
ofstream resultat_fail;          // Файловый поток для записи итогового результата
int kolichestvo_failov;          // Реальное количество файлов, введенное пользователем

// --- ПРИМИТИВЫ СИНХРОНИЗАЦИИ ---
HANDLE sobytiya_dannye_gotovy[MAX_FILES]; // Сигналы ОТ чтецов К сборщику: "Я прочитал число, забирай"
HANDLE sobytiya_mozhno_chitat[MAX_FILES]; // Сигналы ОТ сборщика К чтецам: "Твое число забрали, читай следующее"
HANDLE sobytie_rabota_okonchena;          // Сигнал о том, что все файлы полностью прочитаны
HANDLE mutex;                             // Мьютекс (эстафетная палочка) для защиты консоли от "каши" при выводе


// =========================================================================
// ПОТОКИ-ЧТЕЦЫ: Функция чтения чисел из файла (по одному потоку на файл)
// =========================================================================
DWORD WINAPI chtenie_iz_faila(LPVOID param)
{
    // Получаем номер файла/потока, за который отвечает этот конкретный поток (от 0 до n-1)
    int nomer_faila = *((int*)param);

    while (true)
    {
        // Поток засыпает и ждет разрешения на чтение очередного числа.
        WaitForSingleObject(sobytiya_mozhno_chitat[nomer_faila], INFINITE);

        // Проверяем, не дал ли главный поток команду на полное завершение работы
        // Timeout = 0 означает "просто проверить состояние события и пойти дальше, не зависая"
        if (WaitForSingleObject(sobytie_rabota_okonchena, 0) == WAIT_OBJECT_0)
        {
            break; // Если работа окончена - выходим из цикла, поток завершается
        }

        // Если конец файла еще не достигнут
        if (!faili_konec[nomer_faila])
        {
            // Пытаемся считать число. Если успешно:
            if (faili[nomer_faila] >> chisla_iz_failov[nomer_faila])
            {
                // Захватываем мьютекс, чтобы другие потоки не влезли в наш консольный вывод
                WaitForSingleObject(mutex, INFINITE);
                cout << "Поток " << nomer_faila + 1 << ": прочитал число " << chisla_iz_failov[nomer_faila] << endl;
                ReleaseMutex(mutex); // Освобождаем мьютекс для других

                // Сигнализируем сборщику: "Число успешно прочитано и лежит в массиве"
                SetEvent(sobytiya_dannye_gotovy[nomer_faila]);
            }
            else
            {
                // Если считывание не удалось (дошли до конца файла)
                faili_konec[nomer_faila] = true; // Отмечаем, что этот файл пуст

                // Все равно сигнализируем сборщику, чтобы он не завис в ожидании данных от этого потока
                SetEvent(sobytiya_dannye_gotovy[nomer_faila]);
            }
        }

        // Сбрасываем (выключаем) разрешение на чтение. 
        // Поток на следующей итерации снова уснет в ожидании команды от сборщика.
        ResetEvent(sobytiya_mozhno_chitat[nomer_faila]);
    }
    return 0;
}


// =========================================================================
// ПОТОК-СБОРЩИК: Формирование итогового отсортированного файла
// =========================================================================
DWORD WINAPI formirovanie_rezultata(LPVOID param)
{
    int n = kolichestvo_failov;

    while (true)
    {
        // Подготавливаем массив событий для ожидания
        HANDLE sobytiya_dlya_ozhidaniya[MAX_FILES];
        for (int i = 0; i < n; i++)
        {
            sobytiya_dlya_ozhidaniya[i] = sobytiya_dannye_gotovy[i];
        }

        // Сборщик засыпает и ждет, пока ВСЕ потоки-чтецы не подадут сигнал "данные готовы".
        // Важно: если файл закончился, его событие тоже будет активно (мы это сделали выше), 
        // так что сборщик не зависнет навсегда.
        WaitForMultipleObjects(n, sobytiya_dlya_ozhidaniya, TRUE, INFINITE);

        // Проверяем, остались ли еще данные в файлах
        bool vse_konec = true;
        for (int i = 0; i < n; i++)
        {
            if (!faili_konec[i])
            {
                vse_konec = false; // Нашли хотя бы один не пустой файл
                break;
            }
        }

        // Если все файлы полностью прочитаны
        if (vse_konec)
        {
            SetEvent(sobytie_rabota_okonchena); // Сигнализируем чтецам, что пора по домам
            break; // Завершаем работу сборщика
        }

        // --- ПОИСК МИНИМАЛЬНОГО ЭЛЕМЕНТА ---
        int min_chislo = 2147483647; // Максимально возможное значение для int (как стартовое)
        int nomer_min_faila = -1;    // Индекс потока, у которого оказалось минимальное число

        for (int i = 0; i < n; i++)
        {
            // Ищем минимум только среди тех файлов, которые еще не закончились
            if (!faili_konec[i] && chisla_iz_failov[i] < min_chislo)
            {
                min_chislo = chisla_iz_failov[i];
                nomer_min_faila = i;
            }
        }

        // Если нашли минимальное число
        if (nomer_min_faila != -1)
        {
            // Записываем его в итоговый файл
            resultat_fail << min_chislo << " ";

            // Выводим информацию в консоль (с защитой через мьютекс)
            WaitForSingleObject(mutex, INFINITE);
            cout << "Результат: записал " << min_chislo;

            // Ради интереса выводим сообщение, если какие-то файлы уже закончились
            int kol_konec = 0;
            for (int i = 0; i < n; i++)
            {
                if (faili_konec[i]) kol_konec++;
            }
            if (kol_konec > 0)
            {
                cout << " (некоторые файлы закончились)";
            }
            cout << endl;
            ReleaseMutex(mutex);

            // --- ГЕНИАЛЬНАЯ ЧАСТЬ АЛГОРИТМА ---
            // Мы "изъяли" число только у потока nomer_min_faila. 
            // Значит, новые данные нам нужны ТОЛЬКО от него.
            // Остальные потоки свои числа не потратили, их просить читать не нужно!

            // Даем команду потоку, чье число мы забрали: "Читай следующее!"
            SetEvent(sobytiya_mozhno_chitat[nomer_min_faila]);

            // Сбрасываем его флаг готовности, так как новые данные от него мы еще не получили
            ResetEvent(sobytiya_dannye_gotovy[nomer_min_faila]);
        }
    }
    return 0;
}


// Вспомогательная функция: вручную создает строки вида "file1.txt" или "file10.txt"
void sozdat_imya_faila(char* bufer, int nomer)
{
    bufer[0] = 'f'; bufer[1] = 'i'; bufer[2] = 'l'; bufer[3] = 'e';
    if (nomer < 10)
    {
        bufer[4] = '0' + nomer; // Превращаем цифру в символ ('1', '2'...)
        bufer[5] = '.'; bufer[6] = 't'; bufer[7] = 'x'; bufer[8] = 't'; bufer[9] = '\0';
    }
    else
    {
        bufer[4] = '0' + nomer / 10; // Десятки
        bufer[5] = '0' + nomer % 10; // Единицы
        bufer[6] = '.'; bufer[7] = 't'; bufer[8] = 'x'; bufer[9] = 't'; bufer[10] = '\0';
    }
}


// =========================================================================
// ГЛАВНАЯ ФУНКЦИЯ
// =========================================================================
int main()
{
    setlocale(LC_ALL, "Russian");

    cout << "Введите количество файлов для слияния (максимум " << MAX_FILES << "): ";
    cin >> kolichestvo_failov;
    if (kolichestvo_failov <= 0 || kolichestvo_failov > MAX_FILES)
    {
        cout << "Неверное количество файлов!" << endl;
        return 1;
    }

    // 1. ГЕНЕРАЦИЯ ТЕСТОВЫХ ДАННЫХ
    // Создаем запрошенное количество файлов и заполняем каждый 5-ю отсортированными числами
    cout << "\nСозданы файлы с тестовыми данными:" << endl;
    for (int i = 0; i < kolichestvo_failov; i++)
    {
        char imya_faila[20];
        sozdat_imya_faila(imya_faila, i + 1);
        ofstream f(imya_faila);
        for (int j = 0; j < 5; j++)
        {
            // Формула генерирует числа по порядку. 
            // Для файла 1: 1, 2, 3, 4, 5. Для файла 2: 6, 7, 8, 9, 10 и т.д.
            int chislo = i * 5 + j + 1;
            f << chislo << " ";
        }
        f.close();

        // Вывод сгенерированных данных в консоль
        cout << imya_faila << ": ";
        for (int j = 0; j < 5; j++) cout << (i * 5 + j + 1) << " ";
        cout << endl;
    }

    // 2. ИНИЦИАЛИЗАЦИЯ
    for (int i = 0; i < MAX_FILES; i++)
    {
        faili_konec[i] = false;
        chisla_iz_failov[i] = 0;
    }

    // Открываем созданные файлы для чтения
    for (int i = 0; i < kolichestvo_failov; i++)
    {
        char imya_faila[20];
        sozdat_imya_faila(imya_faila, i + 1);
        faili[i].open(imya_faila);
    }
    resultat_fail.open("result.txt"); // Открываем файл для результатов

    // 3. СОЗДАНИЕ ПРИМИТИВОВ СИНХРОНИЗАЦИИ
    sobytie_rabota_okonchena = CreateEvent(NULL, TRUE, FALSE, NULL);
    mutex = CreateMutex(NULL, FALSE, NULL); // FALSE - мьютекс изначально свободен

    for (int i = 0; i < kolichestvo_failov; i++)
    {
        // Сигналы готовности изначально опущены (FALSE)
        sobytiya_dannye_gotovy[i] = CreateEvent(NULL, TRUE, FALSE, NULL);

        // ВАЖНО: Разрешение на чтение изначально ПОДНЯТО (TRUE).
        // Это сделано для того, чтобы сразу после старта потоки немедленно считали первые числа.
        sobytiya_mozhno_chitat[i] = CreateEvent(NULL, TRUE, TRUE, NULL);
    }

    // 4. ЗАПУСК ПОТОКОВ
    HANDLE potoki_chteniya[MAX_FILES];
    int nomera_potokov[MAX_FILES];

    // Создаем потоки-чтецы
    for (int i = 0; i < kolichestvo_failov; i++)
    {
        nomera_potokov[i] = i; // Сохраняем номер, чтобы передать его в поток
        potoki_chteniya[i] = CreateThread(NULL, 0, chtenie_iz_faila, &nomera_potokov[i], 0, NULL);
    }

    // Создаем поток-сборщик
    HANDLE potok_result = CreateThread(NULL, 0, formirovanie_rezultata, NULL, 0, NULL);

    // 5. ОЖИДАНИЕ И ЗАВЕРШЕНИЕ
    // Главная программа замирает и ждет, пока поток-сборщик полностью не закончит работу
    WaitForSingleObject(potok_result, INFINITE);

    // Закрываем файлы
    for (int i = 0; i < kolichestvo_failov; i++) faili[i].close();
    resultat_fail.close();

    // 6. ПРОВЕРКА РЕЗУЛЬТАТА
    cout << "\nСлияние завершено. Результат сохранен в файл result.txt" << endl;
    ifstream resultat("result.txt");
    cout << "Содержимое результирующего файла: ";
    int chislo;
    while (resultat >> chislo)
    {
        cout << chislo << " ";
    }
    resultat.close();
    cout << endl;

    // 7. ОЧИСТКА ПАМЯТИ И ДЕСКРИПТОРОВ
    CloseHandle(potok_result);
    for (int i = 0; i < kolichestvo_failov; i++)
    {
        CloseHandle(potoki_chteniya[i]);
        CloseHandle(sobytiya_dannye_gotovy[i]);
        CloseHandle(sobytiya_mozhno_chitat[i]);
    }
    CloseHandle(sobytie_rabota_okonchena);
    CloseHandle(mutex);

    return 0;
}
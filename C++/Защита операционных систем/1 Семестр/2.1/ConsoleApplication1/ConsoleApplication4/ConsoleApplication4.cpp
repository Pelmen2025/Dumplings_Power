#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <locale>
#include <ctime>

using namespace std;

const int MAX_ELEMS = 1000;

string dek[MAX_ELEMS];
int head = 0;
int tail = 0;
LONG countDek = 0;

string elems[MAX_ELEMS];
int totalElems = 0;
int idxRead = 0;

CRITICAL_SECTION cs_head;
CRITICAL_SECTION cs_tail;
CRITICAL_SECTION cs_file;

int n_potokov = 0;

// Добавление элемента в начало дека
void push_front_simple(const string& s)
{
    head = (head - 1 + MAX_ELEMS) % MAX_ELEMS; // сдвигаем голову назад по кругу
    dek[head] = s;
    InterlockedIncrement(&countDek);
}

// Добавление элемента в конец дека
void push_back_simple(const string& s)
{
    dek[tail] = s;
    tail = (tail + 1) % MAX_ELEMS; // сдвигаем хвост вперёд по кругу
    InterlockedIncrement(&countDek);
}

DWORD WINAPI potok_dobavlyaet(LPVOID param)
{
    int id = (int)(size_t)param;
    while (true)
    {
        // Заходим в критическую секцию — теперь только этот поток имеет право
        // читать и изменять idxRead и elems[]
        EnterCriticalSection(&cs_file);
        if (idxRead >= totalElems) // Проверяем, остались ли ещё задачи
        {
            LeaveCriticalSection(&cs_file);
            break;
        }
        string s = elems[idxRead];
        idxRead++;
        LeaveCriticalSection(&cs_file);

        // Если дек уже заполнен — выходим
        if (InterlockedCompareExchange(&countDek, 0, 0) >= MAX_ELEMS)
            break;

        // Случайно выбираем: в начало (0) или в конец (1)
        int where = rand() % 2;

        if (where == 0)
        {
            EnterCriticalSection(&cs_head);
            if (InterlockedCompareExchange(&countDek, 0, 0) < MAX_ELEMS)
            {
                push_front_simple(s);
                cout << "Поток " << id << " вставил в начало: " << s << endl;
            }
            else
            {
                cout << "Поток " << id << ": нет места для вставки в начало\n";
            }
            LeaveCriticalSection(&cs_head);
        }
        else {
            EnterCriticalSection(&cs_tail);
            if (InterlockedCompareExchange(&countDek, 0, 0) < MAX_ELEMS)
            {
                push_back_simple(s);
                cout << "Поток " << id << " вставил в конец: " << s << endl;
            }
            else
            {
                cout << "Поток " << id << ": нет места для вставки в конец\n";
            }
            LeaveCriticalSection(&cs_tail);
        }
    }

    cout << "Поток " << id << " завершил работу.\n";
    return 0;
}

int main()
{
    setlocale(LC_ALL, "");
    srand((unsigned)time(NULL));

    cout << "Введите путь к файлу с элементами (по строкам): ";
    string filename;
    getline(cin, filename);

    if (filename.length() == 0)
    {
        cout << "Файл не указан, используются тестовые данные.\n";
        elems[0] = "первый";
        elems[1] = "второй";
        elems[2] = "третий";
        elems[3] = "четвёртый";
        elems[4] = "пятый";
        totalElems = 5;
    }
    else {
        ifstream fin(filename.c_str());
        if (!fin)
        {
            cout << "Не удалось открыть файл: " << filename << endl;
            return 1;
        }
        totalElems = 0;
        string line;
        while (totalElems < MAX_ELEMS && getline(fin, line))
        {
            elems[totalElems] = line;
            totalElems++;
        }
        fin.close();
        if (totalElems == 0)
        {
            cout << "Файл пуст. Завершение.\n";
            return 0;
        }
    }

    cout << "Прочитано элементов: " << totalElems << endl;
    cout << "Введите количество потоков n: ";
    cin >> n_potokov;

    if (n_potokov <= 0) n_potokov = 1;
    if (n_potokov > 64) n_potokov = 64;

    head = MAX_ELEMS / 2;
    tail = head;
    InterlockedExchange(&countDek, 0);

    InitializeCriticalSection(&cs_head);
    InitializeCriticalSection(&cs_tail);
    InitializeCriticalSection(&cs_file);

    HANDLE hThreads[64];
    for (int i = 0; i < n_potokov; ++i)
    {
        hThreads[i] = CreateThread(NULL, 0, potok_dobavlyaet, (LPVOID)(size_t)i, 0, NULL);
    }

    WaitForMultipleObjects(n_potokov, hThreads, TRUE, INFINITE);

    for (int i = 0; i < n_potokov; ++i)
        CloseHandle(hThreads[i]);
    int cnt = (int)InterlockedCompareExchange(&countDek, 0, 0);
    cout << "\nДек содержит " << cnt << " элементов:\n";
    int pos = head;
    for (int i = 0; i < cnt; ++i)
    {
        cout << dek[pos] << endl;
        pos = (pos + 1) % MAX_ELEMS;
    }

    DeleteCriticalSection(&cs_head);
    DeleteCriticalSection(&cs_tail);
    DeleteCriticalSection(&cs_file);

    cout << "Готово. Нажмите Enter для выхода...";
    string dummy;
    getline(cin, dummy);
    getline(cin, dummy);
    return 0;
}

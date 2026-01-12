#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <locale.h>

using namespace std;

struct Uzol
{
    int chislo;
    Uzol* sleduyuschiy;
};

class CMonitor
{
public:
    int* occupated_rows;
    HANDLE* mutexyStrok;
    int kol_strok;

    CMonitor(int kol)
    {
        kol_strok = kol;
        occupated_rows = new int[kol];
        mutexyStrok = new HANDLE[kol];
        for (int i = 0; i < kol; i++)
        {
            occupated_rows[i] = 0;
            mutexyStrok[i] = CreateMutex(NULL, FALSE, NULL);
        }
    }

    ~CMonitor()
    {
        for (int i = 0; i < kol_strok; i++)
        {
            CloseHandle(mutexyStrok[i]);
        }
        delete[] occupated_rows;
        delete[] mutexyStrok;
    }

    void OcupateRow(int row_number)
    {
        WaitForSingleObject(mutexyStrok[row_number], INFINITE);
        occupated_rows[row_number] = 1;
    }

    void FreeRow(int row_number)
    {
        occupated_rows[row_number] = 0;
        ReleaseMutex(mutexyStrok[row_number]);
    }
};

Uzol** hashTablitsa;

int hashFunktsiya(int x, int osnovanie)
{
    return x % osnovanie;
}

struct PotokParam
{
    CMonitor* monitor;
    int osnovanie;
    int id;
};

DWORD WINAPI generiruyIPishi(LPVOID param)
{
    PotokParam* p = (PotokParam*)param;
    CMonitor* monitor = p->monitor;
    int osnovanie = p->osnovanie;
    int id = p->id;
    srand(GetTickCount() + id * 100);
    for (int i = 0; i < 10; i++)
    {
        int x = rand() % 100;
        int k = hashFunktsiya(x, osnovanie);
        monitor->OcupateRow(k);
        Uzol* novyy = new Uzol;
        novyy->chislo = x;
        novyy->sleduyuschiy = hashTablitsa[k];
        hashTablitsa[k] = novyy;
        monitor->FreeRow(k);
    }
    return 0;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    int osnovanie;
    cout << "Введите основание хеш-функции (количество строк в таблице): ";
    cin >> osnovanie;
    if (osnovanie <= 0)
    {
        cout << "Основание должно быть больше 0!" << endl;
        return 1;
    }
    int kol_strok = osnovanie;
    hashTablitsa = new Uzol * [kol_strok];
    for (int i = 0; i < kol_strok; i++)
    {
        hashTablitsa[i] = NULL;
    }
    CMonitor monitor(kol_strok);
    int n = 5;
    HANDLE potoki[5];
    PotokParam paramy[5];
    for (int i = 0; i < n; i++)
    {
        paramy[i].monitor = &monitor;
        paramy[i].osnovanie = osnovanie;
        paramy[i].id = i;
        potoki[i] = CreateThread(NULL, 0, generiruyIPishi, (LPVOID)&paramy[i], 0, NULL);
    }
    WaitForMultipleObjects(n, potoki, TRUE, INFINITE);
    for (int i = 0; i < n; i++)
    {
        CloseHandle(potoki[i]);
    }
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

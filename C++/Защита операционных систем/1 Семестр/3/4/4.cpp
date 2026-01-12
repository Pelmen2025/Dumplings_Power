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

Uzol* hashTablitsa[10];

HANDLE hMutexStroki[10];

int hashFunktsiya(int x)
{
    return x % 10;
}

DWORD WINAPI generiruyIPishi(LPVOID param)
{
    int id = (int)param;
    srand(GetTickCount() + id * 100);
    for (int i = 0; i < 10; i++)
    {
        int x = rand() % 100;
        int k = hashFunktsiya(x);
        WaitForSingleObject(hMutexStroki[k], INFINITE);
        Uzol* novyy = new Uzol;
        novyy->chislo = x;
        novyy->sleduyuschiy = hashTablitsa[k];
        hashTablitsa[k] = novyy;
        ReleaseMutex(hMutexStroki[k]);
    }

    return 0;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    for (int i = 0; i < 10; i++)
    {
        hashTablitsa[i] = NULL;
        hMutexStroki[i] = CreateMutex(NULL, FALSE, NULL);
    }
    int n = 5;
    HANDLE potoki[5];
    for (int i = 0; i < n; i++) {
        potoki[i] = CreateThread(NULL, 0, generiruyIPishi, (LPVOID)i, 0, NULL);
    }
    WaitForMultipleObjects(n, potoki, TRUE, INFINITE);
    for (int i = 0; i < n; i++)
    {
        CloseHandle(potoki[i]);
    }
    for (int i = 0; i < 10; i++)
    {
        CloseHandle(hMutexStroki[i]);
    }
    cout << "Хеш-таблица:" << endl;
    for (int i = 0; i < 10; i++)
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
    for (int i = 0; i < 10; i++)
    {
        Uzol* tek = hashTablitsa[i];
        while (tek != NULL)
        {
            Uzol* sleduyuschiy = tek->sleduyuschiy;
            delete tek;
            tek = sleduyuschiy;
        }
    }
    return 0;
}

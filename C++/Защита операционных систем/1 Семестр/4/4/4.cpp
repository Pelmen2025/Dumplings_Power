#include <windows.h>
#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

const int n = 1024;
const int m = 3;

char pul[n];
int svobodno = n;
int zapolnеnо = 0;

HANDLE semSvobodnye;
HANDLE semZapolnennye;
HANDLE mutexPul;

FILE* vyhodnoj_fail;

DWORD WINAPI potok_pechati(LPVOID param)
{
    intptr_t nomer_ptr = (intptr_t)param;
    int nomer = (int)nomer_ptr;
    char imya_faila[20];
    sprintf_s(imya_faila, sizeof(imya_faila), "input%d.txt", nomer);
    FILE* vhod_fail = NULL;
    errno_t err = fopen_s(&vhod_fail, imya_faila, "r");
    if (err != 0 || !vhod_fail)
    {
        cout << "Не могу открыть " << imya_faila << endl;
        return 0;
    }

    char stroka[100];

    while (fgets(stroka, sizeof(stroka), vhod_fail))
    {
        char* ptr = stroka;
        int dlina = (int)strlen(stroka);
        WaitForSingleObject(mutexPul, INFINITE);
        while (dlina > 0)
        {
            while (svobodno < 1)
            {
                ReleaseMutex(mutexPul);
                WaitForSingleObject(semSvobodnye, INFINITE);
                WaitForSingleObject(mutexPul, INFINITE);
            }
            int mogu_zapisat = (dlina < svobodno) ? dlina : svobodno;
            memcpy(pul + (n - svobodno), ptr, mogu_zapisat);
            svobodno -= mogu_zapisat;
            zapolnеnо += mogu_zapisat;
            dlina -= mogu_zapisat;
            ptr += mogu_zapisat;
            ReleaseSemaphore(semZapolnennye, mogu_zapisat, NULL);
        }
        ReleaseMutex(mutexPul);
    }
    fclose(vhod_fail);
    cout << "Поток печати " << nomer << " завершился\n";
    return 0;
}

DWORD WINAPI potok_upravleniya(LPVOID)
{
    while (true)
    {
        WaitForSingleObject(semZapolnennye, INFINITE);
        WaitForSingleObject(mutexPul, INFINITE);
        if (zapolnеnо == 0)
        {
            ReleaseMutex(mutexPul);
            continue;
        }
        char c = pul[0];
        fputc(c, vyhodnoj_fail);
        memmove(pul, pul + 1, zapolnеnо - 1);
        zapolnеnо--;
        svobodno++;
        ReleaseSemaphore(semSvobodnye, 1, NULL);
        ReleaseMutex(mutexPul);
    }
    return 0;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    cout << "Объем пула: " << n << ", потоков печати: " << m << endl << endl;
    errno_t err_out = fopen_s(&vyhodnoj_fail, "output_print.txt", "w");
    if (err_out != 0 || !vyhodnoj_fail)
    {
        cout << "Не могу создать output_print.txt\n";
        return 1;
    }
    semSvobodnye = CreateSemaphore(NULL, n, n, NULL);
    semZapolnennye = CreateSemaphore(NULL, 0, n, NULL);
    mutexPul = CreateMutex(NULL, FALSE, NULL);
    HANDLE potoki[m + 1];
    for (int i = 0; i < m; i++)
    {
        potoki[i] = CreateThread(NULL, 0, potok_pechati, (LPVOID)(intptr_t)(i + 1), 0, NULL);
    }
    potoki[m] = CreateThread(NULL, 0, potok_upravleniya, NULL, 0, NULL);
    WaitForMultipleObjects(m, potoki, TRUE, INFINITE);
    cout << "Все потоки печати завершились. Завершаем программу.\n";
    for (int i = 0; i < m + 1; i++)
    {
        CloseHandle(potoki[i]);
    }
    CloseHandle(semSvobodnye);
    CloseHandle(semZapolnennye);
    CloseHandle(mutexPul);
    fclose(vyhodnoj_fail);
    return 0;
}

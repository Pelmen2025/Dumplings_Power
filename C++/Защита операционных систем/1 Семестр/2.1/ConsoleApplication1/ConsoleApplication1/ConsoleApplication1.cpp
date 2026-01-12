#include <windows.h>
#include <iostream>
#include <string>
#include <locale>

using namespace std;

CRITICAL_SECTION krit_sekciya;
unsigned long long summa = 0;
string tekst;
int k = 0;

DWORD WINAPI potok(LPVOID param)
{
    int nomer = (int)(size_t)param;
    unsigned long long lokal = 0;

    for (int i = nomer; i < (int)tekst.length(); i += k)
    {
        unsigned char ch = (unsigned char)tekst[i];
        lokal += ch;
    }

    EnterCriticalSection(&krit_sekciya);
    summa += lokal;
    LeaveCriticalSection(&krit_sekciya);

    return 0;
}

int main()
{
    setlocale(LC_ALL, "");

    cout << "Введите текст: ";
    getline(cin, tekst);

    if (tekst.length() == 0)
    {
        cout << "Пустой текст. Завершение.\n";
        return 0;
    }

    cout << "Введите количество потоков (k < " << tekst.length() << "): ";
    cin >> k;

    if (k <= 0) k = 1;
    if (k >= (int)tekst.length()) k = (int)tekst.length() - 1;

    InitializeCriticalSection(&krit_sekciya);

    HANDLE potoki[64];
    if (k > 64) k = 64;

    for (int i = 0; i < k; i++)
    {
        potoki[i] = CreateThread(NULL, 0, potok, (LPVOID)(size_t)i, 0, NULL);
    }

    WaitForMultipleObjects(k, potoki, TRUE, INFINITE);

    for (int i = 0; i < k; i++)
    {
        CloseHandle(potoki[i]);
    }

    DeleteCriticalSection(&krit_sekciya);

    unsigned int kontrol = (unsigned int)(summa % 256);

    cout << "\nСумма кодов символов = " << summa << endl;
    cout << "Контрольная сумма (mod 256) = " << kontrol << endl;

    return 0;
}

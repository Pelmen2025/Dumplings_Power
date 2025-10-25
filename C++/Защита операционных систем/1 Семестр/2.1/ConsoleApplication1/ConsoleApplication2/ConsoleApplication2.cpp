#include <windows.h>
#include <iostream>
#include <locale>
#include <ctime>

using namespace std;

CRITICAL_SECTION krit_sekciya;
double matrica[20][20];
double summaObshaya = 0;
int n = 0, m = 0;

HANDLE potokiForm[20];
HANDLE potokiSum[20];
double summaStrok[20];

DWORD WINAPI potokZapolnyaet(LPVOID param)
{
    int i = (int)(size_t)param;

    for (int j = 0; j < m; j++)
    {
        matrica[i][j] = (rand() % 100) / 10.0;
    }

    ResumeThread(potokiSum[i]);

    return 0;
}

DWORD WINAPI potokSchitaet(LPVOID param)
{
    int i = (int)(size_t)param;
    double lokalSum = 0;

    for (int j = 0; j < m; j++)
    {
        lokalSum += matrica[i][j];
    }

    summaStrok[i] = lokalSum;

    EnterCriticalSection(&krit_sekciya);
    summaObshaya += lokalSum;
    LeaveCriticalSection(&krit_sekciya);

    return 0;
}

int main()
{
    setlocale(LC_ALL, "");
    srand((unsigned)time(NULL));

    cout << "Введите количество строк (n <= 20): ";
    cin >> n;
    cout << "Введите количество столбцов (m <= 20): ";
    cin >> m;

    if (n <= 0 || m <= 0)
    {
        cout << "Ошибка: размеры должны быть положительными.\n";
        return 0;
    }

    if (n > 20) n = 20;
    if (m > 20) m = 20;

    InitializeCriticalSection(&krit_sekciya);

    for (int i = 0; i < n; i++)
    {
        potokiSum[i] = CreateThread(
            NULL, 0, potokSchitaet, (LPVOID)(size_t)i,
            CREATE_SUSPENDED, NULL);
    }

    for (int i = 0; i < n; i++)
    {
        potokiForm[i] = CreateThread(
            NULL, 0, potokZapolnyaet, (LPVOID)(size_t)i,
            0, NULL);
    }

    WaitForMultipleObjects(n, potokiForm, TRUE, INFINITE);

    WaitForMultipleObjects(n, potokiSum, TRUE, INFINITE);

    for (int i = 0; i < n; i++)
    {
        CloseHandle(potokiForm[i]);
        CloseHandle(potokiSum[i]);
    }

    DeleteCriticalSection(&krit_sekciya);

    cout << "\nМатрица:\n";
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
            cout << matrica[i][j] << "\t";
        cout << endl;
    }

    cout << "\nОбщая сумма всех элементов = " << summaObshaya << endl;

    system("pause");
    return 0;
}

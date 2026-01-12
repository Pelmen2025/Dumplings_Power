#include <windows.h>
#include <iostream>
#include <locale>
#include <cmath>

using namespace std;

CRITICAL_SECTION krit_sekciya;
double a, b;
int n, m;
double ploshadObshaya = 0;

double f(double x)
{
    return sin(x) + 2.0;
}

// Каждый поток получает свой номер i и считает площадь только своего участка
DWORD WINAPI potokChast(LPVOID param)
{
    int i = (int)(size_t)param;           // номер потока (0, 1, 2, ..., n-1)

    double h_gl = (b - a) / n;            // ширина крупного куска
    double x1 = a + i * h_gl;             // левая граница этого куска
    double x2 = x1 + h_gl;                // правая граница

    double h_m = (x2 - x1) / m;           // ширина маленького прямоугольника
    double lokalPloshad = 0;              // Площадь, посчитанная только этим потоком  

    // считаем площадь внутри своего крупного куска методом средних прямоугольников
    for (int j = 0; j < m; j++)
    {
        double x = x1 + j * h_m + h_m / 2.0; // середина маленького интервала
        lokalPloshad += f(x) * h_m;
    }

    EnterCriticalSection(&krit_sekciya);
    ploshadObshaya += lokalPloshad;
    LeaveCriticalSection(&krit_sekciya);

    return 0;
}

int main()
{
    setlocale(LC_ALL, "");
        
    cout << "Введите левую границу a: ";
    cin >> a;
    cout << "Введите правую границу b: ";
    cin >> b;
    cout << "Введите количество больших частей n: ";
    cin >> n;
    cout << "Введите количество прямоугольников m внутри каждой части: ";
    cin >> m;

    if (n <= 0 || m <= 0 || b <= a)
    {
        cout << "Ошибка ввода данных.\n";
        return 0;
    }

    if (n > 64) n = 64;


    InitializeCriticalSection(&krit_sekciya);

    HANDLE potoki[64];

    for (int i = 0; i < n; i++)
    {
        potoki[i] = CreateThread(NULL, 0, potokChast, (LPVOID)(size_t)i, 0, NULL);
    }

    WaitForMultipleObjects(n, potoki, TRUE, INFINITE);

    for (int i = 0; i < n; i++)
    {
        CloseHandle(potoki[i]);
    }

    DeleteCriticalSection(&krit_sekciya);

    cout << "\nПриближённая площадь под кривой y = sin(x) + 2" << endl;
    cout << "На отрезке [" << a << ", " << b << "] = " << ploshadObshaya << endl;

    return 0;
}

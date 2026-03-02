#include <windows.h>
#include <iostream>
#include <locale.h>
#include <cmath>

using namespace std;

double x;
int nomer_chlena;
double resultat_factorial;
double resultat_stepeni;
double summa_ryada;

HANDLE sobytie_start_factorial;
HANDLE sobytie_start_stepeni;
HANDLE sobytie_factorial_gotov;
HANDLE sobytie_stepeni_gotov;

DWORD WINAPI vychislenie_factoriala(LPVOID param)
{
    while (true)
    {
        WaitForSingleObject(sobytie_start_factorial, INFINITE);
        if (nomer_chlena == -1)
        {
            break;
        }
        double factorial = 1.0;
        for (int i = 1; i <= nomer_chlena; i++)
        {
            factorial *= i;
        }
        resultat_factorial = factorial;
        ResetEvent(sobytie_start_factorial);
        SetEvent(sobytie_factorial_gotov);
    }
    return 0;
}

DWORD WINAPI vychislenie_stepeni(LPVOID param)
{
    while (true)
    {
        WaitForSingleObject(sobytie_start_stepeni, INFINITE);
        if (nomer_chlena == -1)
        {
            break;
        }
        double stepen = 1.0;
        for (int i = 0; i < nomer_chlena; i++)
        {
            stepen *= x;
        }
        resultat_stepeni = stepen;
        ResetEvent(sobytie_start_stepeni);
        SetEvent(sobytie_stepeni_gotov);
    }
    return 0;
}

DWORD WINAPI summa_chlenov_ryada(LPVOID param)
{
    int kolichestvo_chlenov = *(int*)param;
    summa_ryada = 1.0;
    for (int chlen = 1; chlen <= kolichestvo_chlenov; chlen++)
    {
        nomer_chlena = chlen;
        SetEvent(sobytie_start_factorial);
        SetEvent(sobytie_start_stepeni);
        HANDLE sobytiya_dlya_ozhidaniya[2] = { sobytie_factorial_gotov, sobytie_stepeni_gotov };
        WaitForMultipleObjects(2, sobytiya_dlya_ozhidaniya, TRUE, INFINITE);
        double tekushchiy_chlen = resultat_stepeni / resultat_factorial;
        summa_ryada += tekushchiy_chlen;
        cout << "Член " << chlen << ": " << tekushchiy_chlen << endl;
        ResetEvent(sobytie_factorial_gotov);
        ResetEvent(sobytie_stepeni_gotov);
    }
    return 0;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    cout << "Программа вычисления значения функции e^x по ряду Маклорена" << endl;
    cout << "Введите значение аргумента x: ";
    cin >> x;
    int kolichestvo_chlenov;
    cout << "Введите количество членов ряда для вычисления: ";
    cin >> kolichestvo_chlenov;
    sobytie_start_factorial = CreateEvent(NULL, TRUE, FALSE, NULL);
    sobytie_start_stepeni = CreateEvent(NULL, TRUE, FALSE, NULL);
    sobytie_factorial_gotov = CreateEvent(NULL, TRUE, FALSE, NULL);
    sobytie_stepeni_gotov = CreateEvent(NULL, TRUE, FALSE, NULL);
    HANDLE potok_factorial = CreateThread(NULL, 0, vychislenie_factoriala, NULL, 0, NULL);
    HANDLE potok_stepeni = CreateThread(NULL, 0, vychislenie_stepeni, NULL, 0, NULL);
    HANDLE potok_summa = CreateThread(NULL, 0, summa_chlenov_ryada, &kolichestvo_chlenov, 0, NULL);
    WaitForSingleObject(potok_summa, INFINITE);
    cout << "\nРезультат вычисления:" << endl;
    cout << "e^" << x << " ~ " << summa_ryada << endl;
    cout << "Точное значение (через exp): " << exp(x) << endl;
    nomer_chlena = -1;
    SetEvent(sobytie_start_factorial);
    SetEvent(sobytie_start_stepeni);
    WaitForSingleObject(potok_factorial, INFINITE);
    WaitForSingleObject(potok_stepeni, INFINITE);
    CloseHandle(potok_summa);
    CloseHandle(potok_factorial);
    CloseHandle(potok_stepeni);
    CloseHandle(sobytie_start_factorial);
    CloseHandle(sobytie_start_stepeni);
    CloseHandle(sobytie_factorial_gotov);
    CloseHandle(sobytie_stepeni_gotov);
    return 0;
}

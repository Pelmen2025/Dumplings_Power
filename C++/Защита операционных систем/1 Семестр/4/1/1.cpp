#include <windows.h>
#include <iostream>
#include <locale.h>
#include <cstring>

using namespace std;

struct zapis
{
    char material[20];
    int kolichestvo;
};

int n;
int k;
int buf_size;

zapis* buffer;
int tek_pos = 0;

char itog_nazvaniya[100][20];
int itog_kol[100];
int itog_kolvo = 0;

HANDLE hSemPustye;
HANDLE hSemZanyatye;
HANDLE hMutex;

DWORD WINAPI ceh(LPVOID param)
{
    int nomer = *(int*)param;
    delete (int*)param;
    for (int i = 0; i < k; i++)
    {
        WaitForSingleObject(hSemPustye, INFINITE);
        WaitForSingleObject(hMutex, INFINITE);
        cout << "Цех " << nomer << ": введите название материала: ";
        cin >> buffer[tek_pos].material;
        cout << "Цех " << nomer << ": введите количество: ";
        while (!(cin >> buffer[tek_pos].kolichestvo))
        {
            cout << "Ошибка! Введите число: ";
            cin.clear();
            cin.ignore(1000, '\n');
        }
        tek_pos++;
        ReleaseMutex(hMutex);
        ReleaseSemaphore(hSemZanyatye, 1, NULL);
    }
    return 0;
}

DWORD WINAPI obrabotchik(LPVOID)
{
    int vsego = n * k;
    int obrabotano = 0;
    while (obrabotano < vsego)
    {
        WaitForSingleObject(hSemZanyatye, INFINITE);
        WaitForSingleObject(hMutex, INFINITE);
        tek_pos--;
        char* mat = buffer[tek_pos].material;
        int kol = buffer[tek_pos].kolichestvo;
        bool est = false;
        for (int i = 0; i < itog_kolvo; i++)
        {
            if (strcmp(itog_nazvaniya[i], mat) == 0)
            {
                itog_kol[i] += kol;
                est = true;
                break;
            }
        }
        if (!est)
        {
            strcpy_s(itog_nazvaniya[itog_kolvo], sizeof(itog_nazvaniya[itog_kolvo]), mat);
            itog_kol[itog_kolvo] = kol;
            itog_kolvo++;
        }
        obrabotano++;
        ReleaseMutex(hMutex);
        ReleaseSemaphore(hSemPustye, 1, NULL);
    }
    return 0;
}

int main()
{
    setlocale(LC_ALL, "");
    cout << "Введите количество цехов: ";
    cin >> n;
    cout << "Введите количество заявок для каждого цеха: ";
    cin >> k;
    buf_size = n * k;
    buffer = new zapis[buf_size];
    HANDLE* hCeh = new HANDLE[n];
    HANDLE hObr;
    hSemPustye = CreateSemaphore(NULL, buf_size, buf_size, NULL);
    hSemZanyatye = CreateSemaphore(NULL, 0, buf_size, NULL);
    hMutex = CreateMutex(NULL, FALSE, NULL);
    for (int i = 0; i < n; i++)
    {
        int* p = new int;
        *p = i + 1;

        hCeh[i] = CreateThread(NULL, 0, ceh, p, 0, NULL);
    }
    hObr = CreateThread(NULL, 0, obrabotchik, NULL, 0, NULL);
    WaitForMultipleObjects(n, hCeh, TRUE, INFINITE);
    WaitForSingleObject(hObr, INFINITE);
    cout << "\nИтоговая заявка:\n";
    for (int i = 0; i < itog_kolvo; i++)
    {
        cout << itog_nazvaniya[i] << " : " << itog_kol[i] << endl;
    }
    for (int i = 0; i < n; i++)
    {
        CloseHandle(hCeh[i]);
    }
    delete[] hCeh;
    CloseHandle(hObr);
    CloseHandle(hSemPustye);
    CloseHandle(hSemZanyatye);
    CloseHandle(hMutex);
    delete[] buffer;
    return 0;
}

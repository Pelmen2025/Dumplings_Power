#include <windows.h>
#include <iostream>
#include <locale.h>

using namespace std;

float* massiv;
int razmer_massiva;
HANDLE* sobitiya;

struct ParametriPotoka
{
    int nomer_prohoda;
};

DWORD WINAPI prohod_sortirovki(LPVOID param)
{
    ParametriPotoka* parametri = (ParametriPotoka*)param;
    int nomer_prohoda = parametri->nomer_prohoda;
    int kolichestvo_sravneniy = razmer_massiva - 1 - nomer_prohoda;
    for (int i = 0; i < kolichestvo_sravneniy; i++)
    {
        if (massiv[i] > massiv[i + 1])
        {
            float temp = massiv[i];
            massiv[i] = massiv[i + 1];
            massiv[i + 1] = temp;
        }
        if (i == (kolichestvo_sravneniy / 2) && nomer_prohoda < razmer_massiva - 2)
        {
            SetEvent(sobitiya[nomer_prohoda + 1]);
        }
    }
    return 0;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    cout << "Введите размер массива: ";
    cin >> razmer_massiva;
    massiv = new float[razmer_massiva];
    cout << "Введите элементы массива:" << endl;
    for (int i = 0; i < razmer_massiva; i++)
    {
        cout << "Элемент " << i + 1 << ": ";
        cin >> massiv[i];
    }
    cout << "\nИсходный массив:" << endl;
    for (int i = 0; i < razmer_massiva; i++)
    {
        cout << massiv[i] << " ";
    }
    cout << endl;
    sobitiya = new HANDLE[razmer_massiva - 1];
    for (int i = 0; i < razmer_massiva - 1; i++)
    {
        sobitiya[i] = CreateEvent(NULL, true, false, NULL);
    }
    SetEvent(sobitiya[0]);
    HANDLE* potoki = new HANDLE[razmer_massiva - 1];
    ParametriPotoka* parametri = new ParametriPotoka[razmer_massiva - 1];
    for (int i = 0; i < razmer_massiva - 1; i++)
    {
        parametri[i].nomer_prohoda = i;
        WaitForSingleObject(sobitiya[i], INFINITE);
        potoki[i] = CreateThread(NULL, 0, prohod_sortirovki, &parametri[i], 0, NULL);
        ResetEvent(sobitiya[i]);
    }
    WaitForMultipleObjects(razmer_massiva - 1, potoki, TRUE, INFINITE);
    cout << "\nОтсортированный массив:" << endl;
    for (int i = 0; i < razmer_massiva; i++)
    {
        cout << massiv[i] << " ";
    }
    cout << endl;
    for (int i = 0; i < razmer_massiva - 1; i++)
    {
        CloseHandle(potoki[i]);
        CloseHandle(sobitiya[i]);
    }
    delete[] massiv;
    delete[] sobitiya;
    delete[] potoki;
    delete[] parametri;
    return 0;
}

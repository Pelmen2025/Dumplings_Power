#include <windows.h>
#include <iostream>
#include <fstream>
#include <locale.h>

using namespace std;

#define MAX_FILES 10

int chisla_iz_failov[MAX_FILES];
bool faili_konec[MAX_FILES];
ifstream faili[MAX_FILES];
ofstream resultat_fail;
int kolichestvo_failov;

HANDLE sobytiya_dannye_gotovy[MAX_FILES];
HANDLE sobytiya_mozhno_chitat[MAX_FILES];
HANDLE sobytie_rabota_okonchena;
HANDLE mutex;

DWORD WINAPI chtenie_iz_faila(LPVOID param)
{
    int nomer_faila = *((int*)param);
    while (true)
    {
        WaitForSingleObject(sobytiya_mozhno_chitat[nomer_faila], INFINITE);
        if (WaitForSingleObject(sobytie_rabota_okonchena, 0) == WAIT_OBJECT_0)
        {
            break;
        }
        if (!faili_konec[nomer_faila])
        {
            if (faili[nomer_faila] >> chisla_iz_failov[nomer_faila])
            {
                WaitForSingleObject(mutex, INFINITE);
                cout << "Поток " << nomer_faila + 1 << ": прочитал число " << chisla_iz_failov[nomer_faila] << endl;
                ReleaseMutex(mutex);
                SetEvent(sobytiya_dannye_gotovy[nomer_faila]);
            }
            else
            {
                faili_konec[nomer_faila] = true;
                SetEvent(sobytiya_dannye_gotovy[nomer_faila]);
            }
        }
        ResetEvent(sobytiya_mozhno_chitat[nomer_faila]);
    }
    return 0;
}

DWORD WINAPI formirovanie_rezultata(LPVOID param)
{
    int n = kolichestvo_failov;
    while (true)
    {
        HANDLE sobytiya_dlya_ozhidaniya[MAX_FILES];
        for (int i = 0; i < n; i++)
        {
            sobytiya_dlya_ozhidaniya[i] = sobytiya_dannye_gotovy[i];
        }
        WaitForMultipleObjects(n, sobytiya_dlya_ozhidaniya, TRUE, INFINITE);
        bool vse_konec = true;
        for (int i = 0; i < n; i++)
        {
            if (!faili_konec[i])
            {
                vse_konec = false;
                break;
            }
        }
        if (vse_konec)
        {
            SetEvent(sobytie_rabota_okonchena);
            break;
        }
        int min_chislo = 2147483647;
        int nomer_min_faila = -1;
        for (int i = 0; i < n; i++)
        {
            if (!faili_konec[i] && chisla_iz_failov[i] < min_chislo)
            {
                min_chislo = chisla_iz_failov[i];
                nomer_min_faila = i;
            }
        }
        if (nomer_min_faila != -1)
        {
            resultat_fail << min_chislo << " ";
            WaitForSingleObject(mutex, INFINITE);
            cout << "Результат: записал " << min_chislo;
            int kol_konec = 0;
            for (int i = 0; i < n; i++)
            {
                if (faili_konec[i]) kol_konec++;
            }
            if (kol_konec > 0)
            {
                cout << " (некоторые файлы закончились)";
            }
            cout << endl;
            ReleaseMutex(mutex);
            SetEvent(sobytiya_mozhno_chitat[nomer_min_faila]);
            ResetEvent(sobytiya_dannye_gotovy[nomer_min_faila]);
        }
    }
    return 0;
}

void sozdat_imya_faila(char* bufer, int nomer)
{
    bufer[0] = 'f'; bufer[1] = 'i'; bufer[2] = 'l'; bufer[3] = 'e';
    if (nomer < 10)
    {
        bufer[4] = '0' + nomer;
        bufer[5] = '.';
        bufer[6] = 't';
        bufer[7] = 'x';
        bufer[8] = 't';
        bufer[9] = '\0';
    }
    else
    {
        bufer[4] = '0' + nomer / 10;
        bufer[5] = '0' + nomer % 10;
        bufer[6] = '.';
        bufer[7] = 't';
        bufer[8] = 'x';
        bufer[9] = 't';
        bufer[10] = '\0';
    }
}



int main()
{
    setlocale(LC_ALL, "Russian");
    cout << "Введите количество файлов для слияния (максимум " << MAX_FILES << "): ";
    cin >> kolichestvo_failov;
    if (kolichestvo_failov <= 0 || kolichestvo_failov > MAX_FILES)
    {
        cout << "Неверное количество файлов!" << endl;
        return 1;
    }
    cout << "\nСозданы файлы с тестовыми данными:" << endl;
    for (int i = 0; i < kolichestvo_failov; i++)
    {
        char imya_faila[20];
        sozdat_imya_faila(imya_faila, i + 1);
        ofstream f(imya_faila);
        for (int j = 0; j < 5; j++)
        {
            int chislo = i * 5 + j + 1;
            f << chislo << " ";
        }
        f.close();
        cout << imya_faila << ": ";
        for (int j = 0; j < 5; j++)
        {
            cout << (i * 5 + j + 1) << " ";
        }
        cout << endl;
    }
    for (int i = 0; i < MAX_FILES; i++)
    {
        faili_konec[i] = false;
        chisla_iz_failov[i] = 0;
    }
    for (int i = 0; i < kolichestvo_failov; i++)
    {
        char imya_faila[20];
        sozdat_imya_faila(imya_faila, i + 1);
        faili[i].open(imya_faila);
    }
    resultat_fail.open("result.txt");
    sobytie_rabota_okonchena = CreateEvent(NULL, TRUE, FALSE, NULL);
    mutex = CreateMutex(NULL, FALSE, NULL);
    for (int i = 0; i < kolichestvo_failov; i++)
    {
        sobytiya_dannye_gotovy[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        sobytiya_mozhno_chitat[i] = CreateEvent(NULL, TRUE, TRUE, NULL);
    }
    HANDLE potoki_chteniya[MAX_FILES];
    int nomera_potokov[MAX_FILES];
    for (int i = 0; i < kolichestvo_failov; i++)
    {
        nomera_potokov[i] = i;
        potoki_chteniya[i] = CreateThread(NULL, 0, chtenie_iz_faila, &nomera_potokov[i], 0, NULL);
    }
    HANDLE potok_result = CreateThread(NULL, 0, formirovanie_rezultata, NULL, 0, NULL);
    WaitForSingleObject(potok_result, INFINITE);
    for (int i = 0; i < kolichestvo_failov; i++)
    {
        faili[i].close();
    }
    resultat_fail.close();
    cout << "\nСлияние завершено. Результат сохранен в файл result.txt" << endl;
    ifstream resultat("result.txt");
    cout << "Содержимое результирующего файла: ";
    int chislo;
    while (resultat >> chislo)
    {
        cout << chislo << " ";
    }
    resultat.close();
    cout << endl;
    CloseHandle(potok_result);
    for (int i = 0; i < kolichestvo_failov; i++)
    {
        CloseHandle(potoki_chteniya[i]);
        CloseHandle(sobytiya_dannye_gotovy[i]);
        CloseHandle(sobytiya_mozhno_chitat[i]);
    }
    CloseHandle(sobytie_rabota_okonchena);
    CloseHandle(mutex);
    return 0;
}

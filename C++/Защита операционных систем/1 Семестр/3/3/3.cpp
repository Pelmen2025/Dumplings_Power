#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <locale.h>

using namespace std;

HANDLE hEventBufferSvoboden;
HANDLE hEventChitanieGotovo;
HANDLE hEventDeshifrovanieGotovo;
HANDLE hMutexDostupKBufferu;

const int buf_size = 20;
char buffer[buf_size];
char key[100];
int k = 0;
int dlina_buffer = 0;
bool konecFayla = false;

DWORD WINAPI chitayFayl(LPVOID param)
{
    ifstream fayl("output.txt");
    if (!fayl)
    {
        cout << "Ошибка открытия файла\n";
        return 0;
    }
    while (true)
    {
        WaitForSingleObject(hEventBufferSvoboden, INFINITE);
        WaitForSingleObject(hMutexDostupKBufferu, INFINITE);
        dlina_buffer = 0;
        while (dlina_buffer < buf_size && fayl.good())
        {
            int code;
            if (fayl >> code)
            {
                buffer[dlina_buffer] = (char)(unsigned char)code;
                dlina_buffer++;
            }
            else
            {
                break;
            }
        }
        if (dlina_buffer == 0 && fayl.eof())
        {
            konecFayla = true;
            ReleaseMutex(hMutexDostupKBufferu);
            SetEvent(hEventChitanieGotovo);
            break;
        }
        ReleaseMutex(hMutexDostupKBufferu);
        SetEvent(hEventChitanieGotovo);
    }
    fayl.close();
    return 0;
}

DWORD WINAPI deshifruyText(LPVOID param)
{
    while (true)
    {
        WaitForSingleObject(hEventChitanieGotovo, INFINITE);
        WaitForSingleObject(hMutexDostupKBufferu, INFINITE);
        if (konecFayla)
        {
            ReleaseMutex(hMutexDostupKBufferu);
            SetEvent(hEventDeshifrovanieGotovo);
            break;
        }
        int n = dlina_buffer;
        int blocks = (n + k - 1) / k;
        for (int b = 0; b < blocks; b++)
        {
            int start = b * k;
            int end = (b + 1) * k;
            if (end > n) end = n;
            for (int i = 0; i < end - start; i++)
            {
                buffer[start + i] ^= key[i];
            }
            for (int i = 0; i < (end - start) / 2; i++)
            {
                char temp = buffer[start + i];
                buffer[start + i] = buffer[end - 1 - i];
                buffer[end - 1 - i] = temp;
            }
        }
        ReleaseMutex(hMutexDostupKBufferu);
        SetEvent(hEventDeshifrovanieGotovo);
    }
    return 0;
}

DWORD WINAPI pishiVFayl(LPVOID param)
{
    ofstream vyhod("deshifrovanny.txt", ios::binary);
    if (!vyhod)
    {
        cout << "Ошибка открытия выходного файла\n";
        return 0;
    }
    while (true)
    {
        WaitForSingleObject(hEventDeshifrovanieGotovo, INFINITE);
        WaitForSingleObject(hMutexDostupKBufferu, INFINITE);
        if (konecFayla)
        {
            ReleaseMutex(hMutexDostupKBufferu);
            break;
        }
        vyhod.write(buffer, dlina_buffer);
        ReleaseMutex(hMutexDostupKBufferu);
        SetEvent(hEventBufferSvoboden);
    }
    vyhod.close();
    return 0;
}



int main()
{
    setlocale(LC_ALL, "Russian");
    cout << "Введите ключ: ";
    cin.getline(key, 100);
    k = strlen(key);
    if (k == 0)
    {
        cout << "Ключ пустой!\n";
        return 1;
    }
    hEventBufferSvoboden = CreateEvent(NULL, FALSE, TRUE, NULL);
    hEventChitanieGotovo = CreateEvent(NULL, FALSE, FALSE, NULL);
    hEventDeshifrovanieGotovo = CreateEvent(NULL, FALSE, FALSE, NULL);
    hMutexDostupKBufferu = CreateMutex(NULL, FALSE, NULL);
    HANDLE hChitay = CreateThread(NULL, 0, chitayFayl, NULL, 0, NULL);
    HANDLE hDeshifr = CreateThread(NULL, 0, deshifruyText, NULL, 0, NULL);
    HANDLE hPishi = CreateThread(NULL, 0, pishiVFayl, NULL, 0, NULL);
    HANDLE potoki[3] = { hChitay, hDeshifr, hPishi };
    WaitForMultipleObjects(3, potoki, TRUE, INFINITE);
    CloseHandle(hChitay);
    CloseHandle(hDeshifr);
    CloseHandle(hPishi);
    CloseHandle(hEventBufferSvoboden);
    CloseHandle(hEventChitanieGotovo);
    CloseHandle(hEventDeshifrovanieGotovo);
    CloseHandle(hMutexDostupKBufferu);
    cout << "Дешифрование завершено. Результат в deshifrovanny.txt\n";
    return 0;
}

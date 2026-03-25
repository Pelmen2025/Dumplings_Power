#include <windows.h>
#include <iostream>
#include <cstdio>
#include <cctype>
#include <cstring>

using namespace std;

const int RAZMER_BUFERA = 50;
const int KOLVO_DESHIFROVSHIKOV = 4;

char bufer[RAZMER_BUFERA];
int zapisano = 0;
int gotovo = 0;
int pos_zapis = 0;
int pos_chten = 0;

HANDLE semPustye;
HANDLE semSyrye;
HANDLE semGotovye;
HANDLE mutex;

FILE* failIn = NULL;
FILE* failOut = NULL;

DWORD WINAPI PotokChitatel(LPVOID)
{
    char c;
    while ((c = fgetc(failIn)) != EOF)
    {
        WaitForSingleObject(semPustye, INFINITE);
        WaitForSingleObject(mutex, INFINITE);
        bufer[pos_zapis] = c;
        pos_zapis = (pos_zapis + 1) % RAZMER_BUFERA;
        zapisano++;
        ReleaseMutex(mutex);
        ReleaseSemaphore(semSyrye, 1, NULL);
    }
    for (int i = 0; i < KOLVO_DESHIFROVSHIKOV; i++)
    {
        ReleaseSemaphore(semSyrye, 1, NULL);
    }
    cout << "Чтение из файла завершено\n";
    return 0;
}

DWORD WINAPI PotokDeshifrovshik(LPVOID)
{
    while (true)
    {
        if (WaitForSingleObject(semSyrye, 5000) == WAIT_TIMEOUT)
        {
            break;
        }

        WaitForSingleObject(mutex, INFINITE);
        if (zapisano == 0)
        {
            ReleaseMutex(mutex);
            break;
        }
        int idx = (pos_chten + gotovo) % RAZMER_BUFERA;
        char c = bufer[idx];

        // Логика дешифрования
        if (c >= 'A' && c <= 'Z')
        {
            c = (c == 'A') ? 'Z' : c - 1;
        }
        else if (c >= 'a' && c <= 'z')
        {
            c = (c == 'a') ? 'z' : c - 1;
        }

        bufer[idx] = c;
        gotovo++;
        zapisano--;
        ReleaseMutex(mutex);
        ReleaseSemaphore(semGotovye, 1, NULL);
    }
    return 0;
}

DWORD WINAPI PotokPisatel(LPVOID)
{
    while (true)
    {
        if (WaitForSingleObject(semGotovye, 5000) == WAIT_TIMEOUT && gotovo == 0)
        {
            break;
        }
        WaitForSingleObject(mutex, INFINITE);
        if (gotovo == 0)
        {
            ReleaseMutex(mutex);
            continue;
        }
        char c = bufer[pos_chten];
        pos_chten = (pos_chten + 1) % RAZMER_BUFERA;
        gotovo--;
        ReleaseMutex(mutex);
        fputc(c, failOut);
        ReleaseSemaphore(semPustye, 1, NULL);
    }
    cout << "Запись в файл завершена\n";
    return 0;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    memset(bufer, 0, sizeof(bufer));
    cout << "Дешифрование текста (семафоры)\n\n";

    errno_t e1 = fopen_s(&failIn, "output.txt", "r");
    errno_t e2 = fopen_s(&failOut, "decrypted.txt", "w");
    if (e1 || !failIn || e2 || !failOut)
    {
        cout << "Ошибка с файлами!\n";
        return 1;
    }

    semPustye = CreateSemaphore(NULL, RAZMER_BUFERA, RAZMER_BUFERA, NULL);
    semSyrye = CreateSemaphore(NULL, 0, RAZMER_BUFERA, NULL);
    semGotovye = CreateSemaphore(NULL, 0, RAZMER_BUFERA, NULL);
    mutex = CreateMutex(NULL, FALSE, NULL);

    HANDLE h[6];
    h[0] = CreateThread(NULL, 0, PotokChitatel, NULL, 0, NULL);
    for (int i = 1; i <= KOLVO_DESHIFROVSHIKOV; i++)
    {
        h[i] = CreateThread(NULL, 0, PotokDeshifrovshik, NULL, 0, NULL);
    }
    h[5] = CreateThread(NULL, 0, PotokPisatel, NULL, 0, NULL);

    WaitForMultipleObjects(6, h, TRUE, INFINITE);

    for (int i = 0; i < 6; i++) CloseHandle(h[i]);
    CloseHandle(semPustye);
    CloseHandle(semSyrye);
    CloseHandle(semGotovye);
    CloseHandle(mutex);

    fclose(failIn);
    fclose(failOut);

    return 0;
}
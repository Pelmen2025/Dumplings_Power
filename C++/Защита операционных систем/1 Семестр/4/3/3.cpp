#include <windows.h>
#include <iostream>
#include <cstdio>
#include <cstring>

using namespace std;

const int M = 50;
const int N = 4;

char bufer[M];
int in = 0;
int writer_out = 0;
int out = 0;
int buf_count = 0;
int de_count = 0;
bool done_reading = false;

HANDLE semEmpty;
HANDLE semFull;
HANDLE semDone;
HANDLE mutex;

FILE* fin;
FILE* fout;

DWORD WINAPI reader(LPVOID)
{
    int c;
    while ((c = fgetc(fin)) != EOF)
    {
        WaitForSingleObject(semEmpty, INFINITE);
        WaitForSingleObject(mutex, INFINITE);
        bufer[in] = (char)c;
        in = (in + 1) % M;
        buf_count++;
        ReleaseMutex(mutex);
        ReleaseSemaphore(semFull, 1, NULL);
    }
    WaitForSingleObject(mutex, INFINITE);
    done_reading = true;
    ReleaseMutex(mutex);
    ReleaseSemaphore(semFull, N, NULL);
    cout << "Чтение из файла завершено\n";
    return 0;
}

DWORD WINAPI worker(LPVOID)
{
    while (true)
    {
        if (WaitForSingleObject(semFull, 3000) == WAIT_TIMEOUT)
        {
            break;
        }
        WaitForSingleObject(mutex, INFINITE);
        if (buf_count == 0)
        {
            if (done_reading)
            {
                ReleaseMutex(mutex);
                break;
            }
            ReleaseMutex(mutex);
            continue;
        }
        char ch = bufer[out];
        if (ch >= 'A' && ch <= 'Z')
        {
            ch = (ch == 'A') ? 'Z' : ch - 1;
        }
        else if (ch >= 'a' && ch <= 'z')
        {
            ch = (ch == 'a') ? 'z' : ch - 1;
        }
        bufer[out] = ch;
        out = (out + 1) % M;
        buf_count--;
        de_count++;
        ReleaseMutex(mutex);
        ReleaseSemaphore(semDone, 1, NULL);
    }
    return 0;
}

DWORD WINAPI writer(LPVOID)
{
    while (true)
    {
        if (WaitForSingleObject(semDone, 3000) == WAIT_TIMEOUT)
        {
            WaitForSingleObject(mutex, INFINITE);
            bool finished = (de_count == 0 && done_reading);
            ReleaseMutex(mutex);
            if (finished) break;
            continue;
        }
        WaitForSingleObject(mutex, INFINITE);
        char ch = bufer[writer_out];
        writer_out = (writer_out + 1) % M;
        de_count--;
        ReleaseMutex(mutex);
        fputc(ch, fout);
        ReleaseSemaphore(semEmpty, 1, NULL);
    }
    cout << "Запись в файл завершена\n";
    return 0;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    cout << "Задание 3 - Дешифрование текста\n\n";
    memset(bufer, 0, sizeof(bufer));
    if (fopen_s(&fin, "output.txt", "r") || !fin)
    {
        cout << "Нет файла output.txt\n";
        system("pause");
        return 1;
    }
    if (fopen_s(&fout, "decrypted.txt", "w") || !fout)
    {
        cout << "Не могу создать decrypted.txt\n";
        fclose(fin);
        system("pause");
        return 1;
    }
    semEmpty = CreateSemaphore(NULL, M, M, NULL);
    semFull = CreateSemaphore(NULL, 0, M, NULL);
    semDone = CreateSemaphore(NULL, 0, M, NULL);
    mutex = CreateMutex(NULL, FALSE, NULL);
    HANDLE h[6];
    h[0] = CreateThread(NULL, 0, reader, NULL, 0, NULL);
    for (int i = 1; i <= N; i++)
    {
        h[i] = CreateThread(NULL, 0, worker, NULL, 0, NULL);
    }
    h[5] = CreateThread(NULL, 0, writer, NULL, 0, NULL);
    WaitForMultipleObjects(6, h, TRUE, INFINITE);
    for (int i = 0; i < 6; i++) CloseHandle(h[i]);
    CloseHandle(semEmpty);
    CloseHandle(semFull);
    CloseHandle(semDone);
    CloseHandle(mutex);
    fclose(fin);
    fclose(fout);
    return 0;
}

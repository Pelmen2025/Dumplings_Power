#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <locale.h>

using namespace std;

HANDLE hMutexBufferGotov;
HANDLE hMutexBufferObrabotan;
HANDLE hMutexDostupKBufferu;

const int m = 2;
char buffer[3];  // 10 байт данных + место под '\0'
int summa = 0;
bool konecChitaniya = false;

DWORD WINAPI chitayFayl(LPVOID param)
{
    ifstream fayl("input.txt", ios::binary);
    if (!fayl)
    {
        cout << "Ошибка открытия файла\n";
        return 0;
    }

    while (true)
    {
        WaitForSingleObject(hMutexBufferObrabotan, INFINITE);
        WaitForSingleObject(hMutexDostupKBufferu, INFINITE);
        fayl.read(buffer, m);
        int prochitano = fayl.gcount();
        if (prochitano == 0)
        {
            konecChitaniya = true;
            // Освобождаем мьютексы, чтобы второй поток смог завершиться
            ReleaseMutex(hMutexDostupKBufferu);
            ReleaseMutex(hMutexBufferGotov);
            break;
        }
        buffer[prochitano] = '\0';
        // Отпускаем доступ к буферу
        ReleaseMutex(hMutexDostupKBufferu);
        // Сообщаем, что данные в буфере готовы к обработке
        ReleaseMutex(hMutexBufferGotov);
    }
    fayl.close();
    return 0;
}

DWORD WINAPI schitaySummu(LPVOID param)
{
    while (true)
    {
        // Ждём, пока в буфере появятся новые данные
        WaitForSingleObject(hMutexBufferGotov, INFINITE);
        // Берём доступ к буферу
        WaitForSingleObject(hMutexDostupKBufferu, INFINITE);
        if (konecChitaniya)
        {
            ReleaseMutex(hMutexDostupKBufferu);
            break;
        }
        for (int i = 0; buffer[i] != '\0'; i++)
        {
            summa = (summa + (unsigned char)buffer[i]) % 256;
        }
        // Отпускаем буфер
        ReleaseMutex(hMutexDostupKBufferu);
        // Сообщаем читателю, что можно записывать следующую порцию
        ReleaseMutex(hMutexBufferObrabotan);
    }
    cout << "Контрольная сумма: " << summa << endl;
    return 0;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    hMutexBufferGotov = CreateMutex(NULL, FALSE, NULL);
    hMutexBufferObrabotan = CreateMutex(NULL, FALSE, NULL);
    hMutexDostupKBufferu = CreateMutex(NULL, FALSE, NULL);
    if (hMutexBufferGotov == NULL || hMutexBufferObrabotan == NULL || hMutexDostupKBufferu == NULL)
    {
        cout << "Ошибка создания мьютекса\n";
        return 1;
    }
    HANDLE hChitay = CreateThread(NULL, 0, chitayFayl, NULL, 0, NULL);
    HANDLE hSchitay = CreateThread(NULL, 0, schitaySummu, NULL, 0, NULL);
    if (hChitay == NULL || hSchitay == NULL)
    {
        cout << "Ошибка создания потоков\n";
        return 1;
    }
    HANDLE potoki[2] = { hChitay, hSchitay };
    WaitForMultipleObjects(2, potoki, TRUE, INFINITE);
    CloseHandle(hChitay);
    CloseHandle(hSchitay);
    CloseHandle(hMutexBufferGotov);
    CloseHandle(hMutexBufferObrabotan);
    CloseHandle(hMutexDostupKBufferu);
    return 0;
}

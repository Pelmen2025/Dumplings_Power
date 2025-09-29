#include <windows.h>
#include <iostream>
#include <string>
#include <locale.h>
using namespace std;

// Глобальные переменные
unsigned char* encrypted_text;  // зашифрованный текст (массив байт)
unsigned char* key;             // ключ шифрования (массив байт)
unsigned char** blocks;         // массив блоков зашифрованного текста
unsigned char** decrypted;      // массив расшифрованных блоков
int* block_numbers;             // номера блоков
int k;                          // длина ключа в байтах
int n;                          // длина текста в байтах
int num_blocks;                 // количество блоков

// Структура для передачи параметров в поток
struct ThreadParam {
    int block_num;
    int block_len;
};

// Функция дешифрования одного блока
DWORD WINAPI decrypt_block(LPVOID param)
{
    // Получаем номер блока и его длину
    ThreadParam* tp = (ThreadParam*)param;
    int block_num = tp->block_num;
    int block_len = tp->block_len;

    unsigned char* block = blocks[block_num];
    unsigned char* result = decrypted[block_num];
    unsigned char* temp = new unsigned char[block_len];

    // Шаг 1: Применяем XOR с ключом (обратная операция)
    for (int i = 0; i < block_len; i++)
    {
        temp[i] = block[i] ^ key[i];
    }

    // Шаг 2: Переворачиваем блок обратно (первый->последний, последний->первый)
    for (int i = 0; i < block_len; i++)
    {
        result[i] = temp[block_len - 1 - i];
    }

    delete[] temp;
    return 0;
}

int main()
{
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    // Ввод данных
    string encrypted_str, key_str;

    cout << "Введите зашифрованный текст: ";
    getline(cin, encrypted_str);

    cout << "Введите ключ: ";
    getline(cin, key_str);

    n = encrypted_str.length();
    k = key_str.length();

    if (k == 0)
    {
        cout << "Ошибка: ключ не может быть пустым!" << endl;
        return 1;
    }

    // Копируем строки в массивы байт
    encrypted_text = new unsigned char[n + 1];
    key = new unsigned char[k + 1];

    for (int i = 0; i < n; i++)
        encrypted_text[i] = (unsigned char)encrypted_str[i];
    encrypted_text[n] = '\0';

    for (int i = 0; i < k; i++)
        key[i] = (unsigned char)key_str[i];
    key[k] = '\0';

    // Вычисляем количество блоков
    num_blocks = (n + k - 1) / k;  // округление вверх

    // Выделяем память для массивов
    blocks = new unsigned char* [num_blocks];
    decrypted = new unsigned char* [num_blocks];
    block_numbers = new int[num_blocks];

    // Разделяем зашифрованный текст на блоки
    for (int i = 0; i < num_blocks; i++)
    {
        int start = i * k;
        int length = min(k, n - start);

        blocks[i] = new unsigned char[length + 1];
        decrypted[i] = new unsigned char[length + 1];

        for (int j = 0; j < length; j++)
        {
            blocks[i][j] = encrypted_text[start + j];
        }
        blocks[i][length] = '\0';
        decrypted[i][length] = '\0';

        block_numbers[i] = i;
    }

    cout << "--- Зашифрованные блоки ---" << endl;
    for (int i = 0; i < num_blocks; i++)
    {
        cout << "Блок " << i << ": \"" << blocks[i] << "\"" << endl;
    }

    // Создание потоков
    HANDLE* hThread = new HANDLE[num_blocks];
    DWORD* dwThreadID = new DWORD[num_blocks];
    ThreadParam* params = new ThreadParam[num_blocks];

    // Запуск потоков для дешифрования каждого блока
    for (int i = 0; i < num_blocks; i++)
    {
        params[i].block_num = i;
        int start = i * k;
        params[i].block_len = min(k, n - start);

        hThread[i] = CreateThread(
            NULL,                  // атрибуты безопасности по умолчанию
            0,                     // размер стека по умолчанию
            decrypt_block,         // функция потока
            &params[i],            // параметр (номер блока)
            0,                     // флаги создания
            &dwThreadID[i]         // идентификатор потока
        );

        // Проверка создания потока
        if (hThread[i] == NULL)
        {
            cout << "Поток № " << i << " не был создан" << endl;
            cout << "Ошибка: " << GetLastError() << endl;
            return 1;
        }
    }

    // Ожидание завершения всех потоков
    WaitForMultipleObjects(num_blocks, hThread, TRUE, INFINITE);

    // Вывод результатов
    cout << "--- Расшифрованные блоки ---" << endl;
    for (int i = 0; i < num_blocks; i++)
    {
        cout << "Блок " << i << ": \"" << decrypted[i] << "\"" << endl;
    }

    cout << "--- Результат ---" << endl;
    cout << "Зашифрованный текст: \"" << encrypted_text << "\"" << endl;
    cout << "Ключ:                \"" << key << "\"" << endl;
    cout << "Расшифрованный текст: \"";
    for (int i = 0; i < num_blocks; i++)
    {
        cout << decrypted[i];
    }
    cout << "\"" << endl;

    // Вывод в шестнадцатеричном виде
    cout << "Расшифрованный текст (hex): ";
    for (int i = 0; i < num_blocks; i++)
    {
        int start = i * k;
        int length = min(k, n - start);
        for (int j = 0; j < length; j++)
        {
            printf("%02X ", decrypted[i][j]);
        }
    }
    cout << endl;

    // Закрытие дескрипторов потоков
    for (int i = 0; i < num_blocks; i++)
    {
        CloseHandle(hThread[i]);
    }

    // Освобождение памяти
    for (int i = 0; i < num_blocks; i++)
    {
        delete[] blocks[i];
        delete[] decrypted[i];
    }
    delete[] blocks;
    delete[] decrypted;
    delete[] block_numbers;
    delete[] encrypted_text;
    delete[] key;
    delete[] hThread;
    delete[] dwThreadID;
    delete[] params;

    return 0;
}
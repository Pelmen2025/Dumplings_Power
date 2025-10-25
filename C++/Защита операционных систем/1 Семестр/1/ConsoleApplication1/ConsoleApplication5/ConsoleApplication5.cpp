#include <windows.h>
#include <iostream>
#include <string>
#include <locale.h>
using namespace std;

// Глобальные переменные
unsigned char* text;      // исходный текст (массив байт)
unsigned char* key;       // ключ шифрования (массив байт)
unsigned char** blocks;   // массив блоков текста
unsigned char** encrypted;// массив зашифрованных блоков
int* block_numbers;       // номера блоков
int k;                    // длина ключа в байтах
int n;                    // длина текста в байтах
int num_blocks;           // количество блоков

// Структура для передачи параметров в поток
struct ThreadParam {
    int block_num;
    int block_len;
};

// Функция шифрования одного блока
DWORD WINAPI encrypt_block(LPVOID param)
{
    // Приводим параметр к типу ThreadParam
    ThreadParam* tp = (ThreadParam*)param;
    int block_num = tp->block_num; // Получаем номер блока
    int block_len = tp->block_len; // Получаем длину блока

    // Получаем указатели на текущий блок и массив для результата
    unsigned char* block = blocks[block_num];   // Исходный блок текста
    unsigned char* result = encrypted[block_num]; // Зашифрованный блок

    // Шаг 1: Переворачиваем блок (первый->последний, второй->предпоследний и т.д.)
    for (int i = 0; i < block_len; i++)
    {
        result[i] = block[block_len - 1 - i]; // Копируем символы в обратном порядке
    }

    // Шаг 2: Применяем XOR с ключом
    for (int i = 0; i < block_len; i++)
    {
        result[i] = result[i] ^ key[i]; // Побитовое XOR с соответствующим символом ключа
    }

    return 0; // Возвращаем 0 для указания успешного завершения потока
}

int main()
{
    setlocale(LC_ALL, "Russian");
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    // Ввод данных
    string text_str, key_str;

    cout << "Введите текст для шифрования: ";
    getline(cin, text_str);

    cout << "Введите ключ: ";
    getline(cin, key_str);

    n = text_str.length();
    k = key_str.length();

    if (k == 0)
    {
        cout << "Ошибка: ключ не может быть пустым!" << endl;
        return 1;
    }

    // Копируем строки в массивы байт
    text = new unsigned char[n + 1];
    key = new unsigned char[k + 1];

    for (int i = 0; i < n; i++)
        text[i] = (unsigned char)text_str[i];
    text[n] = '\0';

    for (int i = 0; i < k; i++)
        key[i] = (unsigned char)key_str[i];
    key[k] = '\0';

    // Вычисляем количество блоков
    num_blocks = (n + k - 1) / k;  // округление вверх

    // Выделяем память для массивов
    blocks = new unsigned char* [num_blocks];
    encrypted = new unsigned char* [num_blocks];
    block_numbers = new int[num_blocks];

    // Разделяем текст на блоки
    for (int i = 0; i < num_blocks; i++)
    {
        int start = i * k; // Начальная позиция текущего блока в массиве text
        int length = min(k, n - start); // Длина текущего блока (не больше k или оставшегося текста)

        blocks[i] = new unsigned char[length + 1]; // Выделяем память для блока (+1 для '\0')
        encrypted[i] = new unsigned char[length + 1]; // Выделяем память для зашифрованного блока

        // Копируем символы из text в текущий блок
        for (int j = 0; j < length; j++)
        {
            blocks[i][j] = text[start + j]; // Копируем символы из text в блок
        }
        blocks[i][length] = '\0'; // Добавляем завершающий нуль для блока
        encrypted[i][length] = '\0'; // Добавляем завершающий нуль для зашифрованного блока
        block_numbers[i] = i; // Сохраняем номер блока (индекс)
    }

    cout << "--- Исходные блоки ---" << endl;
    for (int i = 0; i < num_blocks; i++)
    {
        cout << "Блок " << i << ": \"" << blocks[i] << "\"" << endl;
    }

    // Создание потоков
    HANDLE* hThread = new HANDLE[num_blocks];
    DWORD* dwThreadID = new DWORD[num_blocks];
    ThreadParam* params = new ThreadParam[num_blocks];

    // Запуск потоков для шифрования каждого блока
    for (int i = 0; i < num_blocks; i++)
    {
        params[i].block_num = i;
        int start = i * k;
        params[i].block_len = min(k, n - start);

        hThread[i] = CreateThread(
            NULL,                  // атрибуты безопасности по умолчанию
            0,                     // размер стека по умолчанию
            encrypt_block,         // функция потока
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
    cout << "--- Зашифрованные блоки ---" << endl;
    for (int i = 0; i < num_blocks; i++)
    {
        cout << "Блок " << i << ": \"" << encrypted[i] << "\"" << endl;
    }

    cout << "--- Результат ---" << endl;
    cout << "Исходный текст:      \"" << text << "\"" << endl;
    cout << "Ключ:                \"" << key << "\"" << endl;
    cout << "Зашифрованный текст: \"";
    for (int i = 0; i < num_blocks; i++)
    {
        cout << encrypted[i];
    }
    cout << "\"" << endl;

    // Закрытие дескрипторов потоков
    for (int i = 0; i < num_blocks; i++)
    {
        CloseHandle(hThread[i]);
    }

    // Освобождение памяти
    for (int i = 0; i < num_blocks; i++)
    {
        delete[] blocks[i];
        delete[] encrypted[i];
    }
    delete[] blocks;
    delete[] encrypted;
    delete[] block_numbers;
    delete[] text;
    delete[] key;
    delete[] hThread;
    delete[] dwThreadID;
    delete[] params;

    return 0;
}
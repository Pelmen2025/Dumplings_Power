#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include <cstdint>

using namespace std;

// Вычисление CRC16 с заданным полиномом
uint16_t computeCRC16(const string& data, uint16_t poly)
{
    uint16_t crc = 0xFFFF;  // начальное значение

    for (char byte : data)
    {
        crc ^= (uint8_t)byte << 8;  // загружаем байт в старший разряд

        for (int bit = 0; bit < 8; ++bit)
        {
            if (crc & 0x8000)       // если старший бит = 1
                crc = (crc << 1) ^ poly;
            else
                crc <<= 1;
        }
    }
    return crc;
}

int main()
{
    setlocale(LC_ALL, "Russian"); // Поддержка русского языка в консоли Windows

    string poly_str, message;

    cout << "=== Вычисление CRC16 ===\n\n";

    // Ввод полинома
    cout << "Введите полином (hex, например 8005 или 1021): ";
    cin >> poly_str;

    // Парсим полином из hex-строки
    uint16_t poly;
    stringstream ss;
    ss << hex << poly_str;
    ss >> poly;

    // Ввод сообщения
    cout << "Введите сообщение: ";
    cin.ignore();
    getline(cin, message);

    // Вычисляем CRC16
    uint16_t crc = computeCRC16(message, poly);

    // Вывод результата
    cout << "\n--- Результат ---\n";
    cout << "Полином    : 0x" << uppercase << hex << setw(4) << setfill('0') << poly << "\n";
    cout << "Сообщение  : \"" << message << "\"\n";
    cout << "CRC16      : 0x" << uppercase << hex << setw(4) << setfill('0') << crc << "\n";

    return 0;
}
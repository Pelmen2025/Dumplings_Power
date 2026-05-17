#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>

using namespace std;

// Вспомогательная функция для удобного вывода вектора
void printVector(const vector<int>& vec) {
    for (int bit : vec) cout << bit << " ";
    cout << endl;
}

// Класс кодера/декодера Хэмминга (7,4)
class HammingCodec {
public:
    vector<int> encode(const vector<int>& data) {
        if (data.size() != 4) throw invalid_argument("Требуется 4 бита данных.");

        vector<int> c(7);
        cout << "  [КОДЕР] Шаг 1. Распределение информационных битов (d1-d4) по позициям 3, 5, 6, 7:" << endl;
        c[2] = data[0]; cout << "    Позиция 3 (индекс 2) = d1 = " << c[2] << endl;
        c[4] = data[1]; cout << "    Позиция 5 (индекс 4) = d2 = " << c[4] << endl;
        c[5] = data[2]; cout << "    Позиция 6 (индекс 5) = d3 = " << c[5] << endl;
        c[6] = data[3]; cout << "    Позиция 7 (индекс 6) = d4 = " << c[6] << endl;

        cout << "  [КОДЕР] Шаг 2. Вычисление проверочных битов (p1, p2, p3) через XOR:" << endl;
        // p1 контролирует позиции 1, 3, 5, 7
        c[0] = c[2] ^ c[4] ^ c[6];
        cout << "    p1 (позиция 1) = d1 ^ d2 ^ d4 = " << c[2] << " ^ " << c[4] << " ^ " << c[6] << " = " << c[0] << endl;

        // p2 контролирует позиции 2, 3, 6, 7
        c[1] = c[2] ^ c[5] ^ c[6];
        cout << "    p2 (позиция 2) = d1 ^ d3 ^ d4 = " << c[2] << " ^ " << c[5] << " ^ " << c[6] << " = " << c[1] << endl;

        // p3 контролирует позиции 4, 5, 6, 7
        c[3] = c[4] ^ c[5] ^ c[6];
        cout << "    p3 (позиция 4) = d2 ^ d3 ^ d4 = " << c[4] << " ^ " << c[5] << " ^ " << c[6] << " = " << c[3] << endl;

        return c;
    }

    vector<int> decode(vector<int> r) {
        if (r.size() != 7) throw invalid_argument("Требуется 7 бит.");

        cout << "  [ДЕКОДЕР] Шаг 1. Вычисление синдрома ошибки (s1, s2, s3):" << endl;
        int s1 = r[0] ^ r[2] ^ r[4] ^ r[6];
        cout << "    s1 = p1 ^ d1 ^ d2 ^ d4 = " << r[0] << "^" << r[2] << "^" << r[4] << "^" << r[6] << " = " << s1 << endl;

        int s2 = r[1] ^ r[2] ^ r[5] ^ r[6];
        cout << "    s2 = p2 ^ d1 ^ d3 ^ d4 = " << r[1] << "^" << r[2] << "^" << r[5] << "^" << r[6] << " = " << s2 << endl;

        int s3 = r[3] ^ r[4] ^ r[5] ^ r[6];
        cout << "    s3 = p3 ^ d2 ^ d3 ^ d4 = " << r[3] << "^" << r[4] << "^" << r[5] << "^" << r[6] << " = " << s3 << endl;

        // Перевод синдрома из двоичной системы в десятичную
        int error_pos = s1 * 1 + s2 * 2 + s3 * 4;
        cout << "  [ДЕКОДЕР] Шаг 2. Анализ синдрома:" << endl;
        cout << "    Двоичный синдром (s3, s2, s1): " << s3 << s2 << s1 << endl;
        cout << "    Десятичное значение синдрома: " << error_pos << endl;

        if (error_pos != 0) {
            cout << "    -> Обнаружена ошибка на позиции " << error_pos << "!" << endl;
            cout << "  [ДЕКОДЕР] Шаг 3. Исправление ошибки:" << endl;
            cout << "    Значение бита на позиции " << error_pos << " было: " << r[error_pos - 1] << endl;
            r[error_pos - 1] ^= 1; // Исправляем (инвертируем)
            cout << "    Значение бита после исправления: " << r[error_pos - 1] << endl;
        }
        else {
            cout << "    -> Синдром равен 0. Ошибок в канале не возникло." << endl;
        }

        cout << "  [ДЕКОДЕР] Шаг 4. Извлечение информационных битов (с позиций 3, 5, 6, 7)." << endl;
        return { r[2], r[4], r[5], r[6] };
    }
};

// Модель дискретного канала связи
class DiscreteChannel {
public:
    vector<int> transmitWithSingleError(vector<int> input, int error_pos) {
        vector<int> output = input;
        int index = error_pos - 1; // Переход от номера позиции к индексу массива
        if (index >= 0 && index < output.size()) {
            output[index] ^= 1;
        }
        return output;
    }
};

int main() {
    setlocale(LC_ALL, "Russian"); // Поддержка русского языка в консоли Windows

    HammingCodec codec;
    DiscreteChannel channel;

    vector<int> original_data = { 1, 0, 1, 1 }; // Исходные данные: 1011

    cout << "=======================================================" << endl;
    cout << "ЭТАП 1: КОДИРОВАНИЕ (Передатчик)" << endl;
    cout << "=======================================================" << endl;
    cout << "Исходные данные для передачи: "; printVector(original_data);
    cout << endl;
    vector<int> encoded_data = codec.encode(original_data);
    cout << "\nИТОГОВОЕ ЗАКОДИРОВАННОЕ СООБЩЕНИЕ:" << endl;
    cout << "Позиции: 1 2 3 4 5 6 7" << endl;
    cout << "Биты:    "; printVector(encoded_data);
    cout << "=======================================================\n" << endl;

    cout << "=======================================================" << endl;
    cout << "ЭТАП 2: ПЕРЕДАЧА ЧЕРЕЗ КАНАЛ СВЯЗИ" << endl;
    cout << "=======================================================" << endl;
    int error_position = 5; // Вносим ошибку в 5-ю позицию (это первый бит данных d2)
    cout << "Вносим искажение (ошибку) в бит на позиции " << error_position << "..." << endl;
    vector<int> received_data = channel.transmitWithSingleError(encoded_data, error_position);

    cout << "Отправлено: "; printVector(encoded_data);
    cout << "Принято:    "; printVector(received_data);
    cout << "=======================================================\n" << endl;

    cout << "=======================================================" << endl;
    cout << "ЭТАП 3: ДЕКОДИРОВАНИЕ И ИСПРАВЛЕНИЕ (Приемник)" << endl;
    cout << "=======================================================" << endl;
    vector<int> decoded_data = codec.decode(received_data);
    cout << "\nВОССТАНОВЛЕННЫЕ ДАННЫЕ: "; printVector(decoded_data);
    cout << "=======================================================\n" << endl;

    // Сравнение
    cout << "РЕЗУЛЬТАТ: ";
    if (original_data == decoded_data) {
        cout << "УСПЕХ! Код Хэмминга успешно нашел и исправил ошибку. Данные спасены." << endl;
    }
    else {
        cout << "ПРОВАЛ. Данные искажены." << endl;
    }

    return 0;
}
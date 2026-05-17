/*
 * Имитационная модель ДСКБП
 * Дискретный Симметричный Канал с Независимыми Ошибками
 *
 * Задание 1: Генерация последовательности ошибок с вероятностью E (ДСЧ 0-1)
 * Задание 2: Статистический анализ адекватности модели при E = 0.01 и 0.001
 *
 * Компиляция: g++ -O2 -std=c++17 -o dskbp dskbp_simulation.cpp
 */

#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <iomanip>
#include <string>
#include <algorithm>

using namespace std;

// ============================================================
// Генератор ошибок (ДСЧ 0-1)
// ============================================================

/**
 * Генерирует последовательность ошибок в канале ДСКБП.
 * Каждый символ независимо искажается с вероятностью E.
 * Используется ДСЧ равномерного распределения на [0, 1).
 *
 * @param N    длина последовательности
 * @param E    вероятность ошибки
 * @param rng  генератор случайных чисел (единый для всей программы)
 * @return     двоичный вектор (1 = ошибка, 0 = нет)
 */
vector<int> generateErrorSequence(long long N, double E, mt19937& rng)
{
    uniform_real_distribution<double> dist(0.0, 1.0);  // ДСЧ [0, 1)

    vector<int> errors(N);
    for (long long i = 0; i < N; ++i)
    {
        double u = dist(rng);           // случайное число из [0, 1)
        errors[i] = (u < E) ? 1 : 0;   // ошибка если u < E
    }
    return errors;
}

// ============================================================
// Вспомогательные функции вывода
// ============================================================

void separator(char c = '=', int w = 65)
{
    cout << string(w, c) << "\n";
}

void printSequencePreview(const vector<int>& errors, int previewLen = 60)
{
    int n = min((long long)previewLen, (long long)errors.size());
    cout << "  Первые " << previewLen << " символов: ";
    for (int i = 0; i < n; ++i)
        cout << errors[i];
    if ((long long)errors.size() > previewLen)
        cout << "...";
    cout << "\n";
}

// ============================================================
// Статистический анализ
// ============================================================

struct Stats {
    long long N;
    long long n_errors;
    double E_theoretical;
    double E_empirical;
    double sigma_theoretical;
    double abs_error;
    double rel_error_pct;
    double ci_lower;
    double ci_upper;
    bool in_ci;
    double chi2;
    double chi2_critical;
    bool chi2_passed;
};

/**
 * Вычисляет статистики для оценки адекватности модели.
 */
Stats computeStatistics(const vector<int>& errors, double E)
{
    Stats s;
    s.N = errors.size();
    s.n_errors = 0;
    for (int e : errors) s.n_errors += e;

    s.E_theoretical = E;
    s.E_empirical = (double)s.n_errors / s.N;
    s.sigma_theoretical = sqrt(E * (1.0 - E) / s.N);
    s.abs_error = abs(s.E_empirical - E);
    s.rel_error_pct = (E > 0) ? (s.abs_error / E * 100.0) : 0.0;

    // 95% доверительный интервал (z = 1.96)
    double margin = 1.96 * s.sigma_theoretical;
    s.ci_lower = max(0.0, s.E_empirical - margin);
    s.ci_upper = min(1.0, s.E_empirical + margin);
    s.in_ci = (s.ci_lower <= E && E <= s.ci_upper);

    // Критерий хи-квадрат (2 категории: ошибка / не ошибка)
    double exp_err = s.N * E;
    double exp_ok = s.N * (1.0 - E);
    double n_ok = s.N - s.n_errors;
    s.chi2_critical = 3.841;  // df=1, alpha=0.05
    s.chi2 = pow(s.n_errors - exp_err, 2) / exp_err
        + pow(n_ok - exp_ok, 2) / exp_ok;
    s.chi2_passed = (s.chi2 <= s.chi2_critical);

    return s;
}

/**
 * Минимальное N для достижения заданной относительной погрешности.
 * Формула: N >= z^2 * (1 - E) / (E * delta_rel^2)
 */
long long determineNmin(double E, double delta_rel = 0.05, double z = 1.96)
{
    return (long long)ceil(z * z * (1.0 - E) / (E * delta_rel * delta_rel));
}

void printStats(const Stats& s)
{
    auto yn = [](bool b) { return b ? "[DA]" : "[NET]"; };

    cout << fixed;
    cout << "  Число символов N         : " << s.N << "\n";
    cout << "  Число ошибок             : " << s.n_errors << "\n";
    cout << "  Теор. вероятность E      : " << setprecision(4) << s.E_theoretical << "\n";
    cout << "  Эмпир. вероятность E^    : " << setprecision(6) << s.E_empirical << "\n";
    cout << "  Абс. погрешность |E^- E| : " << setprecision(6) << s.abs_error << "\n";
    cout << "  Отн. погрешность         : " << setprecision(2) << s.rel_error_pct << "%\n";
    cout << "  sigma (теор.)            : " << setprecision(6) << s.sigma_theoretical << "\n";
    cout << "  95% ДИ                   : ["
        << setprecision(6) << s.ci_lower << ", "
        << setprecision(6) << s.ci_upper << "]\n";
    cout << "  E в ДИ?                  : " << yn(s.in_ci) << "\n";
    cout << "  chi2 наблюд. / крит.     : "
        << setprecision(4) << s.chi2 << " / " << s.chi2_critical << "\n";
    cout << "  Нулевая гипотеза chi2    : "
        << (s.chi2_passed ? "[OK] принята" : "[!!] отвергнута") << " (alpha=0.05)\n";
}

// ============================================================
// ГЛАВНАЯ ПРОГРАММА
// ============================================================

int main()
{
    setlocale(LC_ALL, "Russian");

    const unsigned int SEED = 42;
    const double       DELTA_REL = 0.05;  // желаемая погрешность 5%

    // Единственный генератор на всю программу
    mt19937 rng(SEED);

    separator('=');
    cout << "  ИМИТАЦИОННАЯ МОДЕЛЬ ДСКБП\n";
    cout << "  Дискретный Симметричный Канал с Независимыми Ошибками\n";
    separator('=');

    bool task1_shown = false;

    for (double E : {0.01, 0.001})
    {
        long long N_min = determineNmin(E, DELTA_REL);

        cout << "\n";
        separator('-');
        cout << "  Вероятность ошибки E = " << E << "\n";
        separator('-');
        cout << "\n  Мин. N для d_отн <= "
            << (int)(DELTA_REL * 100) << "%: N_min = " << N_min << "\n\n";

        for (long long N : {N_min / 10, N_min / 2, N_min, N_min * 5})
        {
            if (N < 10) N = 10;

            // generateErrorSequence вызывается ОДИН РАЗ на каждый блок (E, N).
            // Та же последовательность используется и для Задания 1 (превью),
            // и для Задания 2 (статистика) — без повторной генерации.
            auto errors = generateErrorSequence(N, E, rng);

            // Задание 1: показываем превью только для первого блока
            if (!task1_shown)
            {
                separator('=');
                cout << "[ ЗАДАНИЕ 1 ] Генерация последовательности ошибок (ДСЧ 0-1)\n\n";
                cout << "  E = " << E << ", N = " << N << "\n";
                printSequencePreview(errors);
                long long cnt = 0;
                for (int e : errors) cnt += e;
                cout << "  Ошибок в выборке: " << cnt
                    << " (ожидалось ~" << (long long)(N * E) << ")\n\n";
                separator('=');
                cout << "[ ЗАДАНИЕ 2 ] Статистический анализ адекватности модели\n";
                separator('=');
                cout << "\n";
                separator('-');
                cout << "  Вероятность ошибки E = " << E << "\n";
                separator('-');
                cout << "\n  Мин. N для d_отн <= "
                    << (int)(DELTA_REL * 100) << "%: N_min = " << N_min << "\n\n";
                task1_shown = true;
            }

            // Задание 2: статистика по той же последовательности errors
            Stats s = computeStatistics(errors, E);
            cout << "  -- N = " << N << " --\n";
            printStats(s);
            cout << "\n";
        }
    }

    separator('=');
    cout << "  ВЫВОД:\n";
    cout << "  * Погрешность оценки убывает с ростом N (~1/sqrt(N)).\n";
    cout << "  * При N >= N_min достигается заданная точность 5%.\n";
    cout << "  * chi2-критерий подтверждает биномиальный закон ошибок.\n";
    cout << "  * Истинное E попадает в 95% ДИ при достаточном N.\n";
    separator('=');

    return 0;
}
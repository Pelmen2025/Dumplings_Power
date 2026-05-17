/*
 * Имитационная модель ДСКБП
 * Дискретный Симметричный Канал с Независимыми Ошибками
 *
 * Задание 1: Генерация последовательности ошибок (ДСЧ 0-1)
 * Задание 2: Статистический анализ адекватности модели
 * Задание 3: Биномиальное распределение числа ошибок P(i, N, E)
 * Задание 4: Экспериментальное распределение и сравнение с теорией
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

vector<int> generateErrorSequence(long long N, double E, mt19937& rng)
{
    uniform_real_distribution<double> dist(0.0, 1.0);
    vector<int> errors(N);
    for (long long i = 0; i < N; ++i)
    {
        double u = dist(rng);
        errors[i] = (u < E) ? 1 : 0;
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
// Задание 2: Статистический анализ
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

    double margin = 1.96 * s.sigma_theoretical;
    s.ci_lower = max(0.0, s.E_empirical - margin);
    s.ci_upper = min(1.0, s.E_empirical + margin);
    s.in_ci = (s.ci_lower <= E && E <= s.ci_upper);

    double exp_err = s.N * E;
    double exp_ok = s.N * (1.0 - E);
    double n_ok = s.N - s.n_errors;
    s.chi2_critical = 3.841;
    s.chi2 = pow(s.n_errors - exp_err, 2) / exp_err
        + pow(n_ok - exp_ok, 2) / exp_ok;
    s.chi2_passed = (s.chi2 <= s.chi2_critical);

    return s;
}

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
// Задание 3: Биномиальное распределение P(i, N, E)
// ============================================================

// Логарифм факториала для больших чисел (формула Стирлинга не нужна —
// считаем точно через сумму логарифмов)
double logFactorial(int n)
{
    double result = 0.0;
    for (int k = 2; k <= n; ++k)
        result += log((double)k);
    return result;
}

// P(i | N, E) = C(N,i) * E^i * (1-E)^(N-i)
// Считаем через логарифмы, чтобы избежать переполнения при больших N
double binomialProbability(int N, int i, double E)
{
    if (i < 0 || i > N) return 0.0;
    double logP = logFactorial(N) - logFactorial(i) - logFactorial(N - i)
        + i * log(E) + (N - i) * log(1.0 - E);
    return exp(logP);
}

// Вывод таблицы биномиального распределения для i = 0..maxI
void printBinomialDistribution(int N, double E, int maxI)
{
    cout << "  Биномиальное P(i | N=" << N << ", E=" << E << "):\n";
    cout << "  " << string(35, '-') << "\n";
    cout << "  " << setw(6) << "i"
        << setw(14) << "P(i) теор."
        << setw(14) << "P(i) накопл." << "\n";
    cout << "  " << string(35, '-') << "\n";

    double cumulative = 0.0;
    for (int i = 0; i <= maxI; ++i)
    {
        double p = binomialProbability(N, i, E);
        cumulative += p;
        cout << fixed
            << "  " << setw(6) << i
            << setw(14) << setprecision(6) << p
            << setw(14) << setprecision(6) << cumulative << "\n";
    }
    cout << "  " << string(35, '-') << "\n\n";
}

// ============================================================
// Задание 4: Экспериментальное распределение
// ============================================================

// Запускаем M экспериментов: в каждом генерируем N символов и считаем
// число ошибок i. Строим гистограмму частот и сравниваем с теорией.
void runExperiment(int N, double E, int M, int maxI, mt19937& rng)
{
    vector<int> freq(maxI + 2, 0);  // частота: freq[i] = сколько раз встретилось i ошибок

    for (int m = 0; m < M; ++m)
    {
        auto errors = generateErrorSequence(N, E, rng);
        int cnt = 0;
        for (int e : errors) cnt += e;
        if (cnt <= maxI)
            freq[cnt]++;
        else
            freq[maxI + 1]++;  // "больше maxI"
    }

    cout << "  Эксперимент: M=" << M << " опытов, N=" << N
        << fixed << setprecision(3) << ", E=" << E << "\n";
    cout << "  " << string(57, '-') << "\n";
    cout << "  " << setw(6) << "i"
        << setw(14) << "P(i) теор."
        << setw(14) << "P(i) эксп."
        << setw(14) << "Откл., %" << "\n";
    cout << "  " << string(57, '-') << "\n";

    for (int i = 0; i <= maxI; ++i)
    {
        double p_theory = binomialProbability(N, i, E);
        double p_exp = (double)freq[i] / M;
        double dev = (p_theory > 1e-12)
            ? abs(p_exp - p_theory) / p_theory * 100.0
            : 0.0;
        cout << fixed
            << "  " << setw(6) << i
            << setw(14) << setprecision(6) << p_theory
            << setw(14) << setprecision(6) << p_exp
            << setw(13) << setprecision(2) << dev << "%\n";
    }
    if (freq[maxI + 1] > 0)
        cout << "  (>" << maxI << "): " << freq[maxI + 1] << " случаев\n";
    cout << "  " << string(57, '-') << "\n\n";
}

// ============================================================
// ГЛАВНАЯ ПРОГРАММА
// ============================================================

int main()
{
    setlocale(LC_ALL, "Russian"); // Поддержка русского языка в консоли Windows

    const unsigned int SEED = 42;
    const double       DELTA_REL = 0.05;

    mt19937 rng(SEED);

    separator('=');
    cout << "  ИМИТАЦИОННАЯ МОДЕЛЬ ДСКБП\n";
    cout << "  Дискретный Симметричный Канал с Независимыми Ошибками\n";
    separator('=');

    // ---- ЗАДАНИЯ 1 и 2 ----
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

            // generateErrorSequence вызывается ОДИН РАЗ на блок.
            // Та же последовательность идёт и в Задание 1, и в Задание 2.
            auto errors = generateErrorSequence(N, E, rng);

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

            Stats s = computeStatistics(errors, E);
            cout << "  -- N = " << N << " --\n";
            printStats(s);
            cout << "\n";
        }
    }

    // ---- ЗАДАНИЕ 3: Биномиальное распределение ----
    separator('=');
    cout << "[ ЗАДАНИЕ 3 ] Биномиальное распределение числа ошибок P(i,N,E)\n";
    separator('=');
    cout << "\n";

    // N=100, показываем i = 0..8 для E=0.01 и E=0.001
    printBinomialDistribution(100, 0.01, 8);
    printBinomialDistribution(100, 0.001, 5);

    // N=1000
    printBinomialDistribution(1000, 0.01, 20);
    printBinomialDistribution(1000, 0.001, 8);

    // ---- ЗАДАНИЕ 4: Экспериментальное распределение ----
    separator('=');
    cout << "[ ЗАДАНИЕ 4 ] Экспериментальное распределение vs теория\n";
    separator('=');
    cout << "\n";

    // M=10000 опытов, N=100, E=0.01
    runExperiment(100, 0.01, 10000, 8, rng);
    // M=10000 опытов, N=100, E=0.001
    runExperiment(100, 0.001, 10000, 5, rng);
    // M=10000 опытов, N=1000, E=0.01
    runExperiment(1000, 0.01, 10000, 20, rng);
    // M=10000 опытов, N=1000, E=0.001
    runExperiment(1000, 0.001, 10000, 8, rng);

    separator('=');
    cout << "  ВЫВОД:\n";
    cout << "  * Задание 1: ДСЧ 0-1 корректно моделирует независимые ошибки.\n";
    cout << "  * Задание 2: погрешность убывает как 1/sqrt(N); при N>=N_min < 5%.\n";
    cout << "  * Задание 3: теоретическое P(i) описывается биномиальным законом.\n";
    cout << "  * Задание 4: экспериментальное распределение сходится к теории\n";
    cout << "               при увеличении числа опытов M.\n";
    separator('=');

    return 0;
}
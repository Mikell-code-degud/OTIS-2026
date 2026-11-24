/**
 * @file StabilityAnalyzer.h
 * @brief Модуль анализа устойчивости линейных дискретных моделей (из ЛР №2).
 *
 * Для линейной модели 1.6 перед запуском симуляции строится характеристическое
 * уравнение z^3 - a1*z^2 - a2*z - a3 = 0, находятся его корни на Z-плоскости
 * и по критерию Шура выносится вердикт об устойчивости.
 */
#pragma once

#include <complex>
#include <iosfwd>
#include <string>
#include <vector>

/// Вердикт об устойчивости системы.
enum class StabilityVerdict
{
    Stable,     ///< все корни |z| < 1
    Marginal,   ///< есть корень |z| = 1 (граница устойчивости)
    Unstable    ///< есть корень |z| > 1 (расходящийся процесс)
};

/// Полный отчёт об устойчивости модели.
struct StabilityReport
{
    std::string modelName;                       ///< имя модели
    std::vector<double> coefficients;            ///< коэффициенты полинома (по возрастанию степени)
    std::vector<std::complex<double>> roots;     ///< корни характеристического уравнения
    double spectralRadius = 0.0;                 ///< max |z_i|
    StabilityVerdict verdict = StabilityVerdict::Stable;  ///< вердикт
};

/// Анализ устойчивости дискретных линейных систем по критерию Шура.
class StabilityAnalyzer
{
public:
    /// Находит корни полинома с коэффициентами по возрастанию степени.
    static std::vector<std::complex<double>> findRoots(const std::vector<double>& coefficients);
    /// Вердикт по критерию Шура (без явного поиска корней).
    static StabilityVerdict schurVerdict(const std::vector<double>& coefficients);
    /// true, если система устойчива.
    static bool isStable(const std::vector<double>& coefficients);
    /// true, если для отчёта нужно вывести предупреждение.
    static bool needsWarning(const StabilityReport& report);

    /// Спектральный радиус (max |z_i|).
    static double spectralRadius(const std::vector<std::complex<double>>& roots);

    /// Model 1.6: z^3 - a1*z^2 - a2*z - a3 = 0.
    static StabilityReport analyzeModel1_6(double a1, double a2, double a3);
    /// Возвращает коэффициенты характеристического уравнения модели 1.6.
    static std::vector<double> characteristicPolynomial1_6(double a1, double a2, double a3);

    /// true, если система на границе устойчивости (есть корень |z| = 1).
    static bool isMarginallyStable(const std::vector<double>& coefficients);

    /// Текстовое описание отчёта (модель, уравнение, корни, вердикт).
    static std::string describe(const StabilityReport& report);

    /// Пустая строка для устойчивой системы, иначе текст с "WARNING:".
    static std::string warningText(const StabilityReport& report);

    /// Печатает предупреждение в поток, если система неустойчива.
    /// @return true, если предупреждение было напечатано.
    static bool printWarningIfNeeded(const StabilityReport& report, std::ostream& os);
};

#pragma once

#include <complex>
#include <iosfwd>
#include <string>
#include <vector>

enum class StabilityVerdict
{
    Stable,     // все |z| < 1
    Marginal,   // есть |z| = 1
    Unstable    // есть |z| > 1
};

struct StabilityReport
{
    std::string modelName;
    std::vector<double> coefficients;
    std::vector<std::complex<double>> roots;
    double spectralRadius = 0.0;
    StabilityVerdict verdict = StabilityVerdict::Stable;
};

class StabilityAnalyzer
{
public:
    static std::vector<std::complex<double>> findRoots(const std::vector<double>& coefficients);
    static StabilityVerdict schurVerdict(const std::vector<double>& coefficients);
    static bool isStable(const std::vector<double>& coefficients);
    static bool needsWarning(const StabilityReport& report);

    // Спектральный радиус (max |z_i|).
    static double spectralRadius(const std::vector<std::complex<double>>& roots);

    // Model 1.6: z^3 - a1*z^2 - a2*z - a3 = 0
    static StabilityReport analyzeModel1_6(double a1, double a2, double a3);
    static std::vector<double> characteristicPolynomial1_6(double a1, double a2, double a3);

    // true, если система на границе устойчивости (есть корень |z| = 1).
    static bool isMarginallyStable(const std::vector<double>& coefficients);

    static std::string describe(const StabilityReport& report);

    // Пустая строка для устойчивой системы, иначе текст с "WARNING:".
    static std::string warningText(const StabilityReport& report);

    // Печатает предупреждение в поток, если система неустойчива.
    // Возвращает true, если предупреждение было напечатано.
    static bool printWarningIfNeeded(const StabilityReport& report, std::ostream& os);
};

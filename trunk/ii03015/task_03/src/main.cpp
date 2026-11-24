/**
 * @file main.cpp
 * @brief Точка входа: замкнутая система «ПИД-регулятор + объект управления».
 *
 * Лабораторная работа №3, вариант 6. Программа:
 *  - даёт выбрать один из трёх объектов управления из ЛР №1 (модели 1.6, 2.8, 3.10);
 *  - для линейной модели 1.6 перед стартом печатает отчёт об устойчивости (ЛР №2);
 *  - принимает настройки ПИД-регулятора (Kp, Ki, Kd, dt, uMin, uMax);
 *  - работает в одном из двух режимов: А — фиксированное число тактов n,
 *    Б — до достижения установившегося режима |e| < eps;
 *  - выводит результаты строгой таблицей t / w / e(t) / u(t) / y(t);
 *  - сохраняет данные в pid_result.csv для построения графиков (plot.py);
 *    в CSV дополнительно пишется колонка y_open — отклик того же объекта
 *    без регулятора на единичный скачок (для сравнительной кривой на графике).
 */
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "Model.h"
#include "Model1_6.h"
#include "Model2_8.h"
#include "Model3_10.h"
#include "PIDController.h"
#include "StabilityAnalyzer.h"

// ---------------------------------------------------------------------------
//  Прототипы вспомогательных функций
// ---------------------------------------------------------------------------
double checkInputDouble(const std::string& text);
int checkInputInt(const std::string& text, int minVal, int maxVal);
std::unique_ptr<Model> createPlant(int userChoice, double& plantDt);
PIDController createPid();
void selectMode(int& mode, int& n, double& eps);
void simulate(Model& plant, PIDController& pid, double w, int mode, int n,
              double eps, double plantDt, std::ofstream& file);

/**
 * @brief Точка входа программы.
 * @return 0 при успехе, 1 при отказе от симуляции или ошибке файла.
 */
int main()
{
    std::remove("pid_result.csv");

    std::cout << "Choose controlled plant:" << std::endl;
    std::cout << " 1 - Model 1.6  (linear, 3rd order)" << std::endl;
    std::cout << " 2 - Model 2.8  (nonlinear logistic map)" << std::endl;
    std::cout << " 3 - Model 3.10 (differential equation, Euler)" << std::endl;
    const int userChoice = checkInputInt("Your choice: ", 1, 3);

    double plantDt = 1.0;
    std::unique_ptr<Model> plant = createPlant(userChoice, plantDt);
    if (!plant)
    {
        std::cout << "Simulation cancelled." << std::endl;
        return 1;
    }

    PIDController pid = createPid();

    const double w = checkInputDouble("Target value w (setpoint) = ");

    int mode = 1;
    int n = 0;
    double eps = 0.0;
    selectMode(mode, n, eps);

    std::cout << std::setw(5)  << "t"
              << std::setw(12) << "w"
              << std::setw(12) << "e(t)"
              << std::setw(12) << "u(t)"
              << std::setw(12) << "y(t)" << std::endl;
    std::cout << std::string(53, '-') << std::endl;

    std::ofstream file("pid_result.csv");
    if (!file)
    {
        std::cout << "Error: cannot create pid_result.csv" << std::endl;
        return 1;
    }
    file << "t;w;e;u;y;y_open" << std::endl;
    file << std::fixed << std::setprecision(4);

    simulate(*plant, pid, w, mode, n, eps, plantDt, file);
    file.close();

    std::cout << std::endl << "Data saved to pid_result.csv" << std::endl;
    std::cout << "Run 'python plot.py' to draw graphs." << std::endl;
    return 0;
}

/**
 * @brief Создаёт выбранный объект управления и (для модели 1.6) проверяет устойчивость.
 * @param userChoice номер модели (1..3)
 * @param plantDt    [out] шаг интегрирования для модели 3.10
 * @return указатель на модель или nullptr, если пользователь отказался симулировать неустойчивую модель
 */
std::unique_ptr<Model> createPlant(int userChoice, double& plantDt)
{
    switch (userChoice)
    {
    case 1:
    {
        std::cout << "Model 1.6: y(t+1) = a1*y(t) + a2*y(t-1) + a3*y(t-2) + b*u(t)" << std::endl;
        const double a1 = checkInputDouble("a1 = ");
        const double a2 = checkInputDouble("a2 = ");
        const double a3 = checkInputDouble("a3 = ");
        const double b  = checkInputDouble("b  = ");
        std::cout << std::endl;

        const StabilityReport report = StabilityAnalyzer::analyzeModel1_6(a1, a2, a3);
        std::cout << StabilityAnalyzer::describe(report) << std::endl;

        if (StabilityAnalyzer::needsWarning(report))
        {
            std::cout << StabilityAnalyzer::warningText(report) << std::endl;
            std::cout << "Simulate unstable plant? (Y/N): ";
            std::string answer;
            std::cin >> answer;
            if (!(answer == "y" || answer == "Y"))
                return nullptr;
        }
        return std::make_unique<Model1_6>(a1, a2, a3, b);
    }
    case 2:
    {
        std::cout << "Model 2.8: y(t+1) = a*y(t)*(1 - y(t)) + b*u(t) + c*sin(y(t-1)*u(t))" << std::endl;
        const double a = checkInputDouble("a = ");
        const double b = checkInputDouble("b = ");
        const double c = checkInputDouble("c = ");
        return std::make_unique<Model2_8>(a, b, c);
    }
    case 3:
    {
        std::cout << "Model 3.10: dy/dt = -a*y + b + u" << std::endl;
        const double a = checkInputDouble("a = ");
        const double b = checkInputDouble("b = ");
        while (true)
        {
            plantDt = checkInputDouble("plant dt (Euler step) = ");
            if (plantDt > 0) break;
            std::cout << "dt must be positive!" << std::endl;
        }
        return std::make_unique<Model3_10>(a, b);
    }
    default:
        return nullptr;   // недостижимо: checkInputInt держит диапазон
    }
}

/// Запрашивает у пользователя коэффициенты и пределы ПИД-регулятора.
PIDController createPid()
{
    std::cout << std::endl << "PID controller settings:" << std::endl;
    const double Kp = checkInputDouble("Kp = ");
    const double Ki = checkInputDouble("Ki = ");
    const double Kd = checkInputDouble("Kd = ");
    double dt;
    while (true)
    {
        dt = checkInputDouble("dt (sampling step) = ");
        if (dt > 0) break;
        std::cout << "dt must be positive!" << std::endl;
    }
    double uMin;
    double uMax;
    while (true)
    {
        uMin = checkInputDouble("PID uMin = ");
        uMax = checkInputDouble("PID uMax = ");
        if (uMin < uMax) break;
        std::cout << "uMin must be less than uMax!" << std::endl;
    }
    return PIDController(Kp, Ki, Kd, dt, uMin, uMax);
}

/**
 * @brief Выбор режима симуляции.
 * @param mode [out] 1 — фиксированное n тактов; 2 — до |e| < eps
 * @param n    [out] число тактов (режим А) или предохранительный лимит (режим Б)
 * @param eps  [out] целевая точность (режим Б)
 */
void selectMode(int& mode, int& n, double& eps)
{
    std::cout << "Choose simulation mode:" << std::endl;
    std::cout << " 1 - fixed number of steps (n)" << std::endl;
    std::cout << " 2 - until |e| < eps (steady state)" << std::endl;
    mode = checkInputInt("Your choice: ", 1, 2);
    if (mode == 1)
    {
        n = checkInputInt("Input number of steps (n): ", 1, 100000);
    }
    else
    {
        while (true)
        {
            eps = checkInputDouble("eps (target accuracy) = ");
            if (eps > 0) break;
            std::cout << "eps must be positive!" << std::endl;
        }
        n = 100000;
    }
}

/**
 * @brief Прогон замкнутого контура «ПИД + объект» с выводом таблицы и записью CSV.
 * @param plant   объект управления
 * @param pid     регулятор
 * @param w       уставка
 * @param mode    1 — n тактов, 2 — до |e| < eps
 * @param n       число тактов (или лимит)
 * @param eps     целевая точность (режим Б)
 * @param plantDt шаг интегрирования объекта (модель 3.10)
 * @param file    поток для записи результатов в CSV
 */
void simulate(Model& plant, PIDController& pid, double w, int mode, int n,
              double eps, double plantDt, std::ofstream& file)
{
    // --- Замкнутый контур «ПИД + объект»: печатаем таблицу и копим данные ---
    std::vector<double> errs;
    std::vector<double> ctrls;
    std::vector<double> outs;
    double y = 0.0;
    double yPrev = 0.0;
    double yPrev2 = 0.0;
    bool reached = false;

    for (int t = 0; t < n; ++t)
    {
        const double e = w - y;
        const double u = pid.step(w, y);
        const double nextY = plant.calculateNext(y, yPrev, yPrev2, u, plantDt);
        yPrev2 = yPrev;
        yPrev = y;
        y = nextY;

        std::cout << std::setw(5)  << t
                  << std::setw(12) << std::fixed << std::setprecision(4) << w
                  << std::setw(12) << e
                  << std::setw(12) << u
                  << std::setw(12) << y << std::endl;

        errs.push_back(e);
        ctrls.push_back(u);
        outs.push_back(y);

        if (mode == 2 && std::fabs(e) < eps)
        {
            reached = true;
            std::cout << std::endl;
            std::cout << "Steady state reached: |e| < " << std::fixed
                      << std::setprecision(4) << eps << " in " << (t + 1)
                      << " steps." << std::endl;
            break;
        }
    }

    // --- Открытый контур: тот же объект без регулятора на единичном скачке
    //     u = 1. Нужен только для наглядной кривой сравнения на графике. ---
    std::vector<double> openOut;
    {
        double yo = 0.0;
        double yoPrev = 0.0;
        double yoPrev2 = 0.0;
        for (std::size_t t = 0; t < outs.size(); ++t)
        {
            const double nextY =
                plant.calculateNext(yo, yoPrev, yoPrev2, 1.0, plantDt);
            yoPrev2 = yoPrev;
            yoPrev = yo;
            yo = nextY;
            openOut.push_back(yo);
        }
    }

    // --- Запись результатов в CSV: t;w;e;u;y;y_open ---
    for (std::size_t t = 0; t < outs.size(); ++t)
    {
        file << t << ";" << w << ";" << errs[t] << ";" << ctrls[t]
             << ";" << outs[t] << ";" << openOut[t] << std::endl;
    }

    if (mode == 2 && !reached)
    {
        std::cout << std::endl;
        std::cout << "WARNING: |e| >= " << std::fixed << std::setprecision(4)
                  << eps << " within the step limit." << std::endl;
    }
}

/// Ввод вещественного числа с защитой от некорректного ввода.
double checkInputDouble(const std::string& text)
{
    double x;
    while (true)
    {
        std::cout << text;
        if (std::cin >> x) return x;
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Incorrect input! Try another number." << std::endl;
    }
}

/// Ввод целого числа в диапазоне [minVal, maxVal] с защитой от некорректного ввода.
int checkInputInt(const std::string& text, int minVal, int maxVal)
{
    int x;
    while (true)
    {
        std::cout << text;
        if ((std::cin >> x) && x >= minVal && x <= maxVal) return x;
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Enter integer from " << minVal << " to " << maxVal << "!" << std::endl;
    }
}

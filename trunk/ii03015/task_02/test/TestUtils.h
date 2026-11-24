#pragma once

// ---------------------------------------------------------------------------
//  Общие вспомогательные средства для всех тестов ЛР №2.
// ---------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <cmath>
#include <functional>
#include <sstream>
#include <string>
#include <vector>

#include "Model.h"
#include "Model1_6.h"
#include "Model2_8.h"
#include "Model3_10.h"
#include "Simulation.h"

namespace otis_test
{
// Единый допуск для сравнения чисел с плавающей точкой.
// 1e-12 достижим, потому что все эталоны посчитаны в double независимым
// способом (NumPy) либо вручную; больший запас не нужен и лишь ослаблял бы
// проверку.
constexpr double kEps = 1e-12;

// Прогон модели: n шагов, постоянное u. Возвращает y на каждом такте,
// начиная с начального состояния (элемент 0 - это y0).
inline std::vector<double> runConstant(const Model& model, double y0, double u, int steps,
                                       double dt = 1.0)
{
    const SimulationConfig config{steps, y0, dt};
    const CallableInputSource input([u](int) { return u; });
    std::vector<double> values;
    for (const SimStep& step : simulate(model, config, input))
        values.push_back(step.y);
    return values;
}

// Прогон модели с произвольным сигналом u(tau).
inline std::vector<double> runSignal(const Model& model, double y0, int steps,
                                     const std::function<double(int)>& u, double dt = 1.0)
{
    const SimulationConfig config{steps, y0, dt};
    const CallableInputSource input(u);
    std::vector<double> values;
    for (const SimStep& step : simulate(model, config, input))
        values.push_back(step.y);
    return values;
}

// Сравнение векторов с допуском.
inline void expectNear(const std::vector<double>& actual, const std::vector<double>& expected,
                       double eps = kEps)
{
    ASSERT_EQ(actual.size(), expected.size());
    for (std::size_t i = 0; i < expected.size(); ++i)
        EXPECT_NEAR(actual[i], expected[i], eps) << "index = " << i;
}

// Вывод значения в строку (для имен параметризованных тестов).
template <typename T>
std::string toStr(const T& value)
{
    std::ostringstream out;
    out << value;
    return out.str();
}
} // namespace otis_test

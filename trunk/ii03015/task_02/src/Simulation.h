#pragma once

#include <functional>
#include <vector>

#include "InputSignal.h"
#include "Model.h"

// ---------------------------------------------------------------------------
//  Один шаг симуляции.
// ---------------------------------------------------------------------------
struct SimStep
{
    int tau = 0;
    double u = 0.0;
    double y = 0.0;
};

struct SimulationConfig
{
    int steps = 10;      // количество шагов интегрирования n
    double y0 = 0.0;     // начальное условие
    double dt = 1.0;     // шаг интегрирования (используется Model 3.10)
};

// ---------------------------------------------------------------------------
//  Источник входного сигнала (интерфейс позволяет подменять сигнал в тестах,
//  в том числе через Google Mock).
// ---------------------------------------------------------------------------
class IInputSource
{
public:
    virtual ~IInputSource() = default;

    // Значение входного воздействия на шаге tau.
    virtual double valueAt(int tau) const = 0;
};

// Сигнал из InputSignal.h (ступенька / импульс / гармоника).
class SignalInputSource final : public IInputSource
{
public:
    SignalInputSource(SignalType type, double amplitude);

    double valueAt(int tau) const override;

    SignalType getType() const;
    double getAmplitude() const;

private:
    SignalType type_;
    double amplitude_;
};

// Произвольная функция u(tau) - удобно для тестов и нестандартных сигналов.
class CallableInputSource final : public IInputSource
{
public:
    explicit CallableInputSource(std::function<double(int)> function);

    double valueAt(int tau) const override;

private:
    std::function<double(int)> function_;
};

// ---------------------------------------------------------------------------
//  Собственно прогон модели.
//  rows[0]         - начальное состояние (tau = 0, y = y0);
//  rows[t + 1]     - значение после t + 1 шагов.
// ---------------------------------------------------------------------------
std::vector<SimStep> simulate(const Model& model,
                              const SimulationConfig& config,
                              const IInputSource& input);

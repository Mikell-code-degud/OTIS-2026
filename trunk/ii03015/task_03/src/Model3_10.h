/**
 * @file Model3_10.h
 * @brief Модель 3.10 (вариант 6): обыкновенное дифференциальное уравнение.
 *
 * Уравнение: dy/dt = -a*y + b + u.
 * Дискретизация методом Эйлера: y(t+1) = y(t) + dt*(-a*y(t) + b + u(t)).
 */
#pragma once
#include "Model.h"

/// Модель в виде дифференциального уравнения (объект управления блока 3 ЛР №1).
class Model3_10 final : public Model
{
private:
    double a;  ///< коэффициент при выходной переменной
    double b;  ///< постоянное внешнее смещение

public:
    /**
     * @brief Конструктор модели дифференциального уравнения.
     * @param a,b коэффициенты уравнения
     */
    Model3_10(double a, double b);

    double calculateNext(double y, double yPrev, double yPrev2,
                         double u, double dt) const override;

    const char* getName() const override;
};

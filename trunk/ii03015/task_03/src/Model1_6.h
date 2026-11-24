/**
 * @file Model1_6.h
 * @brief Линейная модель 1.6 (вариант 6): линейная динамическая система 3-го порядка.
 *
 * Уравнение: y(t+1) = a1*y(t) + a2*y(t-1) + a3*y(t-2) + b*u(t).
 */
#pragma once
#include "Model.h"

/// Линейная модель третьего порядка (объект управления блока 1 ЛР №1).
class Model1_6 final : public Model
{
private:
    double a1;  ///< коэффициент при y(t)
    double a2;  ///< коэффициент при y(t-1)
    double a3;  ///< коэффициент при y(t-2)
    double b;   ///< коэффициент усиления по управлению

public:
    /**
     * @brief Конструктор линейной модели.
     * @param a1,a2,a3 коэффициенты авторегрессии
     * @param b коэффициент при управляющем воздействии
     */
    Model1_6(double a1, double a2, double a3, double b);

    double calculateNext(double y, double yPrev, double yPrev2,
                         double u, double dt) const override;

    const char* getName() const override;
};

/**
 * @file Model2_8.h
 * @brief Нелинейная модель 2.8 (вариант 6): логистическая карта + гармоническое возмущение.
 *
 * Уравнение: y(t+1) = a*y(t)*(1 - y(t)) + b*u(t) + c*sin(y(t-1)*u(t)).
 */
#pragma once
#include "Model.h"

/// Нелинейная модель: логистическое отображение с возмущением (объект управления блока 2 ЛР №1).
class Model2_8 final : public Model
{
private:
    double a;  ///< параметр логистической карты
    double b;  ///< коэффициент усиления по управлению
    double c;  ///< амплитуда нелинейного возмущения

public:
    /**
     * @brief Конструктор нелинейной модели.
     * @param a,b,c коэффициенты модели
     */
    Model2_8(double a, double b, double c);

    double calculateNext(double y, double yPrev, double yPrev2,
                         double u, double dt) const override;

    const char* getName() const override;
};

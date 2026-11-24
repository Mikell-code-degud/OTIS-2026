#pragma once
#include "Model.h"

class Model3_10 final : public Model
{
private:
    double a, b;

public:
    Model3_10(double a, double b);

    double calculateNext(double y, double yPrev, double yPrev2,
                         double u, double dt) const override;

    const char* getName() const override;

    // --- Добавлено в ЛР №2: доступ к коэффициентам для модуля устойчивости и тестов ---
    double getA() const;
    double getB() const;
};

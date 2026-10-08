#include "Model1_6.h"

Model1_6::Model1_6(double a1, double a2, double a3, double b)
    : a1(a1), a2(a2), a3(a3), b(b) {}

double Model1_6::calculateNext(double y, double yPrev, double yPrev2,
                               double u, double /*dt*/) const
{
    // Model 1.6: y(t+1) = a1*y(t) + a2*y(t-1) + a3*y(t-2) + b*u(t)
    return a1 * y + a2 * yPrev + a3 * yPrev2 + b * u;
}

const char* Model1_6::getName() const
{
    return "Model 1.6 - Third-Order Dynamic System";
}

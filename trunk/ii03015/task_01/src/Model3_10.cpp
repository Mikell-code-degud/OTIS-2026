#include "Model3_10.h"

Model3_10::Model3_10(double a, double b)
    : a(a), b(b) {}

double Model3_10::calculateNext(double y, double /*yPrev*/, double /*yPrev2*/,
                                double u, double dt) const
{
    // Model 3.10: dy/dt = -a*y + b + u
    // Euler: y(t+1) = y(t) + dt*(-a*y(t) + b + u(t))
    return y + dt * (-a * y + b + u);
}

const char* Model3_10::getName() const
{
    return "Model 3.10 - Basic External Constant Offset";
}

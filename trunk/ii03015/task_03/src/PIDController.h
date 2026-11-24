#pragma once

class PIDController
{
private:
    double Kp;
    double Ki;
    double Kd;
    double dt;
    double uMin;
    double uMax;
    double u_prev  = 0;
    double e_prev1 = 0;
    double e_prev2 = 0;

public:

    PIDController(double Kp, double Ki, double Kd, double dt, double uMin, double uMax)
        : Kp(Kp), Ki(Ki), Kd(Kd), dt(dt), uMin(uMin), uMax(uMax)
    {
    }

    double step(double w, double y)
    {
        const double e = w - y;
        const double dU = Kp * (e - e_prev1)
                        + Ki * e * dt
                        + (Kd / dt) * (e - 2.0 * e_prev1 + e_prev2);
        double u = u_prev + dU;
        if (u > uMax) u = uMax;
        else if (u < uMin) u = uMin;
        e_prev2 = e_prev1;
        e_prev1 = e;
        u_prev = u;
        return u;
    }

    void reset()
    {
        u_prev = 0;
        e_prev1 = 0;
        e_prev2 = 0;
    }
};

/**
 * @file PIDController.h
 * @brief Дискретный ПИД-регулятор в рекуррентной (инкрементной) форме.
 *
 * Непрерывный закон: u(t) = Kp*e + Ki*integral(e) + Kd*de/dt, где e = w - y.
 * Дискретная рекуррентная форма («алгоритм скорости»):
 *
 *   du   = Kp*(e - e_{t-1}) + Ki*e*dt + (Kd/dt)*(e - 2*e_{t-1} + e_{t-2})
 *   u(t) = u(t-1) + du,  с обрезкой u в [uMin, uMax].
 *
 * Обрезка управляющего сигнала по физическим пределам актуатора одновременно
 * играет роль простейшего anti-windup. Память регулятора — три величины:
 * u_prev = u(t-1), e_prev1 = e(t-1), e_prev2 = e(t-2).
 */
#pragma once

/// Дискретный ПИД-регулятор (рекуррентная форма, обрезка по пределам актуатора).
class PIDController
{
private:
    double Kp;          ///< пропорциональный коэффициент
    double Ki;          ///< интегральный коэффициент
    double Kd;          ///< дифференциальный коэффициент
    double dt;          ///< шаг квантования (sampling step)
    double uMin;        ///< нижний предел управляющего сигнала
    double uMax;        ///< верхний предел управляющего сигнала
    double u_prev  = 0; ///< u(t-1)
    double e_prev1 = 0; ///< e(t-1)
    double e_prev2 = 0; ///< e(t-2)

public:
    /**
     * @brief Конструктор регулятора.
     * @param Kp,Ki,Kd коэффициенты регулятора
     * @param dt       шаг квантования (должен быть > 0)
     * @param uMin,uMax физические пределы актуатора
     */
    PIDController(double Kp, double Ki, double Kd, double dt, double uMin, double uMax)
        : Kp(Kp), Ki(Ki), Kd(Kd), dt(dt), uMin(uMin), uMax(uMax)
    {
    }

    /**
     * @brief Один такт работы регулятора.
     * @param w задающее воздействие (уставка)
     * @param y текущее значение регулируемой величины
     * @return управляющее воздействие u(t)
     */
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

    /// Сброс памяти регулятора в начальное состояние.
    void reset()
    {
        u_prev = 0;
        e_prev1 = 0;
        e_prev2 = 0;
    }
};

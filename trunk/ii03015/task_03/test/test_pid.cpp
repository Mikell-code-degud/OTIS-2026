#include <gtest/gtest.h>

#include <cmath>
#include <string>

#include "Model1_6.h"
#include "Model2_8.h"
#include "Model3_10.h"
#include "PIDController.h"
#include "StabilityAnalyzer.h"

TEST(PIDController, MathClampAndReset)
{
    {
        SCOPED_TRACE("первый такт: пустая история ошибок, du = Kp*e + Ki*e*dt");
        PIDController pid(2.0, 0.5, 0.2, 1.0, -10.0, 10.0);

        EXPECT_DOUBLE_EQ(pid.step(1.0, 0.0), 2.7);
    }
    {
        SCOPED_TRACE("второй такт: учитывается история ошибок e(t-1), e(t-2)");
        PIDController pid(2.0, 0.5, 0.2, 1.0, -10.0, 10.0);
        pid.step(1.0, 0.0);
        EXPECT_NEAR(pid.step(1.0, 0.5), 2.7 - 1.05, 1e-12);
    }
    {
        SCOPED_TRACE("обрезка по пределам актуатора и удержание на границе");
        PIDController pid(100.0, 0.0, 0.0, 1.0, -5.0, 5.0);
        EXPECT_DOUBLE_EQ(pid.step(50.0, 0.0), 5.0);
        EXPECT_DOUBLE_EQ(pid.step(50.0, 0.0), 5.0);
    }
    {
        SCOPED_TRACE("reset() полностью обнуляет память регулятора");
        PIDController pid(2.0, 0.5, 0.2, 1.0, -10.0, 10.0);
        pid.step(1.0, 0.0);
        pid.reset();
        EXPECT_DOUBLE_EQ(pid.step(1.0, 0.0), 2.7);
    }
}

TEST(ClosedLoop, FollowsModel16ReferenceTrajectory)
{

    PIDController pid(0.2, 0.1, 0.0, 1.0, -100.0, 100.0);
    Model1_6 plant(0.4, 0.3, 0.1, 1.0);

    const double expectedU[] = {3.0, 3.1, 3.41, 3.461};
    const double expectedY[] = {3.0, 4.3, 6.03, 7.463};

    double y = 0.0, yPrev = 0.0, yPrev2 = 0.0;
    for (int t = 0; t < 4; ++t)
    {
        SCOPED_TRACE("такт " + std::to_string(t));
        const double u = pid.step(10.0, y);
        EXPECT_NEAR(u, expectedU[t], 1e-9);
        const double nextY = plant.calculateNext(y, yPrev, yPrev2, u, 1.0);
        yPrev2 = yPrev;
        yPrev = y;
        y = nextY;
        EXPECT_NEAR(y, expectedY[t], 1e-9);
    }
}

TEST(ClosedLoop, ReachesSetpointWithinEps)
{

    PIDController pid(0.3, 0.1, 0.0, 1.0, -100.0, 100.0);
    Model1_6 plant(0.4, 0.3, 0.1, 1.0);

    const double w = 10.0;
    double y = 0.0, yPrev = 0.0, yPrev2 = 0.0;
    for (int t = 0; t < 200; ++t)
    {
        const double u = pid.step(w, y);
        const double nextY = plant.calculateNext(y, yPrev, yPrev2, u, 1.0);
        yPrev2 = yPrev;
        yPrev = y;
        y = nextY;
    }
    EXPECT_NEAR(w - y, 0.0, 0.05);
}

TEST(Plants, ReturnKnownValues)
{
    {
        SCOPED_TRACE("Model 1.6: y(t+1) = a1*y + a2*yPrev + a3*yPrev2 + b*u");
        Model1_6 m(0.5, 0.3, 0.2, 2.0);

        EXPECT_DOUBLE_EQ(m.calculateNext(1.0, 2.0, 3.0, 1.0, 1.0), 3.7);
        EXPECT_STREQ(m.getName(), "Model 1.6 - Third-Order Dynamic System");
    }
    {
        SCOPED_TRACE("Model 2.8: логистическая карта + sin-возмущение");
        Model2_8 m(1.0, 2.0, 3.0);

        EXPECT_NEAR(m.calculateNext(0.5, 1.0, 0.0, 1.0, 1.0),
                    2.25 + 3.0 * std::sin(1.0), 1e-12);
    }
    {
        SCOPED_TRACE("Model 3.10: шаг Эйлера для dy/dt = -a*y + b + u");
        Model3_10 m(2.0, 1.0);

        EXPECT_DOUBLE_EQ(m.calculateNext(1.0, 0.0, 0.0, 3.0, 0.5), 2.0);
    }
}

TEST(Stability, LinearModelVerdictsAndReport)
{
    {
        SCOPED_TRACE("устойчивый набор коэффициентов");
        const StabilityReport report = StabilityAnalyzer::analyzeModel1_6(0.4, 0.3, 0.1);
        EXPECT_EQ(report.verdict, StabilityVerdict::Stable);
        EXPECT_LT(report.spectralRadius, 1.0);
        EXPECT_FALSE(StabilityAnalyzer::needsWarning(report));
        EXPECT_TRUE(StabilityAnalyzer::describe(report).find("STABLE") != std::string::npos);
    }
    {
        SCOPED_TRACE("неустойчивый набор: корень z = 2");
        const StabilityReport report = StabilityAnalyzer::analyzeModel1_6(2.0, 0.0, 0.0);
        EXPECT_EQ(report.verdict, StabilityVerdict::Unstable);
        EXPECT_GT(report.spectralRadius, 1.0);
        EXPECT_TRUE(StabilityAnalyzer::needsWarning(report));
        EXPECT_FALSE(StabilityAnalyzer::warningText(report).empty());
    }
    {
        SCOPED_TRACE("стабильность тривиальных полиномов");
        EXPECT_TRUE(StabilityAnalyzer::isStable({1.0}));
        EXPECT_TRUE(StabilityAnalyzer::isStable({1.0, 2.0, 0.0}));
    }
}

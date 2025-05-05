//
// Created by eeasee on 5/5/25.
//

#pragma once

#include "MathUtil.hpp"
#include "main.h"

namespace Utils
{
class PID
{
   public:
    PID(float paramKPOnTarget,
        float paramKPOnMeasurement,
        float paramKI,
        float paramKD,
        float paramDerivAlpha,
        float paramIntegralMax)
        : kPOnTarget(paramKPOnTarget),
          kP(paramKPOnMeasurement),
          kI(paramKI),
          kD(paramKD),
          derivativeLPFAlpha(paramDerivAlpha),
          integralMax(paramIntegralMax)
    {
    }

    float update(float target, float measurement)
    {
        this->error      = target - measurement;
        this->derivative = (lastMeasurement - measurement) * derivativeLPFAlpha + derivative * (1 - derivativeLPFAlpha);
        this->lastMeasurement = measurement;
        this->integral += error;
        this->integral = M_CLAMP(this->integral, -integralMax, integralMax);
        return (kP * error + kI * integral + kD * derivative);
    }

    float kPOnTarget;
    float kP;
    float kI;
    float kD;
    float derivativeLPFAlpha;
    float integralMax;

    float lastMeasurement = 0;
    float integral        = 0;
    float derivative      = 0;
    float error           = 0;
};
}  // namespace Utils
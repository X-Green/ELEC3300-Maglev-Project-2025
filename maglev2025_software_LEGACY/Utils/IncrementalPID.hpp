//
// Created by eeasee on 2025/3/11.
//

#pragma once

#include "Config.hpp"

namespace Utils
{

class IncrementalPID
{
   public:
    IncrementalPID(float paramKPOnTarget, float paramKPOnMeasurement, float paramKI, float paramKD);

    void update(float target, float measurement)
    {
        this->deltaOutput = this->kPOnTarget * (target - this->lastTarget) - this->kPOnMeasurement * (measurement - this->lastMeasurement) +
                            this->kI * (target - measurement) + this->kD * (measurement - 2.0f * this->lastMeasurement + this->lastLastMeasurement);
        this->lastTarget          = target;
        this->lastLastMeasurement = this->lastMeasurement;
        this->lastMeasurement     = measurement;
    }

    void updateDataNoOutput(float target, float measurement)
    {
        this->lastTarget          = target;
        this->lastLastMeasurement = this->lastMeasurement;
        this->lastMeasurement     = measurement;
    }

    float getDeltaOutput() const { return this->deltaOutput; }

    float getDeltaI() const { return this->kI * (this->lastTarget - this->lastMeasurement); }

    void reset()
    {
        this->lastTarget      = 0.0f;
        this->lastMeasurement = 0.0f;
        this->deltaOutput     = 0.0f;
    }

   private:
    float kPOnTarget;
    float kPOnMeasurement;
    float kI;
    float kD;

    float lastMeasurement;
    float lastLastMeasurement;
    float lastTarget;
    float deltaOutput;

    struct MultiplicationParameter
    {
        float paramTarget;
        float paramLastTarget;
        float paramMeasurement;
        float paramLastMeasurement;
        float paramLastLastMeasurement;
    };

    MultiplicationParameter multiplicationParameter;
};

}  // namespace Utils
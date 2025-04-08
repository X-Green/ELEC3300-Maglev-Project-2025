//
// Created by xiaol on 2025/3/11.
//

#include "IncrementalPID.hpp"

Utils::IncrementalPID::IncrementalPID(float paramKPOnTarget, float paramKPOnMeasurement, float paramKI, float paramKD)
    : kPOnTarget(paramKPOnTarget),
      kPOnMeasurement(paramKPOnMeasurement),
      kI(paramKI),
      kD(paramKD),
      lastMeasurement(0.0f),
      lastLastMeasurement(0.0f),
      lastTarget(0.0f),
      deltaOutput(0.0f),
      multiplicationParameter({0, 0, 0, 0, 0})
{
    configASSERT(this->kPOnTarget >= 0.0f);
    configASSERT(this->kPOnMeasurement >= 0.0f);
    configASSERT(this->kI >= 0.0f);
    configASSERT(this->kD >= 0.0f);

    this->multiplicationParameter.paramTarget              = this->kPOnTarget + this->kI;
    this->multiplicationParameter.paramMeasurement         = this->kPOnMeasurement - this->kI + this->kD;
    this->multiplicationParameter.paramLastTarget          = -1 * this->kPOnTarget;
    this->multiplicationParameter.paramLastMeasurement     = -1 * this->kPOnMeasurement - 2.0f * this->kD;
    this->multiplicationParameter.paramLastLastMeasurement = this->kD;
}

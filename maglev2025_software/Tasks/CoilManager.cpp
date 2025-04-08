//
// Created by eeasee on 3/12/25.
//

#include "CoilManager.hpp"

namespace Tasks::CoilManager
{

float coilActualCurrent[4] = {0, 0, 0, 0};
float coilTargetCurrent[4] = {0, 0, 0, 0};

/**
 * Output Value: -1 < v < 1
 * Actual Duty Cycle: d = 0.5*v+0.5
 * (Actually it should be a curve with input from +-inf. But here balabala)
 */
float coilOutputValue[4] = {0, 0, 0, 0};
float enableOutput       = false;

void initCoils() {}

float currentGain           = 1.0f;
float currentBias           = 0.0f;
const float currentLPFAlpha = 0.5f;

void updateCoils()
{
    for (int i = 0; i < 4; ++i)
    {
        coilActualCurrent[i] =
            ((float)SampleTask::adc2Buffer[i] * currentGain + currentBias) * currentLPFAlpha + coilActualCurrent[i] * (1.0f - currentLPFAlpha);
    }
}
void updatePWM(int index, float outputValue)
{
    float duty = 0.5f * outputValue + 0.5f;
    assert_param(duty < 1.0f && duty > 0.0f);



}

}  // namespace Tasks::CoilManager
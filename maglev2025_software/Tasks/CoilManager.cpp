//
// Created by eeasee on 3/12/25.
//

#include "CoilManager.hpp"

namespace Tasks::CoilManager
{

void setOutputEnable()
{
    enableOutput = true;
    hhrtim1.Instance->sCommonRegs.OENR |= HRTIM_OUTPUT_TA1 | HRTIM_OUTPUT_TA2 | HRTIM_OUTPUT_TB1 | HRTIM_OUTPUT_TB2 | HRTIM_OUTPUT_TC1 |
                                          HRTIM_OUTPUT_TC2 | HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2;
}

void setOutputDisable()
{
    enableOutput = false;
    hhrtim1.Instance->sCommonRegs.ODISR |= HRTIM_OUTPUT_TA1 | HRTIM_OUTPUT_TA2 | HRTIM_OUTPUT_TB1 | HRTIM_OUTPUT_TB2 | HRTIM_OUTPUT_TC1 |
                                           HRTIM_OUTPUT_TC2 | HRTIM_OUTPUT_TD1 | HRTIM_OUTPUT_TD2;
}

Utils::IncrementalPID coilPID[4] = {
    Utils::IncrementalPID(0.0f, 0.001f, 0.0005f, 0.0f),
    Utils::IncrementalPID(0.0f, 0.001f, 0.0005f, 0.0f),
    Utils::IncrementalPID(0.0f, 0.001f, 0.0005f, 0.0f),
    Utils::IncrementalPID(0.0f, 0.001f, 0.0005f, 0.0f),
};

void initCoils() {}

float currentGain           = 1.0f;
float currentBias           = 0.0f;
const float currentLPFAlpha = 0.5f;

void updateCoilsFast()
{
    for (int i = 0; i < 4; ++i)
    {
        coilActualCurrent[i] =
            ((float)SampleTask::adc2Buffer[i] * currentGain + currentBias) * currentLPFAlpha + coilActualCurrent[i] * (1.0f - currentLPFAlpha);

        if (!enableOutput)
        {
            coilOutputValue[i] = 0.0f;  // Prevent Integration on not enabled.
            coilPID[i].updateDataNoOutput(0, coilActualCurrent[i]);
            updatePWM(i, 0.0f);
        }
        else
        {
            coilPID[i].update(coilTargetCurrent[i], coilActualCurrent[i]);
            coilOutputValue[i] += coilPID[i].getDeltaOutput();
            coilOutputValue[i] = M_CLAMP(coilOutputValue[i], -0.9f, 0.9f);
            updatePWM(i, coilOutputValue[i]);
        }
    }
}

void updateCoilsSlow() {}

constexpr uint32_t HRTIM_PERIOD = 64000;

void updatePWM(int hrtimTimerIndex, float outputValue)
{
    float duty = 0.5f * outputValue + 0.5f;
    assert_param(duty < 1.0f && duty > 0.0f);
    auto cmp1                                   = (uint32_t)((HRTIM_PERIOD / 2.0f) * (1.0f - duty));
    auto cmp3                                   = (uint32_t)((HRTIM_PERIOD / 2.0f) * (1.0f + duty));
    HRTIM1->sTimerxRegs[hrtimTimerIndex].CMP1xR = cmp1;
    HRTIM1->sTimerxRegs[hrtimTimerIndex].CMP3xR = cmp3;
}

}  // namespace Tasks::CoilManager
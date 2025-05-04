//
// Created by eeasee on 3/12/25.
//

#include "SampleTask.hpp"

#include "adc.h"

namespace Tasks::SampleTask
{

void sampleTaskInit()
{
    HAL_Delay(100);
    HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
    HAL_Delay(100);
    HAL_ADC_Start_DMA(&hadc2, reinterpret_cast<uint32_t *>(adc2Buffer), 4);
}

/**
 * Fast sampling data: Current
 */
void callbackHRTIM()
{
    for (int i = 0; i < 4; ++i)
    {
        CoilManager::coilActualCurrent[i] =
            coilCurrentFilterAlpha *
                (adc2CalibrationGain[i] * ((float)adc2Buffer[i] + (float)adc2Buffer[i + 4]) + adc2CalibrationBias[i]) +
            (1 - coilCurrentFilterAlpha) * CoilManager::coilActualCurrent[i];
    }
}

/**
 * Slow sampling data: Voltage, Temperature, etc..
 */
void callbackNormal() {}

}  // namespace Tasks::SampleTask
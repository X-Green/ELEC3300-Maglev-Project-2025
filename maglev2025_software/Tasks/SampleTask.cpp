//
// Created by eeasee on 3/12/25.
//

#include "SampleTask.hpp"

#include "adc.h"
#include "opamp.h"

namespace Tasks::SampleTask
{

void sampleTaskInit()
{
    HAL_Delay(100);
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
    HAL_ADCEx_Calibration_Start(&hadc5, ADC_SINGLE_ENDED);

    HAL_Delay(100);
    HAL_ADC_Start_DMA(&hadc2, reinterpret_cast<uint32_t *>(adc2Buffer), 4);
    HAL_OPAMP_Start(&hopamp3);  // VIN Sampling
    HAL_OPAMP_Start(&hopamp4);  // IIN Sampling with PGA G=32
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
void callbackNormal()
{
    adc1Value = HAL_ADC_GetValue(&hadc1);
    adc5Value = HAL_ADC_GetValue(&hadc5);
}

}  // namespace Tasks::SampleTask
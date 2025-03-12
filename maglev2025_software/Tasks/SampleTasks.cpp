//
// Created by eeasee on 3/12/25.
//

#include "SampleTasks.hpp"

#include "adc.h"

namespace Tasks::SampleTask
{

uint16_t adc2Buffer[4] = {};

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
    Tasks::CoilManager::coilStatus[0].actualI = adc2Buffer[0];
    Tasks::CoilManager::coilStatus[1].actualI = adc2Buffer[1];
    Tasks::CoilManager::coilStatus[2].actualI = adc2Buffer[2];
    Tasks::CoilManager::coilStatus[3].actualI = adc2Buffer[3];
}

/**
 * Slow sampling data: Voltage, Temperature, etc..
 */
void callbackNormal() {}

}  // namespace Tasks::SampleTask
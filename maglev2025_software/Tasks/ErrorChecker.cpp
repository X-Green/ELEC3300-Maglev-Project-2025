//
// Created by eeasee on 3/12/25.
//

#include "ErrorChecker.hpp"

#include "Buzzer.hpp"
#include "PositionControl.hpp"
#include "SampleTask.hpp"

void setSchmittTrigger(uint32_t &reg, uint32_t mask, bool conditionOff, bool conditionOn)
{
    if (reg & mask)
    {
        if (conditionOff)
        {
            reg &= ~mask;
        }
    }
    else
    {
        if (conditionOn)
        {
            reg |= mask;
        }
    }
}

void Tasks::ErrorChecker::updateErrorState()
{
    setSchmittTrigger(errorCode,
                      ERROR_CODE_MASK_LEV_POSITION,
                      (M_ABS(Tasks::PositionControl::magMeasurement[0]) < 0.03f ||
                       M_ABS(Tasks::PositionControl::magMeasurement[1]) < 0.03f),
                      (M_ABS(Tasks::PositionControl::magMeasurement[0]) > 0.1f ||
                       M_ABS(Tasks::PositionControl::magMeasurement[1]) > 0.1f));

    setSchmittTrigger(errorCode,
                      ERROR_CODE_MASK_LEV_EXISTENCE,
                      (Tasks::PositionControl::magMeasurement[2] > 0.01f),
                      (Tasks::PositionControl::magMeasurement[2] < -0.02f));

    setSchmittTrigger(
        errorCode, ERROR_CODE_MASK_NO_POWER, (SampleTask::vbusVoltage > 8.0f), (SampleTask::vbusVoltage < 7.0f));

    if (errorCode != 0)
    {
        if (CoilManager::enableOutput)
        {
            CoilManager::enableOutput = false;
            Drivers::Buzzer::play(200, 300);
        }
    }
    else
    {
        if (!CoilManager::enableOutput)
        {
            CoilManager::enableOutput = true;
            Drivers::Buzzer::play(1250, 200);
        }
    }
}

void Tasks::ErrorChecker::updateErrorStateFast() {}

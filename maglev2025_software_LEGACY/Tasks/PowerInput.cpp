//
// Created by eeasee on 3/12/25.
//

#include "PowerInput.hpp"

namespace Tasks::PowerInput
{

PowerInputState powerInputState = PowerInputState::NO_POWER_INPUT;

PowerInputState getPowerInputState() { return powerInputState; }
float getPowerInputVoltage()
{
    switch (powerInputState)
    {
    case PowerInputState::NO_POWER_INPUT:
        return 0.0;
    case PowerInputState::USB_5V_INPUT:
        return 5.0;
    case PowerInputState::PD_9V_INPUT:
        return 9.0;
    case PowerInputState::PD_12V_INPUT:
        return 12.0;
    case PowerInputState::PD_20V_INPUT:
        return 20.0;
    case PowerInputState::DC_12V_INPUT:
        return 12.0;
    default:
        __asm volatile("bkpt ");
        return 0.0;
    }
}

void initPowerInput() { powerInputState = PowerInputState::NO_POWER_INPUT; }

void updatePowerInputStatus() {}
void switchToAvailablePowerInput() {}
}  // namespace Tasks::PowerInput
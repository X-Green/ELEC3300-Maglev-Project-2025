//
// Created by eeasee on 3/12/25.
//

#pragma once

namespace Tasks
{

namespace PowerInput
{
enum class PowerInputState
{
    NO_POWER_INPUT,
    DC_12V_INPUT,
    USB_5V_INPUT,
    PD_9V_INPUT,
    PD_12V_INPUT,
    PD_20V_INPUT
};

PowerInputState getPowerInputState();

float getPowerInputVoltage();

/**
 * Prefer DC input, then PD high voltage to low voltage
 */
void initPowerInput();

/**
 * Check if sensed VPP is aligned with PowerInputState
 */
void updatePowerInputStatus();

/**
 * When power is disconnected, try to recover by UCPD
 */
void switchToAvailablePowerInput();

}  // namespace PowerInput

}  // namespace Tasks

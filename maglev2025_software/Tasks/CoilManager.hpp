//
// Created by eeasee on 3/12/25.
//

#pragma once

#include "SampleTask.hpp"
#include "hrtim.h"
#include "main.h"

namespace Tasks::CoilManager
{

struct CoilHAL
{
    uint32_t HRTIM_TIMERID;
    uint32_t HRTIM_TIMERINDEX;
};

const CoilHAL COIL_HAL[] = {
    {HRTIM_TIMERID_TIMER_A, HRTIM_TIMERINDEX_TIMER_A},
    {HRTIM_TIMERID_TIMER_B, HRTIM_TIMERINDEX_TIMER_B},
    {HRTIM_TIMERID_TIMER_C, HRTIM_TIMERINDEX_TIMER_C},
    {HRTIM_TIMERID_TIMER_D, HRTIM_TIMERINDEX_TIMER_D},
};

void initCoils();

/**
 * Input: coil control Data
 */
void updateCoils();

void updatePWM(int index, float outputValue);

}  // namespace Tasks::CoilManager

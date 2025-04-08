//
// Created by eeasee on 3/12/25.
//

#pragma once

#include "IncrementalPID.hpp"
#include "MathUtil.hpp"
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
inline float coilActualCurrent[4] = {0, 0, 0, 0};
inline float coilTargetCurrent[4] = {0, 0, 0, 0};
/**
 * Output Value: -1 < v < 1
 * Actual Duty Cycle: d = 0.5*v+0.5
 * (Actually it should be a curve with input from +-inf. But here balabala)
 */
inline float coilOutputValue[4] = {0, 0, 0, 0};
inline bool enableOutput        = false;

void initCoils();

/**
 * Input: coil control Data
 */
void updateCoilsFast();

void updateCoilsSlow();

void updatePWM(int index, float outputValue);

}  // namespace Tasks::CoilManager

//
// Created by eeasee on 3/12/25.
//

#pragma once
#include "CoilManager.hpp"
#include "main.h"

namespace Tasks::SampleTask
{

inline uint16_t adc2Buffer[8] = {};

/**
 * Using INA240A1, with gain = 20V/V; Using shunt of 10mOhm, with gain = 0.01V/A
 * 4096 -> 1.5+1.5 -> 0.075 -> 7.5A; 0 -> 1.5-1.5 -> -7.5A
 * y=ax+b with a=0.003662, b=-7.5
 */
inline float adc2CalibrationGain[4] = {0.003662f, 0.003662f, 0.003662f, 0.003662f};
inline float adc2CalibrationBias[4] = {-7.5f, -7.5f, -7.5f, -7.5f};
inline float coilCurrentFilterAlpha = 0.5f;

void sampleTaskInit();

void callbackHRTIM();

void callbackNormal();

}  // namespace Tasks::SampleTask
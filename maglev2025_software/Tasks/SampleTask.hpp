//
// Created by eeasee on 3/12/25.
//

#pragma once
#include "CoilManager.hpp"
#include "main.h"

namespace Tasks::SampleTask
{

inline uint16_t adc2Buffer[4] = {0, 0, 0, 0};

void sampleTaskInit();

void callbackHRTIM();

void callbackNormal();

}  // namespace Tasks::SampleTask
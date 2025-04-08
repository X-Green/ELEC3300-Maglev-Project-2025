//
// Created by eeasee on 3/12/25.
//

#pragma once
#include "CoilManager.hpp"
#include "main.h"

namespace Tasks::SampleTask
{

extern uint16_t adc2Buffer[4];

void sampleTaskInit();

void callbackHRTIM();

void callbackNormal();

}  // namespace Tasks::SampleTask
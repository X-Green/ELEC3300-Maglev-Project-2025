//
// Created by eeasee on 3/12/25.
//

#pragma once

#include "main.h"

namespace Tasks::CoilManager
{

struct CoilStatus
{
    float actualI;
    uint16_t temperature;

    CoilStatus() : actualI(0), temperature(0) {}
};

struct CoilManager
{
    bool enableOutput;
    float targetI;
    float dutyCycle;

    CoilManager() : enableOutput(false), targetI(0), dutyCycle(0) {}
};

void initCoils();

void getCoilSensedStatusStruct();
void getCoilControlStatusStruct();

void updateCoils();

}  // namespace Tasks::CoilManager

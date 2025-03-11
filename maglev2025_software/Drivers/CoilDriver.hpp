//
// Created by xiaol on 2025/3/11.
//

#pragma once

namespace Drivers
{

class CoilState
{
   public:
    CoilState() : targetI(0.0f), actualI(0.0f), enableOutput(false), dutyCycle(0.0f) {}

    float targetI;
    float actualI;
    bool enableOutput;
    float dutyCycle;
};

};  // namespace Drivers

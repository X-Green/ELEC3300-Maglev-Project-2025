#pragma once

#include "main.h"

namespace Tasks::LEDTasks
{

enum LED_PATTERN
{
    LED_PATTERN_OFF,
    LED_PATTERN_WARNING,
    LED_PATTERN_RAINBOW,
    LED_PATTERN_RAINBOW_BREATHING,
    LED_PATTERN_RAINBOW_CYCLE,
    LED_PATTERN_RAINBOW_BREATHING_CYCLE,

};

inline LED_PATTERN ledPattern = LED_PATTERN_WARNING;

void kHzTrigger();
}  // namespace Tasks::LEDTasks
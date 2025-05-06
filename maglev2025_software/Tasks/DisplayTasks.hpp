

#pragma once

#include "main.h"

enum DisplayPage
{
    DisplayPageMain = 0,
    DisplayPageWaveform = 1,
    DisplayPageLED = 2,
    DisplayPageBuzzer = 3,
    DisplayPageSensor = 4
};

namespace Drivers::DisplayTasks{

    void DisplayInit(void);
    void DisplayUpdate( uint8_t page);
    void ADCValueToStr(uint16_t adcValue, char* buf, int bufSize);


}


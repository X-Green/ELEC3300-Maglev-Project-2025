#include "DisplayTasks.hpp"
#include "Oled.hpp"
#include "u8g2/u8g2.h"
#include "PositionControl.hpp"
#include "LEDTasks.hpp"
#include "MusicTasks.hpp"
#include "Buzzer.hpp"
#include "PowerInput.hpp"
#include "SampleTask.hpp"

namespace Drivers::DisplayTasks
{
    u8g2_t u8g2; // a structure which will contain all the data for one display
    uint16_t Sample_Current = 0;
    uint16_t Sample_Voltage = 0;
    volatile uint16_t LED_SubPage = 2;
    uint16_t Buzzer_SubPage = 0;
    static uint8_t last_button = GPIO_PIN_SET;
    enum Buzzer_pattern{
        melody1 = 0,
        melody2 = 1,
    };
    //helper function
    void ADCValueToStr(uint16_t adcValue, char* buf, int bufSize)
{
    int i = 0;
    uint16_t value = adcValue;
    char temp[5];
    int j = 0;

    if (value == 0) {
        buf[i++] = '0';
    } else {
        while (value > 0 && j < 5) {
            temp[j++] = (value % 10) + '0';
            value /= 10;
        }
        while (j > 0 && i < bufSize - 1) {
            buf[i++] = temp[--j];
        }
    }
    buf[i] = '\0';
}

    void DisplayInit(void){
        u8g2Init(&u8g2);
        u8g2_ClearBuffer(&u8g2);

        //set timer for en  coder
        htim8.Instance->ARR = 5;

    }
    void DisplayUpdate(uint8_t page){
        page=htim8.Instance->CNT;
        u8g2_ClearBuffer(&u8g2);
        static float displayM0 = 0;
        static float MagnetWaveform[128] = {0};
        static uint8_t DisplayWave[128] = {0};
        displayM0 = Tasks::PositionControl::magMeasurement[0] * 0.4f + displayM0 * 0.6f;
        updateMagnetWaveform(displayM0, MagnetWaveform);
        DataToWave(MagnetWaveform, 128, DisplayWave, 0.0f, -0.02f);

        //get sample current value and filter it
        Sample_Current = Tasks::SampleTask::adc5Value*0.4f + Sample_Current * 0.6f;
        Sample_Voltage = Tasks::SampleTask::vbusVoltage*0.9f + Sample_Voltage * 0.1f;
        switch (page)
        {
        case DisplayPageMain:
            u8g2_SetFont(&u8g2, u8g2_font_DigitalDiscoThin_tf);
            u8g2_DrawStr(&u8g2, 0, 32, "ELEC3300Project");
            u8g2_SendBuffer(&u8g2);            
            break;
        case DisplayPageWaveform:
            // x axis
            u8g2_DrawLine(&u8g2, 0, 63, 127, 63);    
            // y axis
            u8g2_DrawLine(&u8g2, 0, 32, 0, 63);  
            u8g2_DrawStr(&u8g2, 2, 20, "Magnetic Value");
            u8g2_DrawStr(&u8g2, 100, 62, "Timeline");
            DrawWave(0, 40, DisplayWave, 128, &u8g2);
            u8g2_SendBuffer(&u8g2);            
            break;
        case DisplayPageLED:{
            if(last_button == GPIO_PIN_SET && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET)
            {

                LED_SubPage++;
                if(LED_SubPage > 5) LED_SubPage = 2;
                Tasks::LEDTasks::ledPattern = (Tasks::LEDTasks::LED_PATTERN)LED_SubPage;

            }
            last_button = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);

            u8g2_DrawFrame(&u8g2, 0, 0, 128, 64);
            u8g2_DrawStr(&u8g2, 30, 10, "LED Pattern:");
            u8g2_SetFont(&u8g2, u8g2_font_DigitalDiscoThin_tf);
            char* DisplayLEDMode = "Unknown LED Pattern";
            switch (Tasks::LEDTasks::ledPattern)
            {
            case Tasks::LEDTasks::LED_PATTERN_OFF:
                DisplayLEDMode = "OFF";
                break;
            case Tasks::LEDTasks::LED_PATTERN_WARNING:
                DisplayLEDMode = "WARNING";
                break;
            case Tasks::LEDTasks::LED_PATTERN_RAINBOW:
                DisplayLEDMode = "RAINBOW";
                break;
            case Tasks::LEDTasks::LED_PATTERN_RAINBOW_BREATHING:
                DisplayLEDMode = "RAINBOW BREATHING";
                break;
            case Tasks::LEDTasks::LED_PATTERN_RAINBOW_CYCLE:
                DisplayLEDMode = "RAINBOW CYCLE";
                break;
            case Tasks::LEDTasks::LED_PATTERN_RAINBOW_BREATHING_CYCLE:
                DisplayLEDMode = "RAINBOW BREATHING CYCLE";
                break;
            default:
                DisplayLEDMode = "Unknown LED Pattern";
                break;
            }
            u8g2_DrawStr(&u8g2, 20, 40, DisplayLEDMode);
            u8g2_SendBuffer(&u8g2);            
            break;
        }
        case DisplayPageBuzzer:
        u8g2_DrawFrame(&u8g2, 0, 0, 128, 64);
        u8g2_DrawStr(&u8g2, 20, 20, "Playing songs:");
        if(last_button == GPIO_PIN_SET && HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10) == GPIO_PIN_RESET)
        {
            Buzzer_SubPage++;
        if(Buzzer_SubPage > 2) Buzzer_SubPage = 0;

        static uint16_t lastSubPage = 0;
        if (lastSubPage != Buzzer_SubPage)
        {
            lastSubPage = Buzzer_SubPage;
            switch (Buzzer_SubPage)
            {
            case 0:
                u8g2_DrawStr(&u8g2, 20, 40, "Song 1: Melody 1");
                Tasks::Music::stopMelody();
                Tasks::Music::startMelody(Tasks::Music::melody1, sizeof(Tasks::Music::melody1) / sizeof(Tasks::Music::melody1[0]));
                /* code */
                break;
            case 1:
                u8g2_DrawStr(&u8g2, 20, 40, "Song 2: Melody 2");
                Tasks::Music::stopMelody();
                Tasks::Music::startMelody(Tasks::Music::melody2, sizeof(Tasks::Music::melody2) / sizeof(Tasks::Music::melody1[0]));
                /* code */
                break;
            default:
                break;
            }
        }


        }
        last_button = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10);
        // if(Buzzer_SubPage == 0)
        // {
        //     Tasks::Music::startMelody(Tasks::Music::melody1, sizeof(Tasks::Music::melody1) / sizeof(Tasks::Music::melody1[0]));
        //     u8g2_DrawStr(&u8g2, 20, 40, "Song 1: Melody 1");
        // }
        // else if(Buzzer_SubPage == 1)
        // {
        //     Tasks::Music::startMelody(Tasks::Music::melody2, sizeof(Tasks::Music::melody2) / sizeof(Tasks::Music::melody1[0]));
        //     u8g2_DrawStr(&u8g2, 20, 40, "Song 2: Melody 2");
        // }
        // else{

        // }
    

            u8g2_SendBuffer(&u8g2);
            break;
        case DisplayPageSensor:
            u8g2_DrawFrame(&u8g2, 0, 0, 128, 64);
            u8g2_SetFont(&u8g2, u8g2_font_DigitalDiscoThin_tf);
            u8g2_DrawStr(&u8g2, 0, 20, "Sensor Data:");
            char Curr_buf[8];
            char Volt_buf[8];
            ADCValueToStr(Tasks::SampleTask::adc5Value, Curr_buf, sizeof(Curr_buf));
            ADCValueToStr(Tasks::SampleTask::vbusVoltage, Volt_buf, sizeof(Volt_buf));
            u8g2_DrawStr(&u8g2, 30, 35, "Current:");
            u8g2_DrawStr(&u8g2, 100, 35, Curr_buf);
            u8g2_DrawStr(&u8g2, 30, 56, "Voltage:");
            u8g2_DrawStr(&u8g2, 100, 56, Volt_buf);
            u8g2_SendBuffer(&u8g2);
            break;
        default:
            break;
        }
    }

}
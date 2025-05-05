#include "Buzzer.hpp"
#include "Oled.hpp"
#include "CoilManager.hpp"
#include "CommandInput.hpp"
#include "ErrorChecker.hpp"
#include "PositionControl.hpp"
#include "SampleTask.hpp"
#include "TMAG5170.hpp"
#include "WS2812.hpp"
#include "adc.h"
#include "dma.h"
#include "hrtim.h"
#include "opamp.h"
#include "spi.h"
#include "stm32g4xx_it.h"
#include "usart.h"
// extern "C" void DMA_SPI1_RX_CompleteCallback(DMA_HandleTypeDef *hdma);
// extern DMA_HandleTypeDef hdma_spi1_rx;

namespace MainTask
{

bool initialized               = false;
volatile uint16_t adcBuffer[4] = {0, 0, 0, 0};

void init()
{
    Drivers::Sensors::TMAG5170::TMAG5170_init();
    Drivers::Sensors::TMAG5170::setDeviceConfig();
    Drivers::Sensors::TMAG5170::setSensorConfig();
    //    Drivers::Sensors::TMAG5170::setMagGainConfigInDecimal(0x01, 1.0);
    //    Drivers::Sensors::TMAG5170::setMagGainConfigInDecimal(0x02, 1.0);
    //    Drivers::Sensors::TMAG5170::setMagGainConfigInDecimal(0x03, 1.0);

    Drivers::Sensors::TMAG5170::alertIndicatesConversionEnable();
    Drivers::Sensors::TMAG5170::initDMATxBuffers();

    Tasks::SampleTask::sampleTaskInit();

    Tasks::CommandInput::initCommandInput();

    HAL_Delay(100);

    HAL_TIM_Base_Start_IT(&htim17);

    __HAL_HRTIM_MASTER_ENABLE_IT(&hhrtim1, HRTIM_MASTER_IT_MREP);  // enable master repetition interrupt
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_MASTER);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_A);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_B);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    Tasks::CoilManager::setOutputEnable();

    HAL_Delay(100);

    Drivers::WS2812::init();
    Drivers::WS2812::setColor(0, Drivers::WS2812::RGB_RED);
    Drivers::WS2812::setColor(1, Drivers::WS2812::RGB_ORANGE);
    Drivers::WS2812::setColor(2, Drivers::WS2812::RGB_YELLOW);
    Drivers::WS2812::setColor(3, Drivers::WS2812::RGB_GREEN);
    Drivers::WS2812::setColor(4, Drivers::WS2812::RGB_CYAN);
    Drivers::WS2812::setColor(5, Drivers::WS2812::RGB_BLUE);
    Drivers::WS2812::setColor(6, Drivers::WS2812::RGB_PURPLE);
    Drivers::WS2812::setColor(7, Drivers::WS2812::RGB_RED);
    Drivers::WS2812::setColor(8, Drivers::WS2812::RGB_ORANGE);
    Drivers::WS2812::setColor(9, Drivers::WS2812::RGB_YELLOW);
    Drivers::WS2812::setColor(10, Drivers::WS2812::RGB_GREEN);
    Drivers::WS2812::setColor(11, Drivers::WS2812::RGB_CYAN);
    Drivers::WS2812::setColor(12, Drivers::WS2812::RGB_BLUE);
    Drivers::WS2812::setColor(13, Drivers::WS2812::RGB_PURPLE);

    Drivers::Buzzer::init();

    initialized = true;

    Drivers::Buzzer::play(1000, 400);
    HAL_Delay(1000);
    Drivers::Buzzer::play(2000, 100);
    HAL_Delay(200);
    Drivers::Buzzer::play(2000, 100);
    HAL_Delay(200);
    Drivers::Buzzer::play(2000, 100);

    Drivers::Oled::OLED_Init();
    Drivers::Oled::OLED_On();
    // Drivers::Oled::OLED_ShowNum(48,4,6,1,16, 0);
    Drivers::Oled::OLED_Clear();
    HAL_Delay(100);
    Drivers::Oled::OLED_DrawBMP(0,0,128, 8 ,epd_bitmap_GZS,0);//正相显示图片BMP1
    HAL_Delay(1000);
    
}

volatile float testOutput = 0.0f;

float magnetic_measurements[3] = {0};
void loop()
{
    //    Drivers::Sensors::TMAG5170::getMagMeasurementsNrml(const_cast<float *>(magnetic_measurements));
    // Drivers::Sensors::TMAG5170::startDMASequentialNormalReadXYZ();

    //    for (int i = 0; i < 4; i++)
    //    {
    //        Tasks::CoilManager::updatePWM(i, testOutput);
    //    }

    HAL_Delay(500);
}

void trigger1KHz()
{
    Tasks::SampleTask::callbackNormal();
    Tasks::ErrorChecker::updateErrorState();
}

void triggerOneHz()
{
    static volatile int oneHzCounter = 0;
    oneHzCounter += 1;
    Tasks::PositionControl::counterLogs.dataReadyCounter = 0;
}

}  // namespace MainTask

extern "C"
{
    /**
     * Called at main.c
     */
    [[noreturn]] void systemStart()
    {
        MainTask::init();
        for (;;)
            MainTask::loop();
    }

    /**
     * @brief HRTIM interrupt @ 85kHz
     */
    void HRTIM1_Master_IRQHandler(void)
    {
        __HAL_HRTIM_MASTER_CLEAR_IT(&hhrtim1, HRTIM_MASTER_IT_MREP);
        Tasks::SampleTask::callbackHRTIM();
        Tasks::CoilManager::updateCoilsFast();
    }

    /**
     * @brief EXTI from MAG_CS, around 3kHz
     */
    void EXTI4_IRQHandler(void)
    {
        __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_4);
        if (!MainTask::initialized)
            return;
        Tasks::PositionControl::onDataReady();
    }

    void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
    {
        if (!MainTask::initialized)
            return;

        if (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY)
        {
            __asm("bkpt");
        }

        Drivers::Sensors::TMAG5170::continueDMASequentialNormalReadXYZ();
    }

    void TIM1_TRG_COM_TIM17_IRQHandler(void)
    {
        uint32_t itsource = htim17.Instance->DIER;
        uint32_t itflag   = htim17.Instance->SR;
        if ((itflag & (TIM_FLAG_UPDATE)) == (TIM_FLAG_UPDATE))
        {
            if ((itsource & (TIM_IT_UPDATE)) == (TIM_IT_UPDATE))
            {
                __HAL_TIM_CLEAR_FLAG(&htim17, TIM_FLAG_UPDATE);
                MainTask::trigger1KHz();
                static uint32_t timerCounter1KHz = 0;
                timerCounter1KHz++;
                if (timerCounter1KHz > 1000)
                {
                    timerCounter1KHz = 0;
                    MainTask::triggerOneHz();
                }
            }
        }
    }

    void TIM1_UP_TIM16_IRQHandler(void) { Drivers::Buzzer::onTIM1UpdateCallback(); }

    void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
    {
        if (huart == &huart1)
        {
            Tasks::CommandInput::onU1RXCallback();
        }
        else
        {
            __asm("bkpt");
        }
    }
}

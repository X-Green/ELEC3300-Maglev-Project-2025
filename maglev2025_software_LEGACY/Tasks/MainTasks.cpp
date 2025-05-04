#include "CoilManager.hpp"
#include "PositionControl.hpp"
#include "TMAG5170.hpp"
#include "adc.h"
#include "dma.h"
#include "hrtim.h"
#include "SampleTask.hpp"
#include "spi.h"
#include "stm32g4xx_it.h"

// extern "C" void DMA_SPI1_RX_CompleteCallback(DMA_HandleTypeDef *hdma);
// extern DMA_HandleTypeDef hdma_spi1_rx;

namespace MainTask
{

bool initialized               = false;
volatile uint16_t adcBuffer[4] = {0, 0, 0, 0};

void init()
{
    Drivers::Sensors::TMAG5170::TMAG5170_init();
    Drivers::Sensors::TMAG5170::enableMagChannels(0x07);
    Drivers::Sensors::TMAG5170::setMagGainConfigInDecimal(0x01, 1.0);
    Drivers::Sensors::TMAG5170::setMagGainConfigInDecimal(0x02, 1.0);
    Drivers::Sensors::TMAG5170::setMagGainConfigInDecimal(0x03, 1.0);
    Drivers::Sensors::TMAG5170::enterActiveMeasureMode();
    Drivers::Sensors::TMAG5170::alertIndicatesConversionEnable();
    Drivers::Sensors::TMAG5170::initDMATxBuffers();

    Tasks::SampleTask::sampleTaskInit();

    HAL_Delay(100);
    //    auto s = HAL_DMA_RegisterCallback(
    //        &hdma_spi1_rx, HAL_DMA_XFER_CPLT_CB_ID, DMA_SPI1_RX_CompleteCallback);
    //    if (s != HAL_OK)
    //    {
    //        __asm("bkpt");
    //    }
    HAL_Delay(100);

    HAL_TIM_Base_Start_IT(&htim17);

    __HAL_HRTIM_MASTER_ENABLE_IT(&hhrtim1, HRTIM_MASTER_IT_MREP);  // enable master repetition interrupt
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_MASTER);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_A);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_B);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_C);
    HAL_HRTIM_WaveformCountStart(&hhrtim1, HRTIM_TIMERID_TIMER_D);
    Tasks::CoilManager::setOutputEnable();

    HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
    HAL_Delay(100);

    initialized = true;

    HAL_Delay(1000);
}

volatile float testOutput               = 0.0f;
void loop()
{
    // Drivers::Sensors::TMAG5170::getMagMeasurementsNrml(const_cast<float *>(magnetic_measurements));
    // Drivers::Sensors::TMAG5170::startDMASequentialNormalReadXYZ();

//    for (int i = 0; i < 4; i++)
//    {
//        Tasks::CoilManager::updatePWM(i, testOutput);
//    }

    HAL_Delay(500);
}

void trigger1KHz() {}

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
    void EXTI9_5_IRQHandler(void)
    {
        __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_6);
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
        __HAL_TIM_CLEAR_IT(&htim17, TIM_IT_UPDATE);
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

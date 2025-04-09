#include "PositionControl.hpp"
#include "TMAG5170.hpp"
#include "dma.h"
#include "hrtim.h"
#include "spi.h"
#include "stm32g4xx_it.h"

extern "C" void DMA_SPI1_RX_CompleteCallback(DMA_HandleTypeDef *hdma);
extern DMA_HandleTypeDef hdma_spi1_rx;

namespace MainTask
{

void init()
{
    // Add your code here
    Drivers::Sensors::TMAG5170::TMAG5170_init();
    Drivers::Sensors::TMAG5170::enableMagChannels(0x07);
    Drivers::Sensors::TMAG5170::setMagGainConfigInDecimal(0x01, 1.0);
    Drivers::Sensors::TMAG5170::setMagGainConfigInDecimal(0x02, 1.0);
    Drivers::Sensors::TMAG5170::setMagGainConfigInDecimal(0x03, 1.0);
    Drivers::Sensors::TMAG5170::enterActiveMeasureMode();
    //    HAL_Delay(1);
    //    Drivers::Sensors::TMAG5170::setConvAvg();
    Drivers::Sensors::TMAG5170::alertIndicatesConversionEnable();

    HAL_DMA_RegisterCallback(
        &hdma_spi1_rx, HAL_DMA_XFER_CPLT_CB_ID, DMA_SPI1_RX_CompleteCallback);

    HAL_Delay(1000);

    HAL_TIM_Base_Start_IT(&htim17);
}

void loop()
{
    static float magnetic_measurements[3] = {0};

    Drivers::Sensors::TMAG5170::getMagMeasurementsNrml(magnetic_measurements);

    static volatile uint16_t cfg_device_tmag5170 = 0;
    cfg_device_tmag5170 =
        Drivers::Sensors::TMAG5170::normalReadRegister(DEVICE_CONFIG_ADDRESS);
    HAL_Delay(100);
}

void trigger1KHz() {}

void triggerOneHz()
{
    static volatile int oneHzCounter = 0;
    oneHzCounter += 1;

    Tasks::PositionControl::counterLogs.dataReadyCounter = 0;

}  // namespace MainTask

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
    }

    /**
     * @brief EXTI from MAG_CS, around 3kHz
     */
    void EXTI9_5_IRQHandler(void)
    {
        __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_6);
        Tasks::PositionControl::onDataReady();
    }

    /**
     * Need to be registered
     */
    void DMA_SPI1_RX_CompleteCallback(DMA_HandleTypeDef *hdma)
    {
        Tasks::PositionControl::updatePosition();
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

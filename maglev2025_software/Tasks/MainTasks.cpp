#include "TMAG5170.hpp"
#include "hrtim.h"

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
    Drivers::Sensors::TMAG5170::alertIndicatesConversionEnable();
    HAL_Delay(1000);
}

void loop()
{
    // 创建数组存储三轴磁场数据（顺序：XYZ）
    static float magnetic_measurements[3] = {0};
    static uint8_t measurement[3]         = {0};

    Drivers::Sensors::TMAG5170::getMagMeasurementsNrml(magnetic_measurements);
    // HAL_SPI_Receive(&hspi1, (uint8_t *)measurement, 3, 1000);
    // magnetic_measurements[0]  X
    // magnetic_measurements[1]  Y
    // magnetic_measurements[2]  Z

    // 添加适当的延时
    HAL_Delay(100);
}

extern "C"
{
    /**
     * Called at main.c
     */
    [[noreturn]] void systemStart()
    {
        init();
        for (;;)
            loop();
    }

    /**
     * @brief HRTIM interrupt @ 85kHz
     */
    void HRTIM1_Master_IRQHandler(void) { __HAL_HRTIM_MASTER_CLEAR_IT(&hhrtim1, HRTIM_MASTER_IT_MREP); }

    /**
     * @brief EXTI from MAG_CS, around 3kHz
     */
    void EXTI9_5_IRQHandler(void) { __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_6); }

    /**
     * @brief TIM16 interrupt @ 1kHz
     */
    void TIM1_UP_TIM16_IRQHandler(void) { __HAL_TIM_CLEAR_FLAG(&htim16, TIM_FLAG_UPDATE); }
}

}  // namespace MainTask
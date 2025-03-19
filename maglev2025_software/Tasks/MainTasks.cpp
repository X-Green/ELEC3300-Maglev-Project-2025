#include "TMAG5170.hpp"


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

}

void loop()
{
    // 创建数组存储三轴磁场数据（顺序：XYZ）
    static float magnetic_measurements[3] = {0};
    
    // 读取磁场数据
    Drivers::Sensors::TMAG5170::getMagMeasurementsNrml(magnetic_measurements);
    
    // magnetic_measurements[0] 是 X 轴数据 (mT)
    // magnetic_measurements[1] 是 Y 轴数据 (mT)
    // magnetic_measurements[2] 是 Z 轴数据 (mT)

    // 添加适当的延时
    HAL_Delay(100);
    
}
extern "C"
{
    void systemStart()
    {
        init();
        for (;;)
            loop();
    }
}

}  // namespace MainTask
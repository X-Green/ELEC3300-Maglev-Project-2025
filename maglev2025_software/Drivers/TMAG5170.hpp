//
// Created by eeasee on 2025/3/11.
//

#pragma once

#include "MathUtil.hpp"
#include "TMAG5170_MACRO.hpp"
#include "main.h"
#include "spi.h"
#include "tim.h"

namespace Drivers::Sensors::TMAG5170
{

#define DISABLE_CRC

uint32_t isqrt32(uint32_t h);

float resultRegisterTomT(int16_t register_bits, uint16_t range);
float angleRegisterToDeg(uint16_t register_bits);

uint8_t calculateCRC(uint8_t &data);
uint8_t verifyCRC(uint8_t &data);

void enterDeepSleepMode();
void exitDeepSleepMode();
void resetDevice();

void TMAG5170_init();

void writeToRegister(uint8_t address, uint16_t data_to_write);
void sendAndReceiveFrame(uint8_t dataTx[], uint8_t dataRx[]);

void normalRead(uint16_t output[], uint8_t address, uint8_t cmd_bits);
uint16_t normalReadRegister(uint8_t address);
void setSensorConfig();

void spiTriggersConversion();
void alertTriggersConversion();
void setDeviceConfig();
void setConvAvg();
void alertIndicatesConversionEnable();

void getMagMeasurementsNrml(float meas_arr[]);

uint8_t getVersion();
uint16_t getXrange();
uint16_t getYrange();
uint16_t getZrange();

void delay_us(uint32_t us);

void setMagGainConfigIn11Bit(uint8_t axis, uint16_t gain_bits);
void setMagGainConfigInDecimal(uint8_t axis, float gain_value);

void initDMATxBuffers();
void startDMASequentialNormalReadXYZ();
void continueDMASequentialNormalReadXYZ();

}  // namespace Drivers::Sensors::TMAG5170

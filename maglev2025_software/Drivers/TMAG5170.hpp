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

void setDeviceConfig();
void alertIndicatesConversionEnable();

void getMagMeasurementsNrml(float meas_arr[]);

uint8_t getVersion();

void delay_us(uint32_t us);

void initDMATxBuffers();
void startDMASequentialNormalReadXYZ();
void continueDMASequentialNormalReadXYZ();

}  // namespace Drivers::Sensors::TMAG5170

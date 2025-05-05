//
// Created by eeasee on 2025/3/11.
//

#include "TMAG5170.hpp"

#include "PositionControl.hpp"

namespace Drivers::Sensors::TMAG5170
{
//****************************************************************************
//****************************************************************************
//
// Helper Functions
//
//****************************************************************************
//****************************************************************************

//****************************************************************************
//! 32-bit Square Root Function
//!
//! Takes the square of the input integer h and returns a 32-bit value that can
//! be converted to the floating point value by dividing it by 65536.
//****************************************************************************
uint32_t isqrt32(uint32_t h)
{
    uint32_t x;
    uint32_t y;
    int i;
    x = y = 0;
    for (i = 0; i < 32; i++)
    {
        x = (x << 1) | 1;
        if (y < x)
            x -= 2;
        else
            y -= x;
        x++;
        y <<= 1;
        if ((h & 0x80000000))
            y |= 1;
        h <<= 1;
        y <<= 1;
        if ((h & 0x80000000))
            y |= 1;
        h <<= 1;
    }
    return x;
}

//****************************************************************************
//! Convert Axis Measurement Result Register to mT value
//!
//! Takes in the register of one of the magnetic axis results and the range for
//! that specific axis and calculates the mT value the register represents.
//****************************************************************************
float resultRegisterTomT(int16_t register_bits, uint16_t range)
{
    return (((float)register_bits) / 32768) * range;
}

//****************************************************************************
//! This variable tracks the state of the SYSTEM_CONFIG register (0x02)
//! Since any special read state cannot read out registers when called,
//! keeping track of the SYSTEM_CONFIG register is needed in order to
//! switch to and from normal read mode without altering other bits besides
//! DATA_TYPE's (0x028-6).
//!
//! The macro 'DATA_TYPE_RESULTS' defined in the header can be used to access
//! the DATA_TYPE bits contained in the stored register.
//!
//! This variable also must be initialized using a function like the one
//! below before the rest of any code implementation using the given example
//! functions can work.
//****************************************************************************
static uint16_t SYSTEM_CONFIG_stored = 0;
#define DATA_TYPE_RESULTS ((SYSTEM_CONFIG_stored & ~(SYSTEM_CONFIG_DATA_TYPE_MASK)) >> 6)

//****************************************************************************
//! Convert Angle Result Register to degree value
//!
//! Takes in the bits of the angle result register and converts it to a
//! degree value.
//****************************************************************************
float angleRegisterToDeg(uint16_t register_bits) { return ((float)register_bits / 16); }

//****************************************************************************
//! Calculate CRC for SPI data frame
//!
//! Takes in an array containing a SPI data frame (MSB to LSB) with the CRC bits
//! all set to ZERO and calculates and returns the CRC for that data frame.
//****************************************************************************
uint8_t calculateCRC(uint8_t data[])
{
    int i       = 0;
    uint8_t crc = 0x00;
    uint32_t n;

    // Build TX and RX byte array
    uint8_t d[32] = {0};

    n = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | (data[3]);

    while (n > 0)
    {
        d[i] = n & 1;
        n    = n >> 1;
        i++;
    }

    crc |= d[30] ^ d[26] ^ d[25] ^ d[24] ^ d[23] ^ d[21] ^ d[19] ^ d[18] ^ d[15] ^ d[11] ^
           d[10] ^ d[9] ^ d[8] ^ d[6] ^ d[4] ^ d[3] ^ d[0] ^ 1;
    crc |= (d[31] ^ d[30] ^ d[27] ^ d[23] ^ d[22] ^ d[21] ^ d[20] ^ d[18] ^ d[16] ^
            d[15] ^ d[12] ^ d[8] ^ d[7] ^ d[6] ^ d[5] ^ d[3] ^ d[1] ^ d[0] ^ 1 ^ 1)
           << 1;
    crc |= (d[31] ^ d[28] ^ d[24] ^ d[23] ^ d[22] ^ d[21] ^ d[19] ^ d[17] ^ d[16] ^
            d[13] ^ d[9] ^ d[8] ^ d[7] ^ d[6] ^ d[4] ^ d[2] ^ d[1] ^ 1 ^ 1)
           << 2;
    crc |= (d[29] ^ d[25] ^ d[24] ^ d[23] ^ d[22] ^ d[20] ^ d[18] ^ d[17] ^ d[14] ^
            d[10] ^ d[9] ^ d[8] ^ d[7] ^ d[5] ^ d[3] ^ d[2] ^ 1)
           << 3;

    return crc;
}

//****************************************************************************
//! Verify CRC in SPI data frame
//!
//! Takes in an array containing a SPI data frame (MSB to LSB) and checks if the
//! CRC bits (according to their locations for the TMAG5170) are correct according
//! to the CRC calculation algorithm.
//****************************************************************************
uint8_t verifyCRC(uint8_t data[])
{
    uint8_t crc_received = data[3] & 0x0F;
    data[3] &=
        ~(0x0F);  // the CRC bits of the data must be 0000b to calculate its CRC correctly
    uint8_t crc_calc = calculateCRC(data);
    data[3] |= crc_received;  // the previously removed CRC bits are reinserted

    return crc_received == crc_calc;
}

//****************************************************************************
//! Enter Deep Sleep Mode
//!
//! Make sure to use the exitDeepSleepMode function to properly exit Deep Sleep Mode.
//! Deep Sleep Mode can be alternately exited with a short pulse on the CS pin.
//!
//! WILL WORK IN SPECIAL READ MODE, DEEP SLEEP MODE RESETS DEVICE TO FACTORY SETTINGS
//! (EXITS SPECIAL READ MODE)
//****************************************************************************
void enterDeepSleepMode()
{
    // Send Write command, Deep Sleep resets device to factory settings so
    // an initial register read is not needed (DEVICE_CONFIG default is 0x0000)
    writeToRegister(DEVICE_CONFIG_ADDRESS, DEVICE_CONFIG_OPERATING_MODE_DeepSleepMode);
    SYSTEM_CONFIG_stored = SYSTEM_CONFIG_DEFAULT;
    delay_us(100);
}

//****************************************************************************
//! Exit Deep Sleep Mode
//! (Use this function instead of a different operation mode change function to
//! exit Deep Sleep mode properly)
//!
//! Exits Deep Sleep Mode by pulsing LOW on the CS pin and waiting for the chip
//! to start up. (t_start_deep_sleep)
//!
//! DOES NOT WORK IN SPECIAL READ MODE [DATA_TYPE field at 0x028-6 does not equal 000b]
//****************************************************************************
void exitDeepSleepMode()
{
    // A LOW pulse is needed on CS to exit Deep Sleep Mode (enters Configuration Mode)
    HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_RESET);
    delay_us(2);  // cs pulse
    HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_SET);
    delay_us(500);  // max expected delay as given by t_start_deep_sleep (datasheet pg. 10)
    // device reset
}

//****************************************************************************
//! Reset Device to Factory Settings
//!
//! This function uses the DeepSleep functions to reset the device's registers back to the
//! default settings outlined in the datasheet. This function also resets the
//! SYSTEM_CONFIG_stored variable to the default value in the enterDeepSleepMode function.
//****************************************************************************
void resetDevice()
{
    enterDeepSleepMode();  // Deep Sleep Mode resets the device to its default settings
    exitDeepSleepMode();
}

void TMAG5170_init()
{
    HAL_Delay(50);
    HAL_TIM_Base_Start(&htim16);
    __HAL_SPI_ENABLE(&hspi1);
    HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_SET);

    // resetDevice();

    // disable crc in the initialization
    uint8_t dataTx[4] = {0x0F, 0x00, 0x04, 0x07};
    uint8_t dataRx[4] = {0};

    sendAndReceiveFrame(dataTx, dataRx);
    // sendFrame(dataTx);

    // transmit the data,but should be ok if not received?
    //  spiSendReceiveArrays(dataTx, dataRx, TMAG5170_FRAME_NUM_BYTES);

    // Check if SYSTEM_CONFIG was written to and update SYSTEM_CONFIG_stored if so
    if (dataTx[0] == SYSTEM_CONFIG_ADDRESS)
        SYSTEM_CONFIG_stored = (dataTx[1] << 8) | dataTx[2];
    SYSTEM_CONFIG_stored &= ~(0xC818);  // Reserved bits cannot be 1, this ensures the
    // stored variable doesn't have them changed
    writeToRegister(SYSTEM_CONFIG_ADDRESS, SYSTEM_CONFIG_DATA_TYPE_Default32bitdata);

    HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_RESET);
}

//****************************************************************************
//! writing to the register to send cmd
//!
//****************************************************************************
void writeToRegister(uint8_t address, uint16_t data_to_write)
{
    // Check that the input address is in range
    assert_param(address < NUM_REGISTERS);

    // Build TX and RX byte arrays
    uint8_t dataTx[4] = {0};
    uint8_t dataRx[4] = {0};

    // MSB is 0 for WRITE command
    dataTx[0] = (address);
    dataTx[1] = (data_to_write >> 8);
    dataTx[2] = (data_to_write);
    dataTx[3] = 0x00;
    // sendFrame(dataTx);
    sendAndReceiveFrame(dataTx, dataRx);
}

// void sendFrame( uint8_t dataTx[] ){
//     HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_RESET);
//     HAL_SPI_Transmit(&hspi1, dataTx, 4, HAL_MAX_DELAY);
//     HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_SET);
// }
/*
void sendAndReceiveFrame(uint8_t dataTx[], uint8_t dataRx[])
{
#ifdef DISABLE_CRC

    // Require that dataTx and dataRx are not NULL pointers
    assert(dataTx && dataRx);

    HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_RESET);
    delay_us(1);  // cs pulse

    // TAMG5170_FRAME_NUM_BYTES = 4 is default
        static volatile HAL_StatusTypeDef state = HAL_OK;
    state = HAL_SPI_TransmitReceive(&hspi1, dataTx, dataRx, 4, HAL_MAX_DELAY);
    // state = HAL_SPI_Receive(&hspi1, dataRx, 4, HAL_MAX_DELAY);

    HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_SET);

#else
    // todo add crc
#endif
}*/

uint16_t tx_buf[2], rx_buf[2];

void sendAndReceiveFrame(uint8_t dataTx[], uint8_t dataRx[])
{
#ifdef DISABLE_CRC
    assert_param(dataTx && dataRx);

    HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_RESET);

    // 将8位数组转换为16位数组，注意保持大端序
    tx_buf[0] = (dataTx[0] << 8) | dataTx[1];  // 高字节在前
    tx_buf[1] = (dataTx[2] << 8) | dataTx[3];  // 低字节在后

    static volatile HAL_StatusTypeDef state = HAL_OK;
    state                                   = HAL_SPI_TransmitReceive(
        &hspi1, (uint8_t *)tx_buf, (uint8_t *)rx_buf, 2, HAL_MAX_DELAY);

    // 将接收到的16位数据正确转换回8位数组
    dataRx[0] = rx_buf[0] >> 8;    // 取高字节
    dataRx[1] = rx_buf[0] & 0xFF;  // 取低字节
    dataRx[2] = rx_buf[1] >> 8;    // 取高字节
    dataRx[3] = rx_buf[1] & 0xFF;  // 取低字节

    HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_SET);
#endif
}

//****************************************************************************
//! Full Read Function for Normal Data Mode (DATA_TYPE = 000b)
//!
//! Takes in an output array of length 2, address to read from, and CMD bits to send
//! along, then creates the dataTx array and calls the sendAndReturnFrame function,
//! interpreting dataRx and putting the read register in output[0] and status bits in
//! output[1]
//!
//! output[2] - empty uint16_t array of length 2, output[0] will be assigned the returned
//! register
//!             at the given address, output[1] will be assigned the returned status bits.
//! address   - uint8_t value from 0x00 to 0x14 containing the register address to read
//! from cmd_bits  - uint8_t value from 0x00 to 0x03 containing the CMD0 and CMD1 bits
//! that will be sent
//!             in dataTx. LSB is CMD0, next bit is CMD1. (see header file or datasheet
//!             pg. 29 for CMD functions)
//****************************************************************************
void normalRead(uint16_t output[], uint8_t address, uint8_t cmd_bits)
{
    // Check that the input address is in range
    assert_param(address < NUM_REGISTERS);

    // Build TX and RX byte arrays
    uint8_t dataTx[4] = {0};
    uint8_t dataRx[4] = {0};

    // MSB is 1 for READ command
    dataTx[0] = (address | 0x80);
    dataTx[1] = 0x00;
    dataTx[2] = 0x00;
    dataTx[3] = cmd_bits << 4;

    sendAndReceiveFrame(dataTx, dataRx);
    output[0] = (dataRx[1] << 8) | dataRx[2];
    output[1] = (dataRx[0] << 4) | (dataRx[3] >> 4);
}

//****************************************************************************
//! Register-only Read Function for Normal Data Mode (DATA_TYPE = 000b)
//!
//! Takes in address to read from and returns register at address without the status bits
//! or triggering any CMD function (cmd_bits = 0x00).
//!
//! address   - uint8_t value from 0x00 to 0x14 containing the register address to read
//! from
//****************************************************************************
uint16_t normalReadRegister(uint8_t address)
{
    uint16_t output[2] = {0};
    normalRead(output, address, 0x00);
    return output[0];
}

void setSensorConfig()
{
    // To prevent undefined behavior, this function does not perform its operation
    // when the DATA_TYPE field (0x028-6) is not set to Normal Read Mode (000b)
    if (DATA_TYPE_RESULTS != DATA_TYPE_RESULTS_NormalMode)
        return;

    uint16_t input;
    input = normalReadRegister(SENSOR_CONFIG_ADDRESS);
    input = (input & ~(SENSOR_CONFIG_MAG_CH_EN_MASK)) | SENSOR_CONFIG_MAG_CH_EN_XYZ;
    input = (input & ~(SENSOR_CONFIG_X_RANGE_MASK)) | SENSOR_CONFIG_X_RANGE_25mT;
    input = (input & ~(SENSOR_CONFIG_Y_RANGE_MASK)) | SENSOR_CONFIG_Y_RANGE_25mT;
    input = (input & ~(SENSOR_CONFIG_Z_RANGE_MASK)) | SENSOR_CONFIG_Z_RANGE_100mT;

    writeToRegister(SENSOR_CONFIG_ADDRESS, input);
}



void setDeviceConfig()
{
    // To prevent undefined behavior, this function does not perform its operation
    // when the DATA_TYPE field (address: 0x028-6) is not set to Normal Read Mode (000b)
    if (DATA_TYPE_RESULTS != DATA_TYPE_RESULTS_NormalMode)
        return;

    volatile uint16_t input;
    // Set OPERATING_MODE (0x006-4) to Active Measure Mode (2h)
    input = normalReadRegister(DEVICE_CONFIG_ADDRESS);

    input = (input & ~(DEVICE_CONFIG_OPERATING_MODE_MASK)) | DEVICE_CONFIG_OPERATING_MODE_ActiveMeasureMode;

    input = (input & ~(DEVICE_CONFIG_CONV_AVG_NUM_MASK)) | DEVICE_CONFIG_CONV_AVG_NUM_4x333Kbps10Kbps1axis;

    writeToRegister(DEVICE_CONFIG_ADDRESS, input);

#ifdef MAX_DELAYS_IN_OPMODE_CHANGES
    delay_us(140);  // max expected delay as given by t_start_sleep + t_stand_by
                    // (datasheet pg. 10)
#endif
}


//****************************************************************************
//! Get Magnetic Measurements in mT (Normal Read Mode)
//!
//! In 32-bit normal read mode:
//! Takes in a size 3 array of floats and updates its measurements of the
//! three magnetic axes in mT. (order XYZ)
//!
//! INPUT ARRAY MUST BE SIZE 3 (or at least have meas_arr to meas_arr + 2 within scope)
//!
//! DOES NOT WORK IN SPECIAL READ MODE [DATA_TYPE field at 0x028-6 does not equal 000b]
//****************************************************************************
void getMagMeasurementsNrml(float meas_arr[])
{
    uint8_t i;

    // Array to store ranges for coordinates in the order XYZ
    uint16_t ranges[3] = {
        50, 50, 50};  // The default value for coordinate ranges is 50 mT (A1)
    // ranges[0]          = getXrange();
    // ranges[1]          = getYrange();
    // ranges[2]          = getZrange();

    static volatile int16_t data;

    for (i = 0; i < 3; ++i)
    {
        data        = normalReadRegister(X_CH_RESULT_ADDRESS + i);  // read in
        meas_arr[i] = data / 32768.0f;
    }
}

//****************************************************************************
//! Enable ALERT to Indicate Conversion
//!
//! Configures the device so when its magnetic measurements are complete, the device will
//! output LOW on the ALERT pin.
//!
//! NOTE: This configures ALERT as an output pin, the respective GPIO pin it is
//!       connected to will have to be set as an input as well. Please ensure none
//!       of the input functions of ALERT are being used as well (such as the ALERT
//!       trigger)
//!
//! DOES NOT WORK IN SPECIAL READ MODE [DATA_TYPE field at 0x028-6 does not equal 000b]
//****************************************************************************
void alertIndicatesConversionEnable()
{
    // To prevent undefined behavior, this function does not perform its operation
    // when the DATA_TYPE field (address: 0x028-6) is not set to Normal Read Mode (000b)
    if (DATA_TYPE_RESULTS != DATA_TYPE_RESULTS_NormalMode)
        return;

    uint16_t input;
    // SET ALERT_MODE (address: 0x03C) to Interrupt Mode (0h)
    input = normalReadRegister(ALERT_CONFIG_ADDRESS);
    input &= ~(ALERT_CONFIG_ALERT_MODE_MASK);
    input |= ALERT_CONFIG_ALERT_MODE_InterruptandTriggerMode;

    // SET RSLT_ALRT (address: 0x038) to ALERT output asserted LOW to indicate conversion
    // completion (1h) (Register already grabbed so no new READ command is needed)
    input |= ALERT_CONFIG_RSLT_ALRT_ALERToutputsignalsconversioncomplete;
    writeToRegister(ALERT_CONFIG_ADDRESS, input);
}


uint8_t getVersion()
{
    return (normalReadRegister(TEST_CONFIG_ADDRESS) & TEST_CONFIG_VER_MASK) >> 4;
}

void delay_us(uint32_t us)
{
    __HAL_TIM_SET_COUNTER(&htim16, 0);
    while (__HAL_TIM_GET_COUNTER(&htim16) < us)
        ;
}

/**
 * 0: x, 1: y, 2: z, 3: finished
 */
uint16_t queryNextItemDMA = 0;
uint16_t txBuffers[3][2];  // 3 axes, 4 bytes per SPI frame
uint16_t rxBuffers[3][2];  // 3 axes, 4 bytes per SPI frame
uint32_t errorCount = 0;

void initDMATxBuffers()
{
    for (int i = 0; i < 3; i++)
    {
        txBuffers[i][0] = (0x80 | (X_CH_RESULT_ADDRESS + i)) << 8;
        txBuffers[i][1] = 0x00;

        rxBuffers[i][0] = 0x01;
        rxBuffers[i][1] = 0x01;
    }
    queryNextItemDMA = 0;
    errorCount       = 0;
}

void startDMASequentialNormalReadXYZ()
{
    if (queryNextItemDMA != 0)
    {
        // last read not finished
        errorCount++;
        return;
    }

    MAG_CS_GPIO_Port->BRR = (uint32_t)MAG_CS_Pin;  // HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive_DMA(&hspi1, (uint8_t *)txBuffers[0], (uint8_t *)rxBuffers[0], 2);
    queryNextItemDMA = 1;
}

void continueDMASequentialNormalReadXYZ()
{
    if (queryNextItemDMA > 3 || queryNextItemDMA == 0)
    {
        // Invalid state
        errorCount++;
        //                queryNextItemDMA = 0;
        return;
    }

    if (queryNextItemDMA == 3)  // no other read after this receiving
    {
        queryNextItemDMA = 0;
        MAG_CS_GPIO_Port->BSRR =
            (uint32_t)MAG_CS_Pin;  // HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_SET);
        for (int i = 0; i < 3; ++i)
        {
            Tasks::PositionControl::magMeasurement[i] = (float)((int16_t)(((rxBuffers[i][0] & 0xFF) << 8) | rxBuffers[i][1] >> 8)) / 32768.0f;
        }
        // todo: Callback
        Tasks::PositionControl::updatePosition();
        return;
    }
    else  // i = 1 or 2
    {
        MAG_CS_GPIO_Port->BSRR =
            (uint32_t)MAG_CS_Pin;  // HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_SET);
        delay_us(5);
        MAG_CS_GPIO_Port->BRR =
            (uint32_t)MAG_CS_Pin;  // HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_RESET);
        HAL_SPI_TransmitReceive_DMA(
            &hspi1, (uint8_t *)txBuffers[queryNextItemDMA], (uint8_t *)rxBuffers[queryNextItemDMA], 2);
        queryNextItemDMA++;  // 1->2 or 2->3
    }
}

} // namespace Drivers::Sensors::TMAG5170


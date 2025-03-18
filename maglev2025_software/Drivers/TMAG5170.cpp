//
// Created by eeasee on 2025/3/11.
//

#include "TMAG5170.hpp"
#include "assert.h"

namespace Drivers {
    namespace Sensors {
    namespace TMAG5170 {
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
    for (i = 0;  i < 32;  i++)
    {
        x = (x << 1) | 1;
        if (y < x) x -= 2;
        else y -= x;
        x++;
        y <<= 1;
        if ((h & 0x80000000)) y |= 1;
        h <<= 1;
        y <<= 1;
        if ((h & 0x80000000)) y |= 1;
        h <<= 1;
    }
    return  x;
}

//****************************************************************************
//! Convert Axis Measurement Result Register to mT value
//!
//! Takes in the register of one of the magnetic axis results and the range for
//! that specific axis and calculates the mT value the register represents.
//****************************************************************************
float resultRegisterTomT( int16_t register_bits, uint16_t range )
{
    return (((float) register_bits) / 32768) * range;
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

//****************************************************************************
//! Convert Angle Result Register to degree value
//!
//! Takes in the bits of the angle result register and converts it to a
//! degree value.
//****************************************************************************
float angleRegisterToDeg( uint16_t register_bits )
{
    return ( (float) register_bits / 16 );
}

//****************************************************************************
//! Calculate CRC for SPI data frame
//!
//! Takes in an array containing a SPI data frame (MSB to LSB) with the CRC bits
//! all set to ZERO and calculates and returns the CRC for that data frame.
//****************************************************************************
uint8_t calculateCRC( uint8_t data[] )
{
    int i = 0;
    uint8_t crc = 0x00;
    uint32_t n;

    // Build TX and RX byte array
    uint8_t d[32] = { 0 };

    n = (data[0] << 24)|(data[1] << 16)|(data[2] << 8)|(data[3]);

    while (n>0)
    {
        d[i] = n&1;
        n = n>>1;
        i++;
    }

    crc |= d[30] ^ d[26] ^ d[25] ^ d[24] ^ d[23] ^ d[21] ^ d[19] ^ d[18] ^ d[15] ^ d[11] ^ d[10] ^ d[9] ^ d[8] ^ d[6] ^ d[4] ^ d[3] ^ d[0] ^ 1;
    crc |= (d[31] ^ d[30] ^ d[27] ^ d[23] ^ d[22] ^ d[21] ^ d[20] ^ d[18] ^ d[16] ^ d[15] ^ d[12] ^ d[8] ^ d[7] ^ d[6] ^ d[5] ^ d[3] ^ d[1] ^ d[0] ^ 1 ^ 1) << 1;
    crc |= (d[31] ^ d[28] ^ d[24] ^ d[23] ^ d[22] ^ d[21] ^ d[19] ^ d[17] ^ d[16] ^ d[13] ^ d[9] ^ d[8] ^ d[7] ^ d[6] ^ d[4] ^ d[2] ^ d[1] ^ 1 ^ 1) << 2;
    crc |= (d[29] ^ d[25] ^ d[24] ^ d[23] ^ d[22] ^ d[20] ^ d[18] ^ d[17] ^ d[14] ^ d[10] ^ d[9] ^ d[8] ^ d[7] ^ d[5] ^ d[3] ^ d[2] ^ 1) << 3;

    return crc;
}


//****************************************************************************
//! Verify CRC in SPI data frame
//!
//! Takes in an array containing a SPI data frame (MSB to LSB) and checks if the
//! CRC bits (according to their locations for the TMAG5170) are correct according
//! to the CRC calculation algorithm.
//****************************************************************************
uint8_t verifyCRC( uint8_t data[] )
{
    uint8_t crc_received = data[3] & 0x0F;
    data[3] &= ~(0x0F); // the CRC bits of the data must be 0000b to calculate its CRC correctly
    uint8_t crc_calc = calculateCRC(data);
    data[3] |= crc_received; // the previously removed CRC bits are reinserted

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
    writeToRegister( DEVICE_CONFIG_ADDRESS, DEVICE_CONFIG_OPERATING_MODE_DeepSleepMode );
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
void exitDeepSleepMode(void)
{
    // A LOW pulse is needed on CS to exit Deep Sleep Mode (enters Configuration Mode)
    HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_RESET);
    delay_us(2); //cs pulse
    HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_SET);
    delay_us(500); // max expected delay as given by t_start_deep_sleep (datasheet pg. 10)
    //device reset


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
    enterDeepSleepMode(); // Deep Sleep Mode resets the device to its default settings
    exitDeepSleepMode();
}


void TMAG5170_init()
{
    HAL_Delay(50);
    HAL_TIM_Base_Start(&htim16);
    resetDevice();
    
    //disable crc in the initialization
    uint8_t dataTx[4] = {0x0F,0x00,0x04,0x07};
    uint8_t dataRx[4] = {0};

    sendAndReceiveFrame(dataTx, dataRx);
    //transmit the data,but should be ok if not received?
    // spiSendReceiveArrays(dataTx, dataRx, TMAG5170_FRAME_NUM_BYTES);

    // Check if SYSTEM_CONFIG was written to and update SYSTEM_CONFIG_stored if so
    if ( dataTx[0] == SYSTEM_CONFIG_ADDRESS ) SYSTEM_CONFIG_stored = (dataTx[1] << 8) | dataTx[2];
    SYSTEM_CONFIG_stored &= ~(0xC818); // Reserved bits cannot be 1, this ensures the
                                       // stored variable doesn't have them changed
    HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_RESET);
    

}

//****************************************************************************
//! writing to the register to send cmd
//!
//****************************************************************************
void writeToRegister(uint8_t address, uint16_t data_to_write){
    // Check that the input address is in range
    assert(address < NUM_REGISTERS);

    // Build TX and RX byte arrays
    uint8_t dataTx[4] = { 0 };
    uint8_t dataRx[4] = { 0 };

    // MSB is 0 for WRITE command
    dataTx[0] = (address);
    dataTx[1] = (data_to_write >> 8);
    dataTx[2] = (data_to_write);
    dataTx[3] = 0x00;

    sendAndReceiveFrame(dataTx, dataRx);
}

void sendAndReceiveFrame(uint8_t dataTx[], uint8_t dataRx[]){
#ifdef DISABLE_CRC

    // Require that dataTx and dataRx are not NULL pointers
    assert(dataTx && dataRx);

    HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_RESET);

    //TAMG5170_FRAME_NUM_BYTES = 4 is default
    for(int i = 0; i < 4; i++){
        HAL_SPI_Transmit(&hspi1, &dataTx[i], 1, HAL_MAX_DELAY);
        HAL_SPI_Receive(&hspi1, &dataRx[i], 1, HAL_MAX_DELAY);
    }
    HAL_GPIO_WritePin(MAG_CS_GPIO_Port, MAG_CS_Pin, GPIO_PIN_SET);


#else
    //todo add crc
#endif
}



}// namespace
}
}
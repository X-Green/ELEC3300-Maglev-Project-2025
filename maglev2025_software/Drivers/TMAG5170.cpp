//
// Created by eeasee on 2025/3/11.
//

#include "TMAG5170.hpp"



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


// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Converison functions
//
// These can be used to encode, and decode 8 16-bit channels in the
// maximum of 4 32-bit channels defined by blink
//


/**
 * Convert a float to a uint without changing the bit values.
 *
 * @arg floatValue: The float value to convert.
 *
 * @returns: The uint value that has the same bit pattern.
 */
inline uint floatToUint(const float floatValue)
{
    return *(uint*) &floatValue;
}


/**
 * Convert a uint to a float without changing the bit values.
 *
 * @arg uintValue: The uint value to convert.
 *
 * @returns: The float value that has the same bit pattern.
 */
inline float uintToFloat(const uint uintValue)
{
    return *(float*) &uintValue;
}


/**
 * IEEE-754 16-bit floating-point format (without infinity): 1-5-10,
 * exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
 * stackoverflow.com/questions/1659440/32-bit-to-16-bit-floating-point-conversion/60047308#60047308
 *
 * @arg halfValue: The half value to convert.
 *
 * @returns: The float value that has the same bit pattern.
 */
float halfToFloat(const uint halfValue)
{
    // exponent
    const uint e = (halfValue & 0x7C00) >> 10;

    // mantissa
    const uint m = (halfValue & 0x03FF) << 13;

    // evil log2 bit hack to count leading zeros in denormalized format
    const uint v = floatToUint((float) m) >> 23;

    // sign : normalized : denormalized
    return uintToFloat(
        (halfValue & 0x8000) << 16
        | (e != 0) * ((e + 112) << 23 | m)
        | ((e == 0) & (m != 0))
        * ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000))
    );
}


/**
 * IEEE-754 16-bit floating-point format (without infinity): 1-5-10,
 * exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
 * stackoverflow.com/questions/1659440/32-bit-to-16-bit-floating-point-conversion/60047308#60047308
 *
 * @arg floatValue: The float value to convert.
 *
 * @returns: The half value that has the same bit pattern.
 */
uint floatToHalf(const float floatValue)
{
    // round-to-nearest-even: add last bit after truncated mantissa
    const uint b = floatToUint(floatValue) + 0x00001000;

    // exponent
    const uint e = (b & 0x7F800000) >> 23;

    // mantissa; in line below:
    // 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
    const uint m = b & 0x007FFFFF;

    // sign : normalized : denormalized : saturate
    return (
        (b & 0x80000000) >> 16
        | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13)
        | ((e < 113) & (e > 101))
        * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1)
        | (e > 143) * 0x7FFF
    );
}


/**
 * Encodes two 32-bit floats as 16-bit floats in a 32-bit uint.
 *
 * @arg value0: The first value to encode.
 * @arg value1: The second value to encode.
 *
 * @returns: The 32-bit uint with two 16-bit floats packed into it.
 */
uint encodeFloatsInUint(const float value0, const float value1)
{
    return (floatToHalf(value0) << 16) + floatToHalf(value1);
}


/**
 * Decodes two 16-bit floats as 32-bit floats from a 32-bit uint.
 *
 * @arg value: The 32-bit uint with two 16-bit floats packed into it.
 *
 * @returns: The two decoded floats.
 */
float2 decodeFloatsFromUint(const uint value)
{
    return float2(
        halfToFloat(value >> 16),
        halfToFloat(value & 0x0000FFFF)
    );
}


/**
 * Encodes two 32-bit uints as 16-bit uints in a 32-bit uint.
 *
 * @arg value0: The first value to encode.
 * @arg value1: The second value to encode.
 *
 * @returns: The 32-bit uint with two 16-bit uints packed into it.
 */
uint encodeTwoValuesInUint(const uint value0, const uint value1)
{
    return (value0 << 16) + (value1 & 0x0000FFFF);
}


/**
 * Decodes two 16-bit uints as 32-bit ints from a 32-bit uint.
 *
 * @arg value: The 32-bit uint with two 16-bit uints packed into it.
 *
 * @returns: The two decoded uints.
 */
int2 decodeTwoValuesFromUint(const uint value)
{
    return int2(
        value >> 16,
        value & 0x0000FFFF
    );
}

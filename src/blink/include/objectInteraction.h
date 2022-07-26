// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Signed Distance Transformations
//
// These operate on the signed distances that have been computed
//
#define SUBTRACTION 128
#define INTERSECTION 256
#define SMOOTH_UNION 512
#define SMOOTH_SUBTRACTION 1024
#define SMOOTH_INTERSECTION 2048


/**
 * The union of the two distances. Ie. The minimum. The corresponding
 * colour will be placed in colour1, and the corresponding surface will
 * be placed in colour9.
 *
 * @arg value0: The first distance.
 * @arg value1: The second distance.
 * @arg colour0: The first colour.
 * @arg colour1: The second colour.
 * @arg colour8: The first surface.
 * @arg colour9: The second surface.
 *
 * @returns: The nearest, modified distance.
 */
inline float union_(
        const float value0,
        const float value1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &colour2,
        float4 &colour3,
        const float4 &colour4,
        float4 &colour5,
        const float4 &colour6,
        float4 &colour7,
        const float4 &colour8,
        float4 &colour9,
        const float value2,
        float &value3,
        const float value4,
        float &value5)
{
    if (fabs(value0) < fabs(value1))
    {
        colour1 = colour0;
        colour3 = colour2;
        colour5 = colour4;
        colour7 = colour6;
        colour9 = colour8;
        value3 = value2;
        value5 = value4;
        return value0;
    }
    return value1;
}


/**
 * The union of the two distances. Ie. The minimum. The corresponding
 * colour will be placed in colour1, and the corresponding surface will
 * be placed in colour9.
 *
 * @arg value0: The first distance.
 * @arg value1: The second distance.
 *
 * @returns: The nearest, modified distance.
 */
inline float union_(const float value0, const float value1)
{
    if (fabs(value0) < fabs(value1))
    {
        return value0;
    }
    return value1;
}


/**
 * Subtract the first object from the second. The corresponding
 * colour will be placed in colour1, and the corresponding surface will
 * be placed in colour9.
 *
 * @arg value0: The first distance.
 * @arg value1: The second distance.
 * @arg colour0: The first colour.
 * @arg colour1: The second colour.
 * @arg colour8: The first surface.
 * @arg colour9: The second surface.
 *
 * @returns: The nearest, modified distance.
 */
inline float subtraction(
        const float value0,
        const float value1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &colour2,
        float4 &colour3,
        const float4 &colour4,
        float4 &colour5,
        const float4 &colour6,
        float4 &colour7,
        const float4 &colour8,
        float4 &colour9,
        const float value2,
        float &value3,
        const float value4,
        float &value5)
{
    if (-value0 > value1)
    {
        colour1 = colour0;
        colour3 = colour2;
        colour5 = colour4;
        colour7 = colour6;
        colour9 = colour8;
        value3 = value2;
        value5 = value4;
        return -value0;
    }
    return value1;
}


/**
 * Subtract the first object from the second. The corresponding
 * colour will be placed in colour1, and the corresponding surface will
 * be placed in colour9.
 *
 * @arg value0: The first distance.
 * @arg value1: The second distance.
 *
 * @returns: The nearest, modified distance.
 */
inline float subtraction(const float value0, const float value1)
{
    return max(-value0, value1);
}


/**
 * Render only the overlapping region of two objects. The corresponding
 * colour will be placed in colour1, and the corresponding surface will
 * be placed in colour9.
 *
 * @arg value0: The first distance.
 * @arg value1: The second distance.
 * @arg colour0: The first colour.
 * @arg colour1: The second colour.
 * @arg colour8: The first surface.
 * @arg colour9: The second surface.
 *
 * @returns: The nearest, modified distance.
 */
inline float intersection(
        const float value0,
        const float value1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &colour2,
        float4 &colour3,
        const float4 &colour4,
        float4 &colour5,
        const float4 &colour6,
        float4 &colour7,
        const float4 &colour8,
        float4 &colour9,
        const float value2,
        float &value3,
        const float value4,
        float &value5)
{
    if (value0 > value1)
    {
        colour1 = colour0;
        colour3 = colour2;
        colour5 = colour4;
        colour7 = colour6;
        colour9 = colour8;
        value3 = value2;
        value5 = value4;
        return value0;
    }
    return value1;
}


/**
 * Render only the overlapping region of two objects. The corresponding
 * colour will be placed in colour1, and the corresponding surface will
 * be placed in colour9.
 *
 * @arg value0: The first distance.
 * @arg value1: The second distance.
 *
 * @returns: The nearest, modified distance.
 */
inline float intersection(const float value0, const float value1)
{
    return max(value0, value1);
}


/**
 * Smoothly blend between two objects. Ie. The minimum. The
 * corresponding colour will be placed in colour1, and the corresponding
 * surface will be placed in colour9.
 *
 * @arg value0: The first distance.
 * @arg value1: The second distance.
 * @arg colour0: The first colour.
 * @arg colour1: The second colour.
 * @arg colour8: The first surface.
 * @arg colour9: The second surface.
 * @arg blendSize: The amount to blend between the objects.
 *
 * @returns: The nearest, modified distance.
 */
inline float smoothUnion(
        const float value0,
        const float value1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &colour2,
        float4 &colour3,
        const float4 &colour4,
        float4 &colour5,
        const float4 &colour6,
        float4 &colour7,
        const float4 &colour8,
        float4 &colour9,
        const float value2,
        float &value3,
        const float value4,
        float &value5,
        const float blendSize)
{
    float amount = saturate(0.5f + 0.5f * (fabs(value1) - fabs(value0)) / blendSize);

    colour1 = blend(colour0, colour1, amount);
    colour3 = blend(colour2, colour3, amount);
    colour5 = blend(colour4, colour5, amount);
    colour7 = blend(colour6, colour7, amount);
    colour9 = blend(colour8, colour9, amount);
    value3 = blend(value2, value3, amount);
    value5 = blend(value4, value5, amount);

    return blend(value0, value1, amount) - blendSize * amount * (1.0f - amount);
}


/**
 * Smoothly blend between two objects. Ie. The minimum. The
 * corresponding colour will be placed in colour1, and the corresponding
 * surface will be placed in colour9.
 *
 * @arg value0: The first distance.
 * @arg value1: The second distance.
 * @arg blendSize: The amount to blend between the objects.
 *
 * @returns: The nearest, modified distance.
 */
inline float smoothUnion(const float value0, const float value1, const float blendSize)
{
    float amount = saturate(0.5f + 0.5f * (fabs(value1) - fabs(value0)) / blendSize);
    return blend(value0, value1, amount) - blendSize * amount * (1.0f - amount);
}


/**
 * Smoothly blend the subtraction of the first object from the second.
 * The corresponding colour will be placed in colour1, and the
 * corresponding surface will be placed in colour9.
 *
 * @arg value0: The first distance.
 * @arg value1: The second distance.
 * @arg colour0: The first colour.
 * @arg colour1: The second colour.
 * @arg colour8: The first surface.
 * @arg colour9: The second surface.
 * @arg blendSize: The amount to blend between the objects.
 *
 * @returns: The nearest, modified distance.
 */
inline float smoothSubtraction(
        const float value0,
        const float value1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &colour2,
        float4 &colour3,
        const float4 &colour4,
        float4 &colour5,
        const float4 &colour6,
        float4 &colour7,
        const float4 &colour8,
        float4 &colour9,
        const float value2,
        float &value3,
        const float value4,
        float &value5,
        const float blendSize)
{
    float amount = saturate(0.5f - 0.5f * (value1 + value0) / blendSize);

    colour1 = blend(colour0, colour1, amount);
    colour3 = blend(colour2, colour3, amount);
    colour5 = blend(colour4, colour5, amount);
    colour7 = blend(colour6, colour7, amount);
    colour9 = blend(colour8, colour9, amount);
    value3 = blend(value2, value3, amount);
    value5 = blend(value4, value5, amount);

    return blend(-value0, value1, amount) + blendSize * amount * (1.0f - amount);
}


/**
 * Smoothly blend the subtraction of the first object from the second.
 * The corresponding colour will be placed in colour1, and the
 * corresponding surface will be placed in colour9.
 *
 * @arg value0: The first distance.
 * @arg value1: The second distance.
 * @arg blendSize: The amount to blend between the objects.
 *
 * @returns: The nearest, modified distance.
 */
inline float smoothSubtraction(
        const float value0,
        const float value1,
        const float blendSize)
{
    float amount = saturate(0.5f - 0.5f * (value1 + value0) / blendSize);
    return blend(-value0, value1, amount) + blendSize * amount * (1.0f - amount);
}


/**
 * Smoothly blend the overlapping region of two objects. The
 * corresponding colour will be placed in colour1, and the corresponding
 * surface will be placed in colour9.
 *
 * @arg value0: The first distance.
 * @arg value1: The second distance.
 * @arg colour0: The first colour.
 * @arg colour1: The second colour.
 * @arg colour8: The first surface.
 * @arg colour9: The second surface.
 * @arg blendSize: The amount to blend between the objects.
 *
 * @returns: The nearest, modified distance.
 */
inline float smoothIntersection(
        const float value0,
        const float value1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &colour2,
        float4 &colour3,
        const float4 &colour4,
        float4 &colour5,
        const float4 &colour6,
        float4 &colour7,
        const float4 &colour8,
        float4 &colour9,
        const float value2,
        float &value3,
        const float value4,
        float &value5,
        const float blendSize)
{
    float amount = saturate(0.5f - 0.5f * (value1 - value0) / blendSize);

    colour1 = blend(colour0, colour1, amount);
    colour3 = blend(colour2, colour3, amount);
    colour5 = blend(colour4, colour5, amount);
    colour7 = blend(colour6, colour7, amount);
    colour9 = blend(colour8, colour9, amount);
    value3 = blend(value2, value3, amount);
    value5 = blend(value4, value5, amount);

    return blend(value0, value1, amount) + blendSize * amount * (1.0f - amount);
}


/**
 * Smoothly blend the overlapping region of two objects. The
 * corresponding colour will be placed in colour1, and the corresponding
 * surface will be placed in colour9.
 *
 * @arg value0: The first distance.
 * @arg value1: The second distance.
 * @arg blendSize: The amount to blend between the objects.
 *
 * @returns: The nearest, modified distance.
 */
inline float smoothIntersection(
        const float value0,
        const float value1,
        const float blendSize)
{
    float amount = saturate(0.5f - 0.5f * (value1 - value0) / blendSize);
    return blend(value0, value1, amount) + blendSize * amount * (1.0f - amount);
}


/**
 * Compute the modified distance resulting from the interaction between
 * two objects. The corresponding colour will be placed in colour1, and
 * the corresponding surface will be placed in colour9.
 *
 * @arg modifications: The modification to perform:
 *     Each bit will enable a modification:
 *         bit 7: subtraction
 *         bit 8: intersection
 *         bit 9: smooth union
 *         bit 10: smooth subtraction
 *         bit 11: smooth intersection
 *     any other value will default to union.
 * @arg value0: The first distance.
 * @arg value1: The second distance.
 * @arg colour0: The first colour.
 * @arg colour1: The second colour.
 * @arg colour8: The first surface.
 * @arg colour9: The second surface.
 * @arg blendSize: The amount to blend between the objects.
 *
 * @returns: The nearest, modified distance.
 */
float performChildInteraction(
        const int modifications,
        const float value0,
        const float value1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &colour2,
        float4 &colour3,
        const float4 &colour4,
        float4 &colour5,
        const float4 &colour6,
        float4 &colour7,
        const float4 &colour8,
        float4 &colour9,
        const float value2,
        float &value3,
        const float value4,
        float &value5,
        const float blendSize)
{
    if (modifications & SUBTRACTION)
    {
        return subtraction(
            value0,
            value1,
            colour0,
            colour1,
            colour2,
            colour3,
            colour4,
            colour5,
            colour6,
            colour7,
            colour8,
            colour9,
            value2,
            value3,
            value4,
            value5
        );
    }
    if (modifications & INTERSECTION)
    {
        return intersection(
            value0,
            value1,
            colour0,
            colour1,
            colour2,
            colour3,
            colour4,
            colour5,
            colour6,
            colour7,
            colour8,
            colour9,
            value2,
            value3,
            value4,
            value5
        );
    }
    if (modifications & SMOOTH_UNION)
    {
        return smoothUnion(
            value0,
            value1,
            colour0,
            colour1,
            colour2,
            colour3,
            colour4,
            colour5,
            colour6,
            colour7,
            colour8,
            colour9,
            value2,
            value3,
            value4,
            value5,
            blendSize
        );
    }
    if (modifications & SMOOTH_SUBTRACTION)
    {
        return smoothSubtraction(
            value0,
            value1,
            colour0,
            colour1,
            colour2,
            colour3,
            colour4,
            colour5,
            colour6,
            colour7,
            colour8,
            colour9,
            value2,
            value3,
            value4,
            value5,
            blendSize
        );
    }
    if (modifications & SMOOTH_INTERSECTION)
    {
        return smoothIntersection(
            value0,
            value1,
            colour0,
            colour1,
            colour2,
            colour3,
            colour4,
            colour5,
            colour6,
            colour7,
            colour8,
            colour9,
            value2,
            value3,
            value4,
            value5,
            blendSize
        );
    }
    return union_(
        value0,
        value1,
        colour0,
        colour1,
        colour2,
        colour3,
        colour4,
        colour5,
        colour6,
        colour7,
        colour8,
        colour9,
        value2,
        value3,
        value4,
        value5
    );
}


/**
 * Compute the modified distance resulting from the interaction between
 * two objects. The corresponding colour will be placed in colour1, and
 * the corresponding surface will be placed in colour9.
 *
 * @arg modifications: The modification to perform:
 *     Each bit will enable a modification:
 *         bit 7: subtraction
 *         bit 8: intersection
 *         bit 9: smooth union
 *         bit 10: smooth subtraction
 *         bit 11: smooth intersection
 *     any other value will default to union.
 * @arg value0: The first distance.
 * @arg value1: The second distance.
 * @arg blendSize: The amount to blend between the objects.
 *
 * @returns: The nearest, modified distance.
 */
float performChildInteraction(
        const int modifications,
        const float value0,
        const float value1,
        const float blendSize)
{
    if (modifications & SUBTRACTION)
    {
        return subtraction(value0, value1);
    }
    if (modifications & INTERSECTION)
    {
        return intersection(value0, value1);
    }
    if (modifications & SMOOTH_UNION)
    {
        return smoothUnion(value0, value1, blendSize);
    }
    if (modifications & SMOOTH_SUBTRACTION)
    {
        return smoothSubtraction(value0, value1, blendSize);
    }
    if (modifications & SMOOTH_INTERSECTION)
    {
        return smoothIntersection(value0, value1, blendSize);
    }
    return union_(value0, value1);
}

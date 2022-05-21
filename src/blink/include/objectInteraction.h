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


/**
 * The union of the two distances. Ie. The minimum. The corresponding
 * colour will be placed in colour1, and the corresponding surface will
 * be placed in surface1.
 *
 * @arg distance0: The first distance.
 * @arg distance1: The second distance.
 * @arg colour0: The first colour.
 * @arg colour1: The second colour.
 * @arg surface0: The first surface.
 * @arg surface1: The second surface.
 *
 * @returns: The nearest, modified distance.
 */
inline float union_(
        const float distance0,
        const float distance1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &colour2,
        float4 &colour3,
        const float4 &colour4,
        float4 &colour5,
        const float4 &colour6,
        float4 &colour7,
        const float4 &surface0,
        float4 &surface1)
{
    if (fabs(distance0) < fabs(distance1))
    {
        colour1 = colour0;
        colour3 = colour2;
        colour5 = colour4;
        colour7 = colour6;
        surface1 = surface0;
        return distance0;
    }
    return distance1;
}


/**
 * The union of the two distances. Ie. The minimum. The corresponding
 * colour will be placed in colour1, and the corresponding surface will
 * be placed in surface1.
 *
 * @arg distance0: The first distance.
 * @arg distance1: The second distance.
 *
 * @returns: The nearest, modified distance.
 */
inline float union_(const float distance0, const float distance1)
{
    if (fabs(distance0) < fabs(distance1))
    {
        return distance0;
    }
    return distance1;
}


/**
 * Subtract the first object from the second. The corresponding
 * colour will be placed in colour1, and the corresponding surface will
 * be placed in surface1.
 *
 * @arg distance0: The first distance.
 * @arg distance1: The second distance.
 * @arg colour0: The first colour.
 * @arg colour1: The second colour.
 * @arg surface0: The first surface.
 * @arg surface1: The second surface.
 *
 * @returns: The nearest, modified distance.
 */
inline float subtraction(
        const float distance0,
        const float distance1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &colour2,
        float4 &colour3,
        const float4 &colour4,
        float4 &colour5,
        const float4 &colour6,
        float4 &colour7,
        const float4 &surface0,
        float4 &surface1)
{
    if (-distance0 > distance1)
    {
        colour1 = colour0;
        colour3 = colour2;
        colour5 = colour4;
        colour7 = colour6;
        surface1 = surface0;
        return -distance0;
    }
    return distance1;
}


/**
 * Subtract the first object from the second. The corresponding
 * colour will be placed in colour1, and the corresponding surface will
 * be placed in surface1.
 *
 * @arg distance0: The first distance.
 * @arg distance1: The second distance.
 *
 * @returns: The nearest, modified distance.
 */
inline float subtraction(const float distance0, const float distance1)
{
    return max(-distance0, distance1);
}


/**
 * Render only the overlapping region of two objects. The corresponding
 * colour will be placed in colour1, and the corresponding surface will
 * be placed in surface1.
 *
 * @arg distance0: The first distance.
 * @arg distance1: The second distance.
 * @arg colour0: The first colour.
 * @arg colour1: The second colour.
 * @arg surface0: The first surface.
 * @arg surface1: The second surface.
 *
 * @returns: The nearest, modified distance.
 */
inline float intersection(
        const float distance0,
        const float distance1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &colour2,
        float4 &colour3,
        const float4 &colour4,
        float4 &colour5,
        const float4 &colour6,
        float4 &colour7,
        const float4 &surface0,
        float4 &surface1)
{
    if (distance0 > distance1)
    {
        colour1 = colour0;
        colour3 = colour2;
        colour5 = colour4;
        colour7 = colour6;
        surface1 = surface0;
        return distance0;
    }
    return distance1;
}


/**
 * Render only the overlapping region of two objects. The corresponding
 * colour will be placed in colour1, and the corresponding surface will
 * be placed in surface1.
 *
 * @arg distance0: The first distance.
 * @arg distance1: The second distance.
 *
 * @returns: The nearest, modified distance.
 */
inline float intersection(const float distance0, const float distance1)
{
    return max(distance0, distance1);
}


/**
 * Smoothly blend between two objects. Ie. The minimum. The
 * corresponding colour will be placed in colour1, and the corresponding
 * surface will be placed in surface1.
 *
 * @arg distance0: The first distance.
 * @arg distance1: The second distance.
 * @arg colour0: The first colour.
 * @arg colour1: The second colour.
 * @arg surface0: The first surface.
 * @arg surface1: The second surface.
 * @arg blendSize: The amount to blend between the objects.
 *
 * @returns: The nearest, modified distance.
 */
inline float smoothUnion(
        const float distance0,
        const float distance1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &colour2,
        float4 &colour3,
        const float4 &colour4,
        float4 &colour5,
        const float4 &colour6,
        float4 &colour7,
        const float4 &surface0,
        float4 &surface1,
        const float blendSize)
{
    float amount = saturate(0.5f + 0.5f * (fabs(distance1) - fabs(distance0)) / blendSize);

    colour1 = blend(colour0, colour1, amount);
    colour3 = blend(colour2, colour3, amount);
    colour5 = blend(colour4, colour5, amount);
    colour7 = blend(colour6, colour7, amount);
    surface1 = blend(surface0, surface1, amount);

    return blend(distance0, distance1, amount) - blendSize * amount * (1.0f - amount);
}


/**
 * Smoothly blend between two objects. Ie. The minimum. The
 * corresponding colour will be placed in colour1, and the corresponding
 * surface will be placed in surface1.
 *
 * @arg distance0: The first distance.
 * @arg distance1: The second distance.
 * @arg blendSize: The amount to blend between the objects.
 *
 * @returns: The nearest, modified distance.
 */
inline float smoothUnion(const float distance0, const float distance1, const float blendSize)
{
    float amount = saturate(0.5f + 0.5f * (fabs(distance1) - fabs(distance0)) / blendSize);
    return blend(distance0, distance1, amount) - blendSize * amount * (1.0f - amount);
}


/**
 * Smoothly blend the subtraction of the first object from the second.
 * The corresponding colour will be placed in colour1, and the
 * corresponding surface will be placed in surface1.
 *
 * @arg distance0: The first distance.
 * @arg distance1: The second distance.
 * @arg colour0: The first colour.
 * @arg colour1: The second colour.
 * @arg surface0: The first surface.
 * @arg surface1: The second surface.
 * @arg blendSize: The amount to blend between the objects.
 *
 * @returns: The nearest, modified distance.
 */
inline float smoothSubtraction(
        const float distance0,
        const float distance1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &colour2,
        float4 &colour3,
        const float4 &colour4,
        float4 &colour5,
        const float4 &colour6,
        float4 &colour7,
        const float4 &surface0,
        float4 &surface1,
        const float blendSize)
{
    float amount = saturate(0.5f - 0.5f * (distance1 + distance0) / blendSize);

    colour1 = blend(colour0, colour1, amount);
    colour3 = blend(colour2, colour3, amount);
    colour5 = blend(colour4, colour5, amount);
    colour7 = blend(colour6, colour7, amount);
    surface1 = blend(surface0, surface1, amount);

    return blend(-distance0, distance1, amount) + blendSize * amount * (1.0f - amount);
}


/**
 * Smoothly blend the subtraction of the first object from the second.
 * The corresponding colour will be placed in colour1, and the
 * corresponding surface will be placed in surface1.
 *
 * @arg distance0: The first distance.
 * @arg distance1: The second distance.
 * @arg blendSize: The amount to blend between the objects.
 *
 * @returns: The nearest, modified distance.
 */
inline float smoothSubtraction(
        const float distance0,
        const float distance1,
        const float blendSize)
{
    float amount = saturate(0.5f - 0.5f * (distance1 + distance0) / blendSize);
    return blend(-distance0, distance1, amount) + blendSize * amount * (1.0f - amount);
}


/**
 * Smoothly blend the overlapping region of two objects. The
 * corresponding colour will be placed in colour1, and the corresponding
 * surface will be placed in surface1.
 *
 * @arg distance0: The first distance.
 * @arg distance1: The second distance.
 * @arg colour0: The first colour.
 * @arg colour1: The second colour.
 * @arg surface0: The first surface.
 * @arg surface1: The second surface.
 * @arg blendSize: The amount to blend between the objects.
 *
 * @returns: The nearest, modified distance.
 */
inline float smoothIntersection(
        const float distance0,
        const float distance1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &colour2,
        float4 &colour3,
        const float4 &colour4,
        float4 &colour5,
        const float4 &colour6,
        float4 &colour7,
        const float4 &surface0,
        float4 &surface1,
        const float blendSize)
{
    float amount = saturate(0.5f - 0.5f * (distance1 - distance0) / blendSize);

    colour1 = blend(colour0, colour1, amount);
    colour3 = blend(colour2, colour3, amount);
    colour5 = blend(colour4, colour5, amount);
    colour7 = blend(colour6, colour7, amount);
    surface1 = blend(surface0, surface1, amount);

    return blend(distance0, distance1, amount) + blendSize * amount * (1.0f - amount);
}


/**
 * Smoothly blend the overlapping region of two objects. The
 * corresponding colour will be placed in colour1, and the corresponding
 * surface will be placed in surface1.
 *
 * @arg distance0: The first distance.
 * @arg distance1: The second distance.
 * @arg blendSize: The amount to blend between the objects.
 *
 * @returns: The nearest, modified distance.
 */
inline float smoothIntersection(
        const float distance0,
        const float distance1,
        const float blendSize)
{
    float amount = saturate(0.5f - 0.5f * (distance1 - distance0) / blendSize);
    return blend(distance0, distance1, amount) + blendSize * amount * (1.0f - amount);
}


/**
 * Compute the modified distance resulting from the interaction between
 * two objects. The corresponding colour will be placed in colour1, and
 * the corresponding surface will be placed in surface1.
 *
 * @arg modifications: The modification to perform:
 *     Each bit will enable a modification:
 *         bit 7: subtraction
 *         bit 8: intersection
 *         bit 9: smooth union
 *         bit 10: smooth subtraction
 *         bit 11: smooth intersection
 *     any other value will default to union.
 * @arg distance0: The first distance.
 * @arg distance1: The second distance.
 * @arg colour0: The first colour.
 * @arg colour1: The second colour.
 * @arg surface0: The first surface.
 * @arg surface1: The second surface.
 * @arg blendSize: The amount to blend between the objects.
 *
 * @returns: The nearest, modified distance.
 */
float performChildInteraction(
        const int modifications,
        const float distance0,
        const float distance1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &colour2,
        float4 &colour3,
        const float4 &colour4,
        float4 &colour5,
        const float4 &colour6,
        float4 &colour7,
        const float4 &surface0,
        float4 &surface1,
        const float blendSize)
{
    if (modifications & 128)
    {
        return subtraction(
            distance0,
            distance1,
            colour0,
            colour1,
            colour2,
            colour3,
            colour4,
            colour5,
            colour6,
            colour7,
            surface0,
            surface1
        );
    }
    if (modifications & 256)
    {
        return intersection(
            distance0,
            distance1,
            colour0,
            colour1,
            colour2,
            colour3,
            colour4,
            colour5,
            colour6,
            colour7,
            surface0,
            surface1
        );
    }
    if (modifications & 512)
    {
        return smoothUnion(
            distance0,
            distance1,
            colour0,
            colour1,
            colour2,
            colour3,
            colour4,
            colour5,
            colour6,
            colour7,
            surface0,
            surface1,
            blendSize
        );
    }
    if (modifications & 1024)
    {
        return smoothSubtraction(
            distance0,
            distance1,
            colour0,
            colour1,
            colour2,
            colour3,
            colour4,
            colour5,
            colour6,
            colour7,
            surface0,
            surface1,
            blendSize
        );
    }
    if (modifications & 2048)
    {
        return smoothIntersection(
            distance0,
            distance1,
            colour0,
            colour1,
            colour2,
            colour3,
            colour4,
            colour5,
            colour6,
            colour7,
            surface0,
            surface1,
            blendSize
        );
    }
    return union_(
        distance0,
        distance1,
        colour0,
        colour1,
        colour2,
        colour3,
        colour4,
        colour5,
        colour6,
        colour7,
        surface0,
        surface1
    );
}


/**
 * Compute the modified distance resulting from the interaction between
 * two objects. The corresponding colour will be placed in colour1, and
 * the corresponding surface will be placed in surface1.
 *
 * @arg modifications: The modification to perform:
 *     Each bit will enable a modification:
 *         bit 7: subtraction
 *         bit 8: intersection
 *         bit 9: smooth union
 *         bit 10: smooth subtraction
 *         bit 11: smooth intersection
 *     any other value will default to union.
 * @arg distance0: The first distance.
 * @arg distance1: The second distance.
 * @arg blendSize: The amount to blend between the objects.
 *
 * @returns: The nearest, modified distance.
 */
float performChildInteraction(
        const int modifications,
        const float distance0,
        const float distance1,
        const float blendSize)
{
    if (modifications & 128)
    {
        return subtraction(distance0, distance1);
    }
    if (modifications & 256)
    {
        return intersection(distance0, distance1);
    }
    if (modifications & 512)
    {
        return smoothUnion(distance0, distance1, blendSize);
    }
    if (modifications & 1024)
    {
        return smoothSubtraction(distance0, distance1, blendSize);
    }
    if (modifications & 2048)
    {
        return smoothIntersection(distance0, distance1, blendSize);
    }
    return union_(distance0, distance1);
}

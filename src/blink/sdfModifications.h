// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// SDF Modifiers
//
// These modify the ray position before computing the SD
//


/**
 * Infinitely repeat an object in the positive quadrant.
 *
 * @arg position: The position of the ray.
 * @arg spacing: The x, y, and z spacing with which to repeat the
 *     object.
 *
 * @returns: The modified ray position that results in repetion.
 */
inline float3 infiniteRepetition(const float3 &position, const float3 &spacing)
{
    return fmod(position + 0.5f * spacing, spacing) - 0.5f * spacing;
}


/**
 * Finitely repeat an object in the positive quadrant.
 *
 * @arg position: The position of the ray.
 * @arg limits: The x, y, and z limits of the repetition.
 * @arg spacing: The spacing with which to repeat the object.
 *
 * @returns: The modified ray position that results in repetion.
 */
inline float3 finiteRepetition(
        const float3 &position,
        const float3 &limits,
        const float spacing)
{
    const int3 intLimits = round_(limits);
    const int3 repeat = clamp_(round_(fabs(position) / spacing), -intLimits, intLimits);
    return position - spacing * float3(repeat.x, repeat.y, repeat.z);
}


/**
 * Modify a ray to elongate an object.
 *
 * @arg position: The position of the ray.
 * @arg elongation: The amount to elongate the object on each respective
 *     axis.
 *
 * @returns: The modified ray position that results in elongation.
 */
inline float3 elongate(const float3 &position, const float3 &elongation)
{
    return position - clamp(position, -elongation, elongation);
}


/**
 * Mirror the object in the yz-plane.
 *
 * @arg position: The position of the ray.
 *
 * @returns: The modified ray position that results in mirroring.
 */
inline float3 mirrorX(const float3 &position)
{
    return float3(fabs(position.x), position.y, position.z);
}


/**
 * Mirror the object in the xz-plane.
 *
 * @arg position: The position of the ray.
 *
 * @returns: The modified ray position that results in mirroring.
 */
inline float3 mirrorY(const float3 &position)
{
    return float3(position.x, fabs(position.y), position.z);
}


/**
 * Mirror the object in the xy-plane.
 *
 * @arg position: The position of the ray.
 *
 * @returns: The modified ray position that results in mirroring.
 */
inline float3 mirrorZ(const float3 &position)
{
    return float3(position.x, position.y, fabs(position.z));
}


/**
 * Round the edges of an object.
 *
 * @arg distance: The distance the ray has travelled.
 * @arg radius: The radius of the edges.
 *
 * @returns: The modified distnace that results in edge rounding.
 */
inline float roundEdges(const float distance, const float radius)
{
    return distance - radius;
}


/**
 * Round the edges of an object.
 *
 * @arg distance: The distance the ray has travelled.
 * @arg thickness: The thickness of the object's walls.
 *
 * @returns: The modified distnace that results in hollowing.
 */
inline float hollow(const float distance, const float thickness)
{
    return fabs(distance) - thickness;
}


/**
 * Modify the position of a ray, resulting in various effects.
 *
 * @arg modifications: The modifications to perform.
 *     Each bit will enable a modification:
 *         bit 0: finite repetition
 *         bit 1: infinite repetition
 *         bit 2: elongation
 *         bit 3: mirror x
 *         bit 4: mirror y
 *         bit 5: mirror z
 * @arg repetition: The values to use when repeating the ray.
 * @arg elongation: The values to use when elongating the ray.
 * @arg position: The position of the ray.
 */
void performShapeModification(
        const int modifications,
        const float4 &repetition,
        const float4 &elongation,
        float3 &position)
{
    if (modifications & 1)
    {
        position = finiteRepetition(
            position,
            float3(repetition.x, repetition.y, repetition.z),
            repetition.w
        );
    }
    else if (modifications & 2)
    {
        position = infiniteRepetition(
            position,
            float3(repetition.x, repetition.y, repetition.z)
        );
    }
    if (modifications & 4)
    {
        position = elongate(
            position,
            float3(elongation.x, elongation.y, elongation.z)
        );
    }
    if (modifications & 8)
    {
        position = mirrorX(position);
    }
    if (modifications & 16)
    {
        position = mirrorY(position);
    }
    if (modifications & 32)
    {
        position = mirrorZ(position);
    }
}


/**
 * Modify the distance a ray has travelled, resulting in various
 * effects.
 *
 * @arg modifications: The modifications to perform.
 *     Each bit will enable a modification:
 *         bit 6: hollowing
 * @arg edgeRadius: The radius to round the edges by.
 * @arg wallThickness: The thickness of the walls if hollowing the
 *     object.
 * @arg position: The position of the ray.
 */
float performDistanceModification(
        const int modifications,
        const float edgeRadius,
        const float wallThickness,
        const float distance)
{
    float result = distance;
    if (modifications & 64)
    {
        result = hollow(result, wallThickness);
    }
    return roundEdges(result, edgeRadius);
}

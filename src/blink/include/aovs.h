// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Functions for returning AOVs
//


/**
 *
 */
inline float4 earlyExitAOVs(
        const int aovType,
        const float3 &position,
        const float3 &normal,
        const float depth,
        const float objectId)
{
    if (aovType == 1)
    {
        return float4(position.x, position.y, position.z, objectId);
    }
    if (aovType == 2)
    {
        return float4(normal.x, normal.y, normal.z, objectId);
    }
    return float4(depth, 0, 0, objectId);
}


/**
 *
 */
inline float4 finalAOVs(
        const int aovType,
        const float iterations,
        const float bounces,
        const float objectId,
        const float4 &rayColour)
{
    if (aovType == 4)
    {
        return float4(iterations, bounces, 0, objectId);
    }
    return float4(rayColour.x, rayColour.y, rayColour.z, objectId);
}


/**
 *
 */
inline float4 rayMissAOVs(
        const int aovType,
        const float iterations,
        const float bounces,
        const float objectId)
{
    if (aovType == 4)
    {
        return float4(iterations, bounces, 0, objectId);
    }
    return float4(0);
}

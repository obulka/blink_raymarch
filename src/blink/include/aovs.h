// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Functions for returning AOVs
//

#define BEAUTY_AOV 0
#define WORLD_POSITION_AOV 1
#define LOCAL_POSITION_AOV 2
#define NORMAL_AOV 3
#define DEPTH_AOV 4
#define STATS_AOV 5


/**
 *
 */
inline float4 earlyExitAOVs(
        const int aovType,
        const float3 &worldPosition,
        const float3 &localPosition,
        const float3 &normal,
        const float depth,
        const float objectId)
{
    if (aovType == WORLD_POSITION_AOV)
    {
        return float4(worldPosition.x, worldPosition.y, worldPosition.z, objectId);
    }
    if (aovType == LOCAL_POSITION_AOV)
    {
        return float4(localPosition.x, localPosition.y, localPosition.z, objectId);
    }
    if (aovType == NORMAL_AOV)
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
    if (aovType == STATS_AOV)
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
    if (aovType == STATS_AOV)
    {
        return float4(iterations, bounces, 0, objectId);
    }
    return float4(0);
}

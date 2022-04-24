// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Handle Various Lights
//


inline void directionalLightData(
        const float3 &surfaceNormal,
        const float3 &light,
        const float maxRayDistance,
        float3 &lightDirection,
        float &distanceToLight)
{
    lightDirection = -light;
    distanceToLight = maxRayDistance;
}


inline void pointLightData(
        const float3 &pointOnSurface,
        const float3 &light,
        float3 &lightDirection,
        float &distanceToLight)
{
    lightDirection = light - pointOnSurface;
    distanceToLight = length(lightDirection);
}


inline void domeLightData(
        const float maxRayDistance,
        const float3 &light,
        float3 &lightDirection,
        float &distanceToLight)
{
    lightDirection = light;
    distanceToLight = maxRayDistance;
}


float getLightData(
        const float3 &pointOnSurface,
        const float3 &surfaceNormal,
        const float3 &light,
        const int lightType,
        const float intensity,
        const float falloff,
        const float maxRayDistance,
        float &distanceToLight,
        float3 &lightDirection)
{
    if (lightType == 1)
    {
        // directional
        directionalLightData(
            surfaceNormal,
            light,
            maxRayDistance,
            lightDirection,
            distanceToLight
        );
    }
    else if (lightType == 2)
    {
        // point
        pointLightData(
            pointOnSurface,
            light,
            lightDirection,
            distanceToLight
        );
    }
    else if (lightType == 100)
    {
        // dome
        domeLightData(
            maxRayDistance,
            light,
            lightDirection,
            distanceToLight
        );
    }

    return intensity / pow(1.0f + distanceToLight, falloff);
}

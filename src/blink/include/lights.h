// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Handle Various Lights
//


/**
 * Get the direction, and distance of a directional light.
 *
 * @arg direction: The direction the light is travelling.
 * @arg lightDirection: Will store the direction to the light.
 * @arg distanceToLight: Will store the distance to the light.
 */
inline void directionalLightData(
        const float3 &direction,
        const float maxRayDistance,
        float3 &lightDirection,
        float3 &lightNormal,
        float &distanceToLight,
        float &solidAngle)
{
    lightDirection = -direction;
    solidAngle = PI;
    distanceToLight = maxRayDistance;
    lightNormal = direction;
}


/**
 * Get the direction, and distance of a point light.
 *
 * @arg pointOnSurface: The point on the surface to compute the
 *     light intensity at.
 * @arg position: The position of the light.
 * @arg lightDirection: Will store the direction to the light.
 * @arg distanceToLight: Will store the distance to the light.
 */
inline void pointLightData(
        const float3 &pointOnSurface,
        const float3 &position,
        float3 &lightDirection,
        float3 &lightNormal,
        float &distanceToLight,
        float &solidAngle)
{
    lightDirection = position - pointOnSurface;
    solidAngle = 0.0f;
    distanceToLight = length(lightDirection);
    lightNormal = -lightDirection;
}


/**
 *
 */
inline float4 emissiveTerm(const float4 &emittance)
{
    return emittance * emittance.w;
}


/**
 *
 */
inline float geometryFactor(const float3 &incidentDirection, const float3 &surfaceNormal)
{
    return fabs(
        dot(surfaceNormal, incidentDirection)
    ) / dot2(incidentDirection);
}


/**
 *
 */
inline float sampleLightsPDF(const float numLights, const float solidAngle)
{
    if (solidAngle == 0.0f)
    {
        return 1.0f / numLights;
    }
    else
    {
        return 1.0f / numLights / solidAngle;
    }
}


/**
 * Get the direction, distance, and intensity of a light.
 *
 * @arg intensity: The light intensity.
 * @arg falloff: The power of the falloff of the light.
 * @arg distanceToLight: The distance to the light.
 *
 * @returns: The light intensity.
 */
inline float lightIntensity(
        const float intensity,
        const float falloff,
        const float distanceToLight)
{
    return intensity / pow(1.0f + distanceToLight, falloff);
}


/**
 * Get the direction, distance, and intensity of a light.
 *
 * @arg pointOnSurface: The point on the surface to compute the
 *     light intensity at.
 * @arg light: The light properties which depend on the light type.
 * @arg lightType: The type of light to compute the intensity for.
 *     0: directional
 *     1: point
 *     2: ambient
 *     3: ambient occlusion
 * @arg maxRayDistance: The maximum distance a ray can travel.
 * @arg distanceToLight: Will store the distance to the light.
 * @arg lightDirection: Will store the direction to the light.
 */
inline void getLightData(
        const float3 &pointOnSurface,
        const float3 &light,
        const int lightType,
        const float maxRayDistance,
        float &distanceToLight,
        float &solidAngle,
        float3 &lightDirection,
        float3 &lightNormal)
{
    if (lightType == 0)
    {
        // directional
        directionalLightData(
            light,
            maxRayDistance,
            lightDirection,
            lightNormal,
            distanceToLight,
            solidAngle
        );
    }
    else if (lightType == 1)
    {
        // point
        pointLightData(
            pointOnSurface,
            light,
            lightDirection,
            lightNormal,
            distanceToLight,
            solidAngle
        );
    }

    lightDirection = normalize(lightDirection);
}

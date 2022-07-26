// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Handle Various Lights
//


inline float4 multipleImportanceSample(
        const float4 &emittance,
        const float4 &throughput,
        const float pdf0,
        const float pdf1)
{
    return emittance * throughput * balanceHeuristic(pdf0, pdf1);
}


/**
 *
 */
float sampleEquiangularPDF(
        const float uniform,
        const float maxDistance,
        const float3 &rayOrigin,
        const float3 &rayDirection,
        const float3 &lightPosition,
        float &distance)
{
    // get coord of closest point to light along (infinite) ray
    const float delta = dot(lightPosition - rayOrigin, rayDirection);

    // get distance this point is from light
    const float D = length(rayOrigin + delta * rayDirection - lightPosition);

    if (D == 0.0f)
    {
        distance = 0.0f;
        return 1.0f;
    }

    // get angle of endpoints
    const float thetaA = atan2(-delta, D);
    const float thetaB = atan2(maxDistance - delta, D);

    // take sample
    const float t = D * tan(mix(thetaA, thetaB, uniform));

    distance = delta + t;

    if (thetaA != thetaB)
    {
        return D / ((thetaB - thetaA) * (D * D + t * t));
    }

    return 1.0f;
}


/**
 * Get the direction, and distance of a spherical light.
 *
 * @arg direction: The direction the light is travelling.
 * @arg lightDirection: Will store the direction to the light.
 * @arg distanceToLight: Will store the distance to the light.
 */
inline void hdriLightData(
        const float3 &seed,
        const float3 &surfaceNormal,
        float3 &lightDirection,
        float &distanceToLight)
{
    lightDirection = cosineDirectionInHemisphere(surfaceNormal, seed);
    distanceToLight = 1.0f;
}


/**
 * Get the direction, and distance of a spherical light.
 *
 * @arg direction: The direction the light is travelling.
 * @arg lightDirection: Will store the direction to the light.
 * @arg distanceToLight: Will store the distance to the light.
 */
inline void sphericalLightData(
        const float3 &seed,
        const float3 &pointOnSurface,
        const float3 &lightPosition,
        const float radius,
        float3 &lightDirection,
        float &distanceToLight,
        float &visibleSurfaceArea)
{
    visibleSurfaceArea = 2.0f * PI * radius * radius;
    float3 lightNormal = uniformDirectionInHemisphere(
        normalize(pointOnSurface - lightPosition),
        seed
    );
    lightDirection = lightPosition + lightNormal * radius - pointOnSurface;
    distanceToLight = length(lightDirection);
    lightDirection = normalize(lightDirection);
}



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
        float &distanceToLight,
        float &visibleSurfaceArea)
{
    visibleSurfaceArea = 2.0f * PI;
    distanceToLight = maxRayDistance;
    lightDirection = normalize(-direction);
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
        float &distanceToLight,
        float &visibleSurfaceArea)
{
    visibleSurfaceArea = 0.0f;
    lightDirection = position - pointOnSurface;
    distanceToLight = length(lightDirection);
    lightDirection = normalize(lightDirection);
}


/**
 *
 */
inline float sampleLightsPDF(const float numLights, const float visibleSurfaceArea)
{
    if (visibleSurfaceArea == 0.0f)
    {
        return 1.0f / numLights;
    }
    else
    {
        return 1.0f / numLights / visibleSurfaceArea;
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
    return intensity / pow(distanceToLight, falloff);
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
        float3 &lightDirection)
{
    if (lightType == 2)
    {
        // directional
        directionalLightData(
            light,
            maxRayDistance,
            lightDirection,
            distanceToLight,
            solidAngle
        );
    }
    else if (lightType == 3)
    {
        // point
        pointLightData(
            pointOnSurface,
            light,
            lightDirection,
            distanceToLight,
            solidAngle
        );
    }
}

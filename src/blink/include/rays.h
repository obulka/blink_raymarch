// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Ray math
//


/**
 * Reflect a ray off of a surface.
 *
 * @arg incidentRayDirection: The incident direction.
 * @arg surfaceNormalDirection: The normal to the surface.
 */
inline float3 reflectRayOffSurface(
    const float3 &incidentRayDirection,
    const float3 &surfaceNormalDirection)
{
    return (
        incidentRayDirection
        - 2 * dot(incidentRayDirection, surfaceNormalDirection) * surfaceNormalDirection
    );
}


/**
 * Refract a ray through a surface.
 *
 * @arg incidentRayDirection: The incident direction.
 * @arg surfaceNormalDirection: The normal to the surface.
 * @arg incidentRefractiveIndex: The refractive index the incident ray
 *     is travelling through.
 * @arg refractedRefractiveIndex: The refractive index the refracted ray
 *     will be travelling through.
 *
 * @returns: The refracted ray direction.
 */
inline float3 refractRayThroughSurface(
    const float3 &incidentRayDirection,
    const float3 &surfaceNormalDirection,
    const float incidentRefractiveIndex,
    const float refractedRefractiveIndex)
{
    const float refractiveRatio = incidentRefractiveIndex / refractedRefractiveIndex;
    const float cosIncident = -dot(incidentRayDirection, surfaceNormalDirection);
    const float sinTransmittedSquared = refractiveRatio * refractiveRatio * (
        1.0f - cosIncident * cosIncident
    );
    if (sinTransmittedSquared > 1.0f)
    {
        return reflectRayOffSurface(incidentRayDirection, surfaceNormalDirection);
    }
    const float cosTransmitted = sqrt(1.0f - sinTransmittedSquared);
    return normalize(
        refractiveRatio * incidentRayDirection
        + (refractiveRatio * cosIncident - cosTransmitted) * surfaceNormalDirection
    );
}


/**
 * Compute the schlick, simplified fresnel reflection coefficient.
 *
 * @arg incidentRayDirection: The incident direction.
 * @arg surfaceNormalDirection: The normal to the surface.
 * @arg incidentRefractiveIndex: The refractive index the incident ray
 *     is travelling through.
 * @arg refractedRefractiveIndex: The refractive index the refracted ray
 *     will be travelling through.
 *
 * @returns: The reflection coefficient.
 */
float schlickReflectionCoefficient(
    const float3 &incidentRayDirection,
    const float3 &surfaceNormalDirection,
    const float incidentRefractiveIndex,
    const float refractedRefractiveIndex)
{
    const float parallelCoefficient = pow(
        (incidentRefractiveIndex - refractedRefractiveIndex)
        / (incidentRefractiveIndex + refractedRefractiveIndex),
        2
    );
    float cosX = -dot(incidentRayDirection, surfaceNormalDirection);
    if (incidentRefractiveIndex > refractedRefractiveIndex)
    {
        const float refractiveRatio = incidentRefractiveIndex / refractedRefractiveIndex;
        const float sinTransmittedSquared = refractiveRatio * refractiveRatio * (
            1.0f - cosX * cosX
        );
        if (sinTransmittedSquared > 1.0f)
        {
            return 1.0f;
        }
        cosX = sqrt(1.0f - sinTransmittedSquared);
    }
    return parallelCoefficient + (1 - parallelCoefficient) * pow(1.0f - cosX, 5);
}


/**
 * Compute the fresnel reflection coefficient.
 *
 * @arg incidentRayDirection: The incident direction.
 * @arg surfaceNormalDirection: The normal to the surface.
 * @arg incidentRefractiveIndex: The refractive index the incident ray
 *     is travelling through.
 * @arg refractedRefractiveIndex: The refractive index the refracted ray
 *     will be travelling through.
 *
 * @returns: The reflection coefficient.
 */
float reflectionCoefficient(
    const float3 &incidentRayDirection,
    const float3 &surfaceNormalDirection,
    const float incidentRefractiveIndex,
    const float refractedRefractiveIndex)
{
    const float refractiveRatio = incidentRefractiveIndex / refractedRefractiveIndex;

    const float cosIncident = -dot(incidentRayDirection, surfaceNormalDirection);

    const float sinTransmittedSquared = refractiveRatio * refractiveRatio * (
        1.0f - cosIncident * cosIncident
    );

    if (sinTransmittedSquared > 1.0f)
    {
        return 1.0f;
    }

    const float cosTransmitted = sqrt(1.0f - sinTransmittedSquared);

    const float orthogonalReflectance = (
        (incidentRefractiveIndex * cosIncident - refractedRefractiveIndex * cosTransmitted)
        / (incidentRefractiveIndex * cosIncident + refractedRefractiveIndex * cosTransmitted)
    );

    const float parallelReflectance = (
        (refractedRefractiveIndex * cosIncident - incidentRefractiveIndex * cosTransmitted)
        / (refractedRefractiveIndex * cosIncident + incidentRefractiveIndex * cosTransmitted)
    );

    return (
        orthogonalReflectance * orthogonalReflectance
        + parallelReflectance * parallelReflectance
    ) / 2.0f;
}


/**
 * Generate a ray out of a camera.
 *
 * @arg cameraWorldMatrix: The camera matrix.
 * @arg inverseProjectionMatrix: The inverse of the projection matrix.
 * @arg uvPosition: The UV position in the resulting image.
 * @arg rayOrigin: Will store the origin of the ray.
 * @arg rayDirection: Will store the direction of the ray.
 */
void createCameraRay(
        const float4x4 &cameraWorldMatrix,
        const float4x4 &inverseProjectionMatrix,
        const float2 &uvPosition,
        float3 &rayOrigin,
        float3 &rayDirection)
{
    positionFromWorldMatrix(cameraWorldMatrix, rayOrigin);
    float4 direction;
    matmul(
        inverseProjectionMatrix,
        float4(uvPosition.x, uvPosition.y, 0, 1),
        direction
    );
    matmul(
        cameraWorldMatrix,
        float4(direction.x, direction.y, direction.z, 0),
        direction
    );
    rayDirection = normalize(float3(direction.x, direction.y, direction.z));
}

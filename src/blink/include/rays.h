// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Ray math
//

// Increase this if you want more than MAX_NESTED_DIELECTRICS nested
// transmissive objects
#define MAX_NESTED_DIELECTRICS 5


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
    return normalize(
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
    float cosX = -dot(surfaceNormalDirection, incidentRayDirection);
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
    return parallelCoefficient + (1.0f - parallelCoefficient) * pow(1.0f - cosX, 5);
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
    float4 direction = matmul(
        inverseProjectionMatrix,
        float4(uvPosition.x, uvPosition.y, 0, 1)
    );
    matmul(
        cameraWorldMatrix,
        float4(direction.x, direction.y, direction.z, 0),
        direction
    );
    rayDirection = normalize(float3(direction.x, direction.y, direction.z));
}


/**
 * Transform a ray's location.
 *
 * @arg rayOrigin: The location the ray originates from.
 * @arg position: The amount to translate the ray.
 * @arg rotation: The amount to rotate the ray (radians).
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
 *
 * @returns: The transformed ray origin.
 */
float3 transformRay(
    const float3 &rayOrigin,
    const float3 &translation,
    const float3 &rotation,
    const int modifications,
    const float4 &repetition,
    const float4 &elongation)
{
    float3x3 rotMatrix;
    rotationMatrix(rotation, rotMatrix);
    float3 transformedRay = matmul(
        rotMatrix.invert(),
        rayOrigin - translation
    );
    performShapeModification(
        modifications,
        repetition,
        elongation,
        transformedRay
    );

    return transformedRay;
}


/**
 *
 */
inline float numPathsToMarch(const float minPaths, const float maxPaths, const float3 &variance)
{
    return clamp(
        round(maxPaths * length(variance)),
        minPaths,
        maxPaths
    );
}


/**
 *
 */
inline float balanceHeuristic(const float pdf0, const float pdf1)
{
    return pdf0 / (pdf0 + pdf1);
}


/**
 *
 */
inline float4 emissiveTerm(const float4 &emittance, const float4 &brdf)
{
    return emittance * emittance.w * brdf;
}


/**
 *
 */
inline void specularBounce(
        const float4 &emittance,
        const float4 &specularity,
        const float3 &surfaceNormal,
        const float3 &diffuseDirection,
        const float roughness,
        const float specularProbability,
        const float offset,
        float4 &rayColour,
        float4 &brdf,
        float3 &direction,
        float3 &position)
{
    // Update the colour of the ray
    rayColour += emissiveTerm(emittance, brdf);

    const float3 specularDirection = reflectRayOffSurface(
        direction,
        surfaceNormal
    );

    direction = normalize(blend(
        diffuseDirection,
        specularDirection,
        roughness * roughness
    ));

    brdf *= specularity * specularProbability;

    // Offset the point so that it doesn't get trapped on the surface.
    position = offsetPoint(
        position,
        surfaceNormal,
        offset
    );
}


/**
 *
 */
inline void transmissiveBounce(
        const float4 &emittance,
        const float4 &transmittance,
        const float3 &surfaceNormal,
        const float3 &diffuseDirection,
        const float roughness,
        const float refractionProbability,
        const float offset,
        const float refractedRefractiveIndex,
        const float objectId,
        float4 &rayColour,
        float4 &brdf,
        float3 &direction,
        float3 &position,
        float nestedDielectrics[MAX_NESTED_DIELECTRICS][6],
        int &numNestedDielectrics,
        float &incidentRefractiveIndex,
        float &distanceTravelledThroughMaterial)
{
    const float3 refractedDirection = refractRayThroughSurface(
        direction,
        surfaceNormal,
        incidentRefractiveIndex,
        refractedRefractiveIndex
    );

    direction = normalize(blend(-diffuseDirection, refractedDirection, roughness * roughness));

    // Offset the point so that it doesn't get trapped on
    // surface.
    position = offsetPoint(position, -surfaceNormal, offset);

    const float4 absorptionColour = float4(
        nestedDielectrics[numNestedDielectrics][0],
        nestedDielectrics[numNestedDielectrics][1],
        nestedDielectrics[numNestedDielectrics][2],
        nestedDielectrics[numNestedDielectrics][3]
    );

    brdf *= exp(-absorptionColour * distanceTravelledThroughMaterial);

    // We are entering a new material so reset the distance
    distanceTravelledThroughMaterial = 0.0f;

    // We are passing through the surface, so update the refractive index
    incidentRefractiveIndex = refractedRefractiveIndex;
    if (nestedDielectrics[numNestedDielectrics][4] == objectId)
    {
        // We are exiting the material we are in, get the
        // last refractive index, by popping the stack
        numNestedDielectrics--;
    }
    else
    {
        // We are not exiting the material we are in, so push
        // the next refractive index to the stack
        numNestedDielectrics++;
        nestedDielectrics[numNestedDielectrics][0] = transmittance.x;
        nestedDielectrics[numNestedDielectrics][1] = transmittance.y;
        nestedDielectrics[numNestedDielectrics][2] = transmittance.z;
        nestedDielectrics[numNestedDielectrics][3] = transmittance.w;
        nestedDielectrics[numNestedDielectrics][4] = objectId;
        nestedDielectrics[numNestedDielectrics][5] = refractedRefractiveIndex;
    }

    // Update the colour of the ray
    rayColour += emissiveTerm(emittance, brdf);

    brdf *= refractionProbability;
}


/**
 *
 */
inline void diffuseBounce(
        const float4 &emittance,
        const float4 &diffusivity,
        const float3 &surfaceNormal,
        const float3 &diffuseDirection,
        const float diffuseProbability,
        const float offset,
        float4 &rayColour,
        float4 &brdf,
        float3 &direction,
        float3 &position)
{
    // Update the colour of the ray
    rayColour += emissiveTerm(emittance, brdf);

    direction = diffuseDirection;
    brdf *= diffusivity * diffuseProbability;

    // Offset the point so that it doesn't get trapped on
    // surface.
    position = offsetPoint(
        position,
        surfaceNormal,
        offset
    );
}

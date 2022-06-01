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
 *
 */
inline float balanceHeuristic(const float pdf0, const float pdf1)
{
    return pdf0 / (pdf0 + pdf1);
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
inline void getReflectivityData(
        const float3 &direction,
        const float3 &surfaceNormal,
        const float objectId,
        const float nestedDielectrics[MAX_NESTED_DIELECTRICS][6],
        const int numNestedDielectrics,
        const float surfaceRefractiveIndex,
        const float incidentRefractiveIndex,
        float &refractedRefractiveIndex,
        float &specularProbability,
        float &refractionProbability)
{
    if (nestedDielectrics[numNestedDielectrics][4] == objectId)
    {
        // We are exiting the material we are in, get the
        // last refractive index from the top of the stack
        refractedRefractiveIndex = nestedDielectrics[numNestedDielectrics - 1][5];
    }
    else
    {
        refractedRefractiveIndex = surfaceRefractiveIndex;
    }

    // Compute the refraction values
    const float reflectivity = schlickReflectionCoefficient(
        direction,
        surfaceNormal,
        incidentRefractiveIndex,
        refractedRefractiveIndex
    );

    const float initialSpecularProbability = specularProbability;

    specularProbability = (
        (specularProbability + refractionProbability > 0.0f)
        * blend(1.0f, specularProbability, reflectivity)
    );

    refractionProbability = (
        refractionProbability * (1.0f - specularProbability)
        / (1.0f - initialSpecularProbability)
    );
}


/**
 *
 */
inline float specularBounce(
        const float3 &incidentDirection,
        const float3 &surfaceNormal,
        const float4 &emittance,
        const float4 &specularity,
        const float3 &diffuseDirection,
        const float roughness,
        const float specularProbability,
        const float offset,
        float4 &emissiveColour,
        float4 &brdf,
        float3 &specularDirection,
        float3 &position)
{
    specularDirection = reflectRayOffSurface(
        incidentDirection,
        surfaceNormal
    );

    specularDirection = normalize(blend(
        diffuseDirection,
        specularDirection,
        roughness * roughness
    ));

    // Offset the point so that it doesn't get trapped on the surface.
    position = offsetPoint(
        position,
        surfaceNormal,
        offset
    );

    brdf = specularity;

    // Update the colour of the ray
    emissiveColour = emissiveTerm(emittance);

    return specularProbability * dot(specularDirection, surfaceNormal) / PI;
}


/**
 *
 */
inline float transmissiveBounce(
        const float3 &incidentDirection,
        const float3 &surfaceNormal,
        const float4 &emittance,
        const float4 &transmittance,
        const float3 &diffuseDirection,
        const float roughness,
        const float refractionProbability,
        const float offset,
        const float refractedRefractiveIndex,
        const float objectId,
        float4 &emissiveColour,
        float4 &brdf,
        float3 &refractedDirection,
        float3 &position,
        float nestedDielectrics[MAX_NESTED_DIELECTRICS][6],
        int &numNestedDielectrics,
        float &incidentRefractiveIndex,
        float &distanceTravelledThroughMaterial)
{
    refractedDirection = refractRayThroughSurface(
        incidentDirection,
        surfaceNormal,
        incidentRefractiveIndex,
        refractedRefractiveIndex
    );

    refractedDirection = normalize(blend(
        -diffuseDirection,
        refractedDirection,
        roughness * roughness
    ));

    // Offset the point so that it doesn't get trapped on
    // surface.
    position = offsetPoint(position, -surfaceNormal, offset);

    const float4 absorptionColour = float4(
        nestedDielectrics[numNestedDielectrics][0],
        nestedDielectrics[numNestedDielectrics][1],
        nestedDielectrics[numNestedDielectrics][2],
        nestedDielectrics[numNestedDielectrics][3]
    );

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

    brdf = exp(-absorptionColour * distanceTravelledThroughMaterial);

    // Update the colour of the ray
    emissiveColour = emissiveTerm(emittance);

    // We are entering a new material so reset the distance
    distanceTravelledThroughMaterial = 0.0f;

    return refractionProbability * fabs(dot(refractedDirection, surfaceNormal)) / PI;
}


/**
 *
 */
inline float diffuseBounce(
        const float3 &surfaceNormal,
        const float4 &emittance,
        const float4 &diffusivity,
        const float3 &diffuseDirection,
        const float diffuseProbability,
        const float offset,
        float4 &emissiveColour,
        float4 &brdf,
        float3 &direction,
        float3 &position)
{
    direction = diffuseDirection;

    // Offset the point so that it doesn't get trapped on
    // surface.
    position = offsetPoint(
        position,
        surfaceNormal,
        offset
    );

    brdf = diffusivity;

    // Update the colour of the ray
    emissiveColour = emissiveTerm(emittance);

    return diffuseProbability * dot(direction, surfaceNormal) / PI;
}


/**
 *
 */
inline float sampleMaterial(
        const float3 &seed,
        const float3 &surfaceNormal,
        const float3 &incidentDirection,
        const float4 &diffusivity,
        const float reflectionOffset,
        const float transmissionOffset,
        const float4 &transmittance,
        const float surfaceRefractiveIndex,
        const float transmissiveRoughness,
        const float4 &specularity,
        const float specularRoughness,
        const float4 &emittance,
        const float objectId,
        float4 &emissiveColour,
        float4 &brdf,
        float3 &outgoingDirection,
        float3 &position,
        float nestedDielectrics[MAX_NESTED_DIELECTRICS][6],
        int &numNestedDielectrics,
        float &incidentRefractiveIndex,
        float &distanceTravelledThroughMaterial)
{
    // Get the diffuse direction for the next ray
    const float3 diffuseDirection = cosineDirectionInHemisphere(
        surfaceNormal,
        seed
    );

    const float rng = random(random(seed.x) + random(seed.y) + random(seed.z));

    float refractedRefractiveIndex;
    float specularProbability = specularity.w;
    float refractionProbability = transmittance.w;

    if (specularProbability > 0.0f || refractionProbability > 0.0f)
    {
        getReflectivityData(
            incidentDirection,
            surfaceNormal,
            objectId,
            nestedDielectrics,
            numNestedDielectrics,
            surfaceRefractiveIndex,
            incidentRefractiveIndex,
            refractedRefractiveIndex,
            specularProbability,
            refractionProbability
        );
    }

    float pdf;

    // Maybe reflect the ray
    if (specularProbability > 0.0f && rng <= specularProbability)
    {
        pdf = specularBounce(
            incidentDirection,
            surfaceNormal,
            emittance,
            specularity,
            diffuseDirection,
            specularRoughness,
            specularProbability,
            reflectionOffset,
            emissiveColour,
            brdf,
            outgoingDirection,
            position
        );
    }
    // Maybe refract the ray
    else if (
        transmittance.w > 0.0f
        && rng <= specularProbability + refractionProbability
    ) {
        pdf = transmissiveBounce(
            incidentDirection,
            surfaceNormal,
            emittance,
            transmittance,
            diffuseDirection,
            transmissiveRoughness,
            refractionProbability,
            transmissionOffset,
            refractedRefractiveIndex,
            objectId,
            emissiveColour,
            brdf,
            outgoingDirection,
            position,
            nestedDielectrics,
            numNestedDielectrics,
            incidentRefractiveIndex,
            distanceTravelledThroughMaterial
        );
    }
    // Otherwise diffuse the light
    else
    {
        pdf = diffuseBounce(
            surfaceNormal,
            emittance,
            diffusivity,
            diffuseDirection,
            1.0f - specularProbability - refractionProbability,
            reflectionOffset,
            emissiveColour,
            brdf,
            outgoingDirection,
            position
        );
    }

    return pdf;
}

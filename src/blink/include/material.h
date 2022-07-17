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
#define MAX_NESTED_DIELECTRICS 4
#define NESTED_DIELECTRIC_PARAMS 9
#define EXTINCTION_X 0
#define EXTINCTION_Y 1
#define EXTINCTION_Z 2
#define OBJECT_ID 3
#define REFRACTIVE_INDEX 4
#define SCATTERING_X 5
#define SCATTERING_Y 6
#define SCATTERING_Z 7
#define DO_REFRACTION 8


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
inline float geometryFactor(
        const float3 &incidentDirection,
        const float3 &surfaceNormal,
        const float distance)
{
    return fabs(
        dot(surfaceNormal, incidentDirection)
    ) / distance / distance;
}



/**
 *
 */
inline void getReflectivityData(
        const float3 &direction,
        const float3 &surfaceNormal,
        const float objectId,
        const float nestedDielectrics[MAX_NESTED_DIELECTRICS][NESTED_DIELECTRIC_PARAMS],
        const int numNestedDielectrics,
        const bool isExiting,
        const float surfaceRefractiveIndex,
        const float4 &surfaceScatteringCoefficient,
        const float incidentRefractiveIndex,
        float &refractedRefractiveIndex,
        float4 &refractedScatteringCoefficient,
        float &specularProbability,
        float &refractionProbability)
{
    if (isExiting)
    {
        // We are exiting the material we are in, get the
        // last refractive index from the top of the stack
        refractedRefractiveIndex = nestedDielectrics[numNestedDielectrics - 1][REFRACTIVE_INDEX];
        refractedScatteringCoefficient = float4(
            nestedDielectrics[numNestedDielectrics - 1][SCATTERING_X],
            nestedDielectrics[numNestedDielectrics - 1][SCATTERING_Y],
            nestedDielectrics[numNestedDielectrics - 1][SCATTERING_Z],
            0
        );
    }
    else
    {
        // Otherwise we will be entering a new material
        refractedRefractiveIndex = surfaceRefractiveIndex;
        refractedScatteringCoefficient = surfaceScatteringCoefficient;
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
        const float3 &diffuseDirection,
        const float roughness,
        const float offset,
        float3 &idealSpecularDirection,
        float3 &specularDirection,
        float3 &position)
{
    idealSpecularDirection = reflectRayOffSurface(
        incidentDirection,
        surfaceNormal
    );

    specularDirection = normalize(blend(
        diffuseDirection,
        idealSpecularDirection,
        roughness
    ));

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
inline float sampleSpecular(
        const float3 &idealSpecularDirection,
        const float3 &specularDirection,
        const float3 &lightDirection,
        const float4 &emittance,
        const float4 &specularity,
        const float specularProbability,
        float4 &emissiveColour,
        float4 &materialBRDF,
        float4 &lightBRDF,
        float &lightPDF)
{
    materialBRDF = specularity;
    lightBRDF = specularity;

    // Update the colour of the ray
    emissiveColour = emittance;

    const float probabilityOverPi = specularProbability / PI;

    lightPDF = 0.0f;

    return probabilityOverPi * dot(idealSpecularDirection, specularDirection);
}


/**
 *
 */
inline void transmissiveBounce(
        const float3 &incidentDirection,
        const float3 &surfaceNormal,
        const float3 &diffuseDirection,
        const float roughness,
        const float offset,
        const float incidentRefractiveIndex,
        const float refractedRefractiveIndex,
        const bool doRefraction,
        float3 &idealRefractedDirection,
        float3 &refractedDirection,
        float3 &position)
{
    if (doRefraction)
    {
        idealRefractedDirection = refractRayThroughSurface(
            incidentDirection,
            surfaceNormal,
            incidentRefractiveIndex,
            refractedRefractiveIndex
        );
    }
    else
    {
        idealRefractedDirection = incidentDirection;
    }

    refractedDirection = normalize(
        blend(-diffuseDirection, idealRefractedDirection, roughness)
    );

    // Offset the point so that it doesn't get trapped on
    // surface.
    position = offsetPoint(
        position,
        -surfaceNormal + refractedDirection,
        offset
    );
}


/**
 *
 */
inline float sampleTransmissive(
        const bool doExtinction,
        const float3 &idealRefractedDirection,
        const float3 &refractedDirection,
        const float4 &emittance,
        const float4 &transmittance,
        const float refractionProbability,
        const float refractedRefractiveIndex,
        const float4 &refractedScatteringCoefficient,
        const float objectId,
        const float distanceToLight,
        const bool doRefraction,
        const bool isExiting,
        float4 &emissiveColour,
        float4 &materialBRDF,
        float4 &lightBRDF,
        float &lightPDF,
        float nestedDielectrics[MAX_NESTED_DIELECTRICS][NESTED_DIELECTRIC_PARAMS],
        int &numNestedDielectrics,
        float &incidentRefractiveIndex,
        float4 &scatteringCoefficient,
        float &distanceTravelledThroughMaterial)
{
    const float4 extinctionCoefficient = float4(
        nestedDielectrics[numNestedDielectrics][EXTINCTION_X],
        nestedDielectrics[numNestedDielectrics][EXTINCTION_Y],
        nestedDielectrics[numNestedDielectrics][EXTINCTION_Z],
        0
    );

    // We are passing through the surface, so update the refractive index
    incidentRefractiveIndex = refractedRefractiveIndex;
    if (isExiting)
    {
        // We are exiting the material we are in, get the
        // last refractive index, by popping the stack
        numNestedDielectrics--;
    }
    else
    {
        // We are not exiting the material we are in, we are entering
        // a new one, so push the next refractive index to the stack
        numNestedDielectrics++;
        nestedDielectrics[numNestedDielectrics][EXTINCTION_X] = transmittance.x;
        nestedDielectrics[numNestedDielectrics][EXTINCTION_Y] = transmittance.y;
        nestedDielectrics[numNestedDielectrics][EXTINCTION_Z] = transmittance.z;
        nestedDielectrics[numNestedDielectrics][OBJECT_ID] = objectId;
        nestedDielectrics[numNestedDielectrics][REFRACTIVE_INDEX] = refractedRefractiveIndex;
        nestedDielectrics[numNestedDielectrics][SCATTERING_X] = refractedScatteringCoefficient.x;
        nestedDielectrics[numNestedDielectrics][SCATTERING_Y] = refractedScatteringCoefficient.y;
        nestedDielectrics[numNestedDielectrics][SCATTERING_Z] = refractedScatteringCoefficient.z;
        nestedDielectrics[numNestedDielectrics][DO_REFRACTION] = doRefraction;
    }

    materialBRDF = exp(
        -extinctionCoefficient
        * distanceTravelledThroughMaterial
        * doExtinction
    );
    lightBRDF = exp(
        -extinctionCoefficient
        * distanceToLight
        * doExtinction
    );

    scatteringCoefficient = refractedScatteringCoefficient;

    // Update the colour of the ray
    emissiveColour = emittance;

    // We are entering a new material so reset the distance
    distanceTravelledThroughMaterial = 0.0f;

    const float probabilityOverPi = refractionProbability / PI;

    lightPDF = 0.0f;

    return probabilityOverPi * dot(idealRefractedDirection, refractedDirection);
}


/**
 *
 */
inline void diffuseBounce(
        const float3 &surfaceNormal,
        const float3 &diffuseDirection,
        const float offset,
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
}


/**
 *
 */
inline float sampleDiffuse(
        const float3 &surfaceNormal,
        const float4 &emittance,
        const float4 &diffusivity,
        const float3 &diffuseDirection,
        const float3 &lightDirection,
        const float diffuseProbability,
        float4 &emissiveColour,
        float4 &materialBRDF,
        float4 &lightBRDF,
        float &lightPDF)
{
    materialBRDF = diffusivity;

    lightBRDF = materialBRDF;

    // Update the colour of the ray
    emissiveColour = emittance;

    const float probabilityOverPi = diffuseProbability / PI;

    lightPDF = probabilityOverPi * saturate(dot(lightDirection, surfaceNormal));
    return probabilityOverPi * dot(diffuseDirection, surfaceNormal);
}


/**
 *
 */
inline float sampleMaterial(
        const float3 &seed,
        const bool doExtinction,
        const float3 &surfaceNormal,
        const float3 &incidentDirection,
        const float3 &lightDirection,
        const float4 &diffusivity,
        const float reflectionOffset,
        const float transmissionOffset,
        const float4 &transmittance,
        const bool doRefraction,
        const float surfaceRefractiveIndex,
        const float4 &surfaceScatteringCoefficient,
        const float transmissiveRoughness,
        const float4 &specularity,
        const float specularRoughness,
        const float4 &emittance,
        const float objectId,
        const float distanceToLight,
        const bool isExiting,
        float4 &emissiveColour,
        float4 &materialBRDF,
        float4 &lightBRDF,
        float3 &outgoingDirection,
        float3 &position,
        float nestedDielectrics[MAX_NESTED_DIELECTRICS][NESTED_DIELECTRIC_PARAMS],
        int &numNestedDielectrics,
        float &incidentRefractiveIndex,
        float4 &scatteringCoefficient,
        float &distanceTravelledThroughMaterial,
        float &lightPDF)
{
    // Get the diffuse direction for the next ray
    const float3 diffuseDirection = cosineDirectionInHemisphere(
        surfaceNormal,
        seed
    );

    const float rng = random(random(seed.x) + random(seed.y) + random(seed.z));

    float refractedRefractiveIndex;
    float4 refractedScatteringCoefficient;
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
            isExiting,
            surfaceRefractiveIndex,
            surfaceScatteringCoefficient,
            incidentRefractiveIndex,
            refractedRefractiveIndex,
            refractedScatteringCoefficient,
            specularProbability,
            refractionProbability
        );
    }
    if (!doRefraction)
    {
        specularProbability = specularity.w;
        refractionProbability = transmittance.w;
    }

    float pdf;

    // Maybe reflect the ray
    if (specularProbability > 0.0f && rng <= specularProbability)
    {
        const float roughness = specularRoughness * specularRoughness;
        float3 idealSpecularDirection;
        specularBounce(
            incidentDirection,
            surfaceNormal,
            diffuseDirection,
            roughness,
            reflectionOffset,
            idealSpecularDirection,
            outgoingDirection,
            position
        );

        pdf = sampleSpecular(
            idealSpecularDirection,
            outgoingDirection,
            lightDirection,
            emittance,
            specularity,
            specularProbability,
            emissiveColour,
            materialBRDF,
            lightBRDF,
            lightPDF
        );
    }
    // Maybe refract the ray
    else if (
        transmittance.w > 0.0f
        && rng <= specularProbability + refractionProbability
    ) {
        const float roughness = transmissiveRoughness * transmissiveRoughness;
        float3 idealRefractedDirection;
        transmissiveBounce(
            incidentDirection,
            surfaceNormal,
            diffuseDirection,
            roughness,
            transmissionOffset,
            incidentRefractiveIndex,
            refractedRefractiveIndex,
            doRefraction,
            idealRefractedDirection,
            outgoingDirection,
            position
        );

        pdf = sampleTransmissive(
            doExtinction,
            idealRefractedDirection,
            outgoingDirection,
            emittance,
            transmittance,
            refractionProbability,
            refractedRefractiveIndex,
            refractedScatteringCoefficient,
            objectId,
            distanceToLight,
            doRefraction,
            isExiting,
            emissiveColour,
            materialBRDF,
            lightBRDF,
            lightPDF,
            nestedDielectrics,
            numNestedDielectrics,
            incidentRefractiveIndex,
            scatteringCoefficient,
            distanceTravelledThroughMaterial
        );

        if (doRefraction)
        {
            materialBRDF *= fabs(dot(outgoingDirection, surfaceNormal));
        }
    }
    // Otherwise diffuse the light
    else
    {
        diffuseBounce(
            surfaceNormal,
            diffuseDirection,
            reflectionOffset,
            outgoingDirection,
            position
        );

        pdf = sampleDiffuse(
            surfaceNormal,
            emittance,
            diffusivity,
            outgoingDirection,
            lightDirection,
            1.0f - specularProbability - refractionProbability,
            emissiveColour,
            materialBRDF,
            lightBRDF,
            lightPDF
        );
        materialBRDF *= fabs(dot(outgoingDirection, surfaceNormal));
    }

    return pdf;
}


/**
 *
 */
void useNoiseOnMaterial(
        const int noiseOptions,
        const float noiseValue,
        float4 &diffusivity,
        float4 &specularity,
        float4 &transmittance,
        float4 &emittance,
        float &transmissiveRoughness,
        float &refractiveIndex)
{
    if (noiseOptions & 4)
    {
        diffusivity.x *= noiseValue;
        diffusivity.y *= noiseValue;
        diffusivity.z *= noiseValue;
    }
    if (noiseOptions & 8)
    {
        specularity.w *= noiseValue;
    }
    if (noiseOptions & 16)
    {
        transmittance.w *= noiseValue;
    }
    if (noiseOptions & 32)
    {
        emittance *= noiseValue;
    }
    if (noiseOptions & 64)
    {
        refractiveIndex = (refractiveIndex - 1.0f) * noiseValue + 1.0f;
    }
    if (noiseOptions & 128)
    {
        transmissiveRoughness *= noiseValue;
    }
    if (noiseOptions & 256)
    {
        diffusivity.w *= noiseValue;
    }
}


/**
 *
 */
void useNoiseOnEmittance(
        const int noiseOptions,
        const float noiseValue,
        float4 &emittance)
{
    if (noiseOptions & 32)
    {
        emittance *= noiseValue;
    }
}

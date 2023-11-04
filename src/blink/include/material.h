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
#define MAX_NESTED_DIELECTRICS 8

// Number of parameters needed for nested dielectrics
#define NESTED_DIELECTRIC_PARAMS 9

// Indices for the nested dielectrics
#define EXTINCTION_X 0
#define EXTINCTION_Y 1
#define EXTINCTION_Z 2
#define OBJECT_ID 3
#define REFRACTIVE_INDEX 4
#define SCATTERING_X 5
#define SCATTERING_Y 6
#define SCATTERING_Z 7
#define DO_REFRACTION 8

#define NOISE_ENABLED 1
#define TURBULENCE_NOISE 2
#define DIFFUSE_NOISE 4
#define SPECULAR_NOISE 8
#define TRANSMITTANCE_NOISE 16
#define EMITTANCE_NOISE 32
#define REFRACTIVE_INDEX_NOISE 64
#define TRANSMISSION_ROUGHNESS_NOISE 128
#define SPECULAR_ROUGHNESS_NOISE 256
#define INVERT_NOISE 512
#define SCATTERING_NOISE 1024
#define EXTINCTION_NOISE 2048


/**
 * Get the extinction coefficient from the nested dielectric stack.
 *
 * @arg nestedDielectrics: The stack of dielectrics that we have
 *     entered without exiting.
 * @arg index: The index of the dielectric to get the extinction
 *     coefficient of.
 *
 * @returns: The extinction coefficient.
 */
inline float4 getExtinctionCoefficient(
        const float nestedDielectrics[MAX_NESTED_DIELECTRICS][NESTED_DIELECTRIC_PARAMS],
        const int index)
{
    return float4(
        nestedDielectrics[index][EXTINCTION_X],
        nestedDielectrics[index][EXTINCTION_Y],
        nestedDielectrics[index][EXTINCTION_Z],
        0
    );
}


/**
 * Get the scattering coefficient from the nested dielectric stack.
 *
 * @arg nestedDielectrics: The stack of dielectrics that we have
 *     entered without exiting.
 * @arg index: The index of the dielectric to get the scattering
 *     coefficient of.
 *
 * @returns: The scattering coefficient.
 */
inline float4 getScatteringCoefficient(
        const float nestedDielectrics[MAX_NESTED_DIELECTRICS][NESTED_DIELECTRIC_PARAMS],
        const int index)
{
    return float4(
        nestedDielectrics[index][SCATTERING_X],
        nestedDielectrics[index][SCATTERING_Y],
        nestedDielectrics[index][SCATTERING_Z],
        0
    );
}


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
 * Compute the factor which, due to surface geometry and distance from a
 * light source, will scale the brightness.
 *
 * @arg incidentDirection: The incident direction.
 * @arg surfaceNormal: The normal to the surface.
 * @arg distance: The distance to the light.
 *
 * @returns: The value to scale the brightness by.
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
 * Modify the reflectance/transmittance based on surface geometry and
 * ray direction.
 *
 * @arg direction: The incoming ray direction.
 * @arg surfaceNormal: The normal to the surface at the position we
 *     are sampling the material of.
 * @arg objectId: The ID of the object whose material we are sampling.
 * @arg nestedDielectrics: The stack of dielectrics that we have
 *     entered without exiting.
 * @arg numNestedDielectrics: The number of dielectrics in the
 *     stack.
 * @arg isExiting: Whether or not we will be exiting the current
 *     material if we transmit through it.
 * @arg surfaceRefractiveIndex: The refractive index of the material.
 * @arg surfaceExtinctionCoefficient: The extinction coefficient of the
 *     material.
 * @arg surfaceScatteringCoefficient: The scattering coefficient of the
 *     material.
 * @arg incidentRefractiveIndex: The refractive index of the material
 *     the ray is currently travelling through.
 * @arg refractedRefractiveIndex: The refractive index of the material
 *     we will enter.
 * @arg refractedExtinctionCoefficient: The extinction coefficient of
 *     the material we would enter.
 * @arg refractedScatteringCoefficient: The scattering coefficient of
 *     the material we would enter.
 * @arg specularProbability: The probability that we would specularly
 *     reflect off this surface.
 * @arg refractionProbability: The probability that we would transmit
 *     through this surface.
 */
inline void getReflectivityData(
        const float3 &direction,
        const float3 &surfaceNormal,
        const float objectId,
        const float nestedDielectrics[MAX_NESTED_DIELECTRICS][NESTED_DIELECTRIC_PARAMS],
        const int numNestedDielectrics,
        const bool isExiting,
        const float surfaceRefractiveIndex,
        const float4 &surfaceExtinctionCoefficient,
        const float4 &surfaceScatteringCoefficient,
        float &incidentRefractiveIndex,
        float &refractedRefractiveIndex,
        float4 &refractedExtinctionCoefficient,
        float4 &refractedScatteringCoefficient,
        float &specularProbability,
        float &refractionProbability)
{
    incidentRefractiveIndex = nestedDielectrics[numNestedDielectrics][REFRACTIVE_INDEX];

    if (isExiting)
    {
        // We are exiting the material we are in, get the
        // last refractive index from the top of the stack
        refractedExtinctionCoefficient = getExtinctionCoefficient(
            nestedDielectrics,
            numNestedDielectrics - 1
        );
        refractedRefractiveIndex = nestedDielectrics[numNestedDielectrics - 1][REFRACTIVE_INDEX];
        refractedScatteringCoefficient = getScatteringCoefficient(
            nestedDielectrics,
            numNestedDielectrics - 1
        );
    }
    else
    {
        // Otherwise we will be entering a new material
        refractedExtinctionCoefficient = surfaceExtinctionCoefficient;
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
 * Perform a specular bounce of the ray.
 *
 * @arg incidentDirection: The incoming ray direction.
 * @arg surfaceNormal: The normal to the surface at the position we
 *     are sampling the material of.
 * @arg diffuseDirection: The direction the ray would travel after
 *     a diffuse bounce.
 * @arg roughness: The specular roughness of the material.
 * @arg offset: The amount to offset the ray in order to escape the
 *     surface.
 * @arg idealSpecularDirection: The direction the ray would travel
 *     after specular reflection if there were no roughness.
 * @arg specularDirection: The location to store the new ray direction.
 * @arg position: The location to store the new ray origin.
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
 * Perform specular material sampling.
 *
 * @arg idealRefractedDirection: The direction the ray would travel
 *     after refraction if there were no roughness.
 * @arg specularDirection: The direction the ray will travel.
 * @arg specularity: The specular values of the surface.
 * @arg specularProbability: The probability that we would specularly
 *     reflect off this surface.
 * @arg materialBRDF: The BRDF of the surface at the position we
 *     are sampling the material of.
 * @arg lightPDF: The PDF of the material we are sampling from the
 *     perspective of the light we will be sampling.
 *
 * @returns: The material PDF.
 */
inline float sampleSpecular(
        const float3 &idealSpecularDirection,
        const float3 &specularDirection,
        const float4 &specularity,
        const float specularProbability,
        float4 &materialBRDF,
        float &lightPDF)
{
    materialBRDF = specularity;

    const float probabilityOverPi = specularProbability / PI;

    lightPDF = 0.0f;

    return probabilityOverPi * dot(idealSpecularDirection, specularDirection);
}


/**
 * Perform a transmissive bounce of the ray.
 *
 * @arg incidentDirection: The incoming ray direction.
 * @arg surfaceNormal: The normal to the surface at the position we
 *     are sampling the material of.
 * @arg diffuseDirection: The direction the ray would travel after
 *     a diffuse bounce.
 * @arg roughness: The transmissive roughness of the material.
 * @arg offset: The amount to offset the ray in order to escape the
 *     surface.
 * @arg incidentRefractiveIndex: The refractive index of the material
 *     the ray is currently travelling through.
 * @arg refractedRefractiveIndex: The refractive index of the material
 *     we will enter.
 * @arg doRefraction: Whether or not refraction is enabled on the
 *     material.
 * @arg idealRefractedDirection: The direction the ray would travel
 *     after refraction if there were no roughness.
 * @arg refractedDirection: The location to store the new ray direction.
 * @arg position: The location to store the new ray origin.
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
 * Perform transmissive material sampling.
 *
 * @arg idealRefractedDirection: The direction the ray would travel
 *     after refraction if there were no roughness.
 * @arg refractedDirection: The direction the ray will travel.
 * @arg refractionProbability: The probability that we would transmit
 *     through this surface.
 * @arg refractedRefractiveIndex: The refractive index of the material.
 * @arg refractedExtinctionCoefficient: The extinction coefficient of
 *     the material.
 * @arg refractedScatteringCoefficient: The scattering coefficient of
 *     the material.
 * @arg objectId: The ID of the object whose material we are sampling.
 * @arg doRefraction: Whether or not refraction is enabled on the
 *     material.
 * @arg isExiting: Whether or not we will be exiting the current
 *     material if we transmit through it.
 * @arg materialBRDF: The BRDF of the surface at the position we
 *     are sampling the material of.
 * @arg lightPDF: The PDF of the material we are sampling from the
 *     perspective of the light we will be sampling.
 * @arg nestedDielectrics: The stack of dielectrics that we have
 *     entered without exiting.
 * @arg numNestedDielectrics: The number of dielectrics in the
 *     stack.
 *
 * @returns: The material PDF.
 */
inline float sampleTransmissive(
        const float3 &idealRefractedDirection,
        const float3 &refractedDirection,
        const float refractionProbability,
        const float refractedRefractiveIndex,
        const float4 &refractedExtinctionCoefficient,
        const float4 &refractedScatteringCoefficient,
        const float objectId,
        const bool doRefraction,
        const bool isExiting,
        float4 &materialBRDF,
        float &lightPDF,
        float nestedDielectrics[MAX_NESTED_DIELECTRICS][NESTED_DIELECTRIC_PARAMS],
        int &numNestedDielectrics)
{
    // We are passing through the surface
    if (isExiting)
    {
        // We are exiting the material we are in, pop the stack
        numNestedDielectrics--;
    }
    else
    {
        // We are not exiting the material we are in, we are entering
        // a new one, so push the next refractive index to the stack
        numNestedDielectrics++;
        nestedDielectrics[numNestedDielectrics][EXTINCTION_X] = refractedExtinctionCoefficient.x;
        nestedDielectrics[numNestedDielectrics][EXTINCTION_Y] = refractedExtinctionCoefficient.y;
        nestedDielectrics[numNestedDielectrics][EXTINCTION_Z] = refractedExtinctionCoefficient.z;
        nestedDielectrics[numNestedDielectrics][OBJECT_ID] = objectId;
        nestedDielectrics[numNestedDielectrics][REFRACTIVE_INDEX] = refractedRefractiveIndex;
        nestedDielectrics[numNestedDielectrics][SCATTERING_X] = refractedScatteringCoefficient.x;
        nestedDielectrics[numNestedDielectrics][SCATTERING_Y] = refractedScatteringCoefficient.y;
        nestedDielectrics[numNestedDielectrics][SCATTERING_Z] = refractedScatteringCoefficient.z;
        nestedDielectrics[numNestedDielectrics][DO_REFRACTION] = doRefraction;
    }

    const float probabilityOverPi = refractionProbability / PI;

    materialBRDF = float4(1);
    lightPDF = 0.0f;

    return probabilityOverPi * dot(idealRefractedDirection, refractedDirection);
}


/**
 * Perform a diffuse bounce of the ray.
 *
 * @arg surfaceNormal: The normal to the surface at the position we
 *     are sampling the material of.
 * @arg diffuseDirection: The direction the ray will travel after
 *     sampling the material.
 * @arg offset: The amount to offset the ray in order to escape the
 *     surface.
 * @arg direction: The location to store the new ray direction.
 * @arg position: The location to store the new ray origin.
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
 * Perform diffuse material sampling.
 *
 * @arg surfaceNormal: The normal to the surface at the position we
 *     are sampling the material of.
 * @arg diffusivity: The diffuse values of the surface.
 * @arg diffuseDirection: The direction the ray will travel after
 *     sampling the material.
 * @arg diffuseProbability: The probability of doing a diffuse bounce
 *     on this material.
 * @arg materialBRDF: The BRDF of the surface at the position we
 *     are sampling the material of.
 * @arg lightPDF: The PDF of the material we are sampling from the
 *     perspective of the light we will be sampling.
 *
 * @returns: The material PDF.
 */
inline float sampleDiffuse(
        const float3 &surfaceNormal,
        const float4 &diffusivity,
        const float3 &diffuseDirection,
        const float diffuseProbability,
        float4 &materialBRDF,
        float &lightPDF)
{
    materialBRDF = diffusivity;
    const float probabilityOverPi = diffuseProbability / PI;
    lightPDF = probabilityOverPi;
    return probabilityOverPi * dot(diffuseDirection, surfaceNormal);
}


/**
 * Perform material sampling.
 *
 * @arg seed: The seed to use in randomization.
 * @arg surfaceNormal: The normal to the surface at the position we
 *     are sampling the material of.
 * @arg incidentDirection: The incoming ray direction.
 * @arg diffusivity: The diffuse values of the surface.
 * @arg offset: The amount to offset the ray in order to escape the
 *     surface.
 * @arg transmittance: The extinction coefficient and transmissive
 *     probability of the surface.
 * @arg doRefraction: Whether or not refraction is enabled on the
 *     material.
 * @arg surfaceRefractiveIndex: The refractive index of the material.
 * @arg surfaceScatteringCoefficient: The scattering coefficient of the
 *     material.
 * @arg transmissionRoughness: The transmissive roughness of the
 *     surface.
 * @arg specularity: The specular values of the surface.
 * @arg specularRoughness: The specular roughness of the surface.
 * @arg objectId: The ID of the object whose material we are sampling.
 * @arg isExiting: Whether or not we will be exiting the current
 *     material if we transmit through it.
 * @arg materialBRDF: The BRDF of the surface at the position we
 *     are sampling the material of.
 * @arg outgoingDirection: The direction the ray will travel after
 *     sampling the material.
 * @arg position: The position on the surface to sample the
 *     material of.
 * @arg nestedDielectrics: The stack of dielectrics that we have
 *     entered without exiting.
 * @arg numNestedDielectrics: The number of dielectrics in the
 *     stack.
 * @arg lightPDF: The PDF of the material we are sampling from the
 *     perspective of the light we will be sampling.
 *
 * @returns: The material PDF.
 */
inline float sampleMaterial(
        const float3 &seed,
        const float3 &surfaceNormal,
        const float3 &incidentDirection,
        const float4 &diffusivity,
        const float offset,
        const float4 &transmittance,
        const bool doRefraction,
        const float surfaceRefractiveIndex,
        const float4 &surfaceScatteringCoefficient,
        const float transmissionRoughness,
        const float4 &specularity,
        const float specularRoughness,
        const float objectId,
        const bool isExiting,
        float4 &materialBRDF,
        float3 &outgoingDirection,
        float3 &position,
        float nestedDielectrics[MAX_NESTED_DIELECTRICS][NESTED_DIELECTRIC_PARAMS],
        int &numNestedDielectrics,
        float &lightPDF)
{
    // Get the diffuse direction for the next ray
    const float3 diffuseDirection = cosineDirectionInHemisphere(
        surfaceNormal,
        seed
    );

    const float rng = random(random(seed.x) + random(seed.y) + random(seed.z));

    float incidentRefractiveIndex;
    float refractedRefractiveIndex;
    float4 refractedScatteringCoefficient;
    float4 refractedExtinctionCoefficient;
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
            transmittance,
            surfaceScatteringCoefficient,
            incidentRefractiveIndex,
            refractedRefractiveIndex,
            refractedExtinctionCoefficient,
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
            offset,
            idealSpecularDirection,
            outgoingDirection,
            position
        );

        pdf = sampleSpecular(
            idealSpecularDirection,
            outgoingDirection,
            specularity,
            specularProbability,
            materialBRDF,
            lightPDF
        );
    }
    // Maybe refract the ray
    else if (
        transmittance.w > 0.0f
        && rng <= specularProbability + refractionProbability
    ) {
        const float roughness = transmissionRoughness * transmissionRoughness;
        float3 idealRefractedDirection;
        transmissiveBounce(
            incidentDirection,
            surfaceNormal,
            diffuseDirection,
            roughness,
            offset,
            incidentRefractiveIndex,
            refractedRefractiveIndex,
            doRefraction,
            idealRefractedDirection,
            outgoingDirection,
            position
        );

        pdf = sampleTransmissive(
            idealRefractedDirection,
            outgoingDirection,
            refractionProbability,
            refractedRefractiveIndex,
            refractedExtinctionCoefficient,
            refractedScatteringCoefficient,
            objectId,
            doRefraction,
            isExiting,
            materialBRDF,
            lightPDF,
            nestedDielectrics,
            numNestedDielectrics
        );
    }
    // Otherwise diffuse the light
    else
    {
        diffuseBounce(
            surfaceNormal,
            diffuseDirection,
            offset,
            outgoingDirection,
            position
        );

        pdf = sampleDiffuse(
            surfaceNormal,
            diffusivity,
            outgoingDirection,
            1.0f - specularProbability - refractionProbability,
            materialBRDF,
            lightPDF
        );
    }

    return pdf;
}


/**
 * Modify a material based on noise.
 *
 * @arg noiseOptions: The noise modifiers.
 * @arg noiseValue: The noise value.
 * @arg diffusivity: The diffuse values of the surface.
 * @arg specularity: The specular values of the surface.
 * @arg transmittance: The extinction coefficient and transmissive
 *     probability of the surface.
 * @arg emittance: The emissive values of the surface.
 * @arg specularRoughness: The specular roughness of the surface.
 * @arg transmissionRoughness: The transmissive roughness of the
 *     surface.
 * @arg refractiveIndex: The refractive index of the material.
 */
inline void useNoiseOnMaterial(
        const int noiseOptions,
        const float noiseValue,
        float4 &diffusivity,
        float4 &specularity,
        float4 &transmittance,
        float4 &emittance,
        float &specularRoughness,
        float &transmissionRoughness,
        float &refractiveIndex)
{
    if (noiseOptions & DIFFUSE_NOISE)
    {
        diffusivity.x *= noiseValue;
        diffusivity.y *= noiseValue;
        diffusivity.z *= noiseValue;
    }
    if (noiseOptions & SPECULAR_NOISE)
    {
        specularity.w *= noiseValue;
    }
    if (noiseOptions & TRANSMITTANCE_NOISE)
    {
        transmittance.w *= noiseValue;
    }
    if (noiseOptions & EMITTANCE_NOISE)
    {
        emittance *= noiseValue;
    }
    if (noiseOptions & REFRACTIVE_INDEX_NOISE)
    {
        refractiveIndex = (refractiveIndex - 1.0f) * noiseValue + 1.0f;
    }
    if (noiseOptions & TRANSMISSION_ROUGHNESS_NOISE)
    {
        transmissionRoughness *= noiseValue;
    }
    if (noiseOptions & SPECULAR_ROUGHNESS_NOISE)
    {
        specularRoughness *= noiseValue;
    }
}


/**
 * Modify the emittance of a material based on noise.
 *
 * @arg noiseOptions: The noise modifiers.
 * @arg noiseValue: The noise value.
 * @arg emittance: The emissive values of the surface.
 */
void useNoiseOnEmittance(
        const int noiseOptions,
        const float noiseValue,
        float4 &emittance)
{
    if (noiseOptions & EMITTANCE_NOISE)
    {
        emittance *= noiseValue;
    }
}

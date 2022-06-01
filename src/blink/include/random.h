// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Randomization functions
//


/**
 *
 */
inline uint wangHash(uint seed)
{
    seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
    seed *= uint(9);
    seed = seed ^ (seed >> 4);
    seed *= uint(0x27d4eb2d);
    seed = seed ^ (seed >> 15);
    return seed;
}


/**
 * Get a random value on the interval [0, 1].
 *
 * @arg seed: The random seed.
 *
 * @returns: A random value on the interval [0, 1].
 */
inline float random(const float seed)
{
    return fract(sin(seed * 91.3458f) * 47453.5453f);
}


/**
 *
 */
inline float random(uint seed)
{
    return float(wangHash(seed)) / 4294967296.0f;
}


/**
 *
 */
inline float2 random(const float2 &seed)
{
    return float2(
        random(seed.x),
        random(seed.y)
    );
}


/**
 *
 */
inline float3 random(const float3 &seed)
{
    return float3(
        random(seed.x),
        random(seed.y),
        random(seed.z)
    );
}


/**
 *
 */
float3 randomUnitVector(const float3 &seed)
{
    const float z = random(seed.x) * 2.0f - 1.0f;
    const float a = random(seed.y) * 2.0f * PI;
    const float r = sqrt(1.0f - z * z);
    const float x = r * cos(a);
    const float y = r * sin(a);
    return float3(x, y, z);
}


/**
 *
 */
float3 uniformDirectionInZHemisphere(const float3 &seed)
{
    const float uniform = random(seed.x);
    const float r = sqrt(1.0f - uniform * uniform);
    const float phi = 2 * PI * random(seed.y);
 
    return float3(cos(phi) * r, sin(phi) * r, uniform);
}


/**
 *
 */
float3 uniformDirectionInHemisphere(const float3 &axis, const float3 &seed)
{
    return normalize(alignWithDirection(
        float3(0, 0, 1),
        axis,
        uniformDirectionInZHemisphere(seed)
    ));
}


/**
 *
 */
float3 cosineDirectionInZHemisphere(const float3 &seed)
{
    const float uniform = random(seed.x);
    const float r = sqrt(uniform);
    const float angle = 2 * PI * random(seed.y);
 
    const float x = r * cos(angle);
    const float y = r * sin(angle);
 
    return float3(x, y, sqrt(positivePart(1 - uniform)));
}


/**
 *
 */
float3 cosineDirectionInHemisphere(const float3 &axis, const float3 &seed)
{
    return normalize(alignWithDirection(
        float3(0, 0, 1),
        axis,
        cosineDirectionInZHemisphere(seed)
    ));
}


/**
 * Get a random direction within a solid angle.
 *
 * @arg axis: The direction the solid angle is aligned with.
 * @arg angle: The angle from the axis to the conical surface.
 * @arg seed: The random seed.
 */
float3 randomDirectionInSolidAngle(
        const float3 &axis,
        const float angle,
        const float3 &seed)
{
    const float3 uniform = random(seed);

    float3x3 rotation;
    rotationMatrix(
        angle * uniform,
        rotation
    );

    float3 direction;
    matmul(rotation, axis, direction);

    return normalize(direction);
}

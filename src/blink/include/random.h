// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Randomization functions
//


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


inline float random(uint seed)
{
    return float(wangHash(seed)) / 4294967296.0f;
}


inline float2 random(float2 seed)
{
    return float2(
        random(seed.x),
        random(seed.y)
    );
}


inline float3 random(float3 seed)
{
    return float3(
        random(seed.x),
        random(seed.y),
        random(seed.z)
    );
}


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
 * Randomly redirect the surface normal to give it roughness.
 *
 * @arg normal: The normal to redirect.
 * @arg roughness: The maximum amount to roughen by.
 * @arg seed: The random seed.
 */
float3 randomDirectionInHemisphere(
    const float3 &normal,
    const float roughness,
    const float3 &seed)
{
    const float3 angle = random(seed);

    float3x3 rotation;
    rotationMatrix(
        PI * roughness * angle / 2.0f,
        rotation
    );

    float3 roughened;
    matmul(rotation, normal, roughened);

    return normalize(roughened);
}

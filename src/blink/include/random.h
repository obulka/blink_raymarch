// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Randomization functions
//


// Some random constants on the interval [1, 2]
#define RAND_CONST_0 1.571411510193971f
#define RAND_CONST_1 1.268632820084931f
#define RAND_CONST_2 1.7880365647937733f
#define RAND_CONST_3 1.3546987471558234f
#define RAND_CONST_4 1.4365958250848703f
#define RAND_CONST_5 1.7045380669435368f
#define RAND_CONST_6 1.2006950006793073f
#define RAND_CONST_7 1.3877943854025474f
#define RAND_CONST_8 1.2513180038618783f
#define RAND_CONST_9 1.8584270278009565f
#define RAND_CONST_10 1.1299747498069974f
#define RAND_CONST_11 1.394137930742262f
#define RAND_CONST_12 1.7779101864424334f


/**
 * Compute a Wang hash.
 *
 * @arg seed: The seed to hash.
 *
 * @returns: The hashed value.
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
 * Compute a Wang hash.
 *
 * @arg seed: The seed to hash.
 *
 * @returns: The hashed value.
 */
inline int wangHash(int seed)
{
    seed = int(seed ^ 61) ^ int(seed >> 16);
    seed *= 9;
    seed = seed ^ (seed >> 4);
    seed *= int(0x27d4eb2d);
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
 * Get a random value on the interval [0, 1].
 *
 * @arg seed: The random seed.
 *
 * @returns: A random value on the interval [0, 1].
 */
inline float random(uint seed)
{
    return float(wangHash(seed)) / 4294967296.0f;
}


/**
 * Get a random value on the interval [0, 1].
 *
 * @arg seed: The random seed.
 *
 * @returns: A random value on the interval [0, 1].
 */
inline float2 random(const float2 &seed)
{
    return float2(
        random(seed.x),
        random(seed.y)
    );
}


/**
 * Get a random value on the interval [0, 1].
 *
 * @arg seed: The random seed.
 *
 * @returns: A random value on the interval [0, 1].
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
 * Get a random value on the interval [0, 1].
 *
 * @arg seed: The random seed.
 *
 * @returns: A random value on the interval [0, 1].
 */
inline float4 random(const float4 &seed)
{
    return float4(
        random(seed.x),
        random(seed.y),
        random(seed.z),
        random(seed.w)
    );
}


/**
 * Create a random unit vector.
 *
 * @arg seed: The random seed.
 *
 * @returns: A random unit vector.
 */
float3 randomUnitVector(const float3 &seed)
{
    const float z = random(seed.x) * 2.0f - 1.0f;
    const float a = random(seed.y) * 2.0f * PI;
    const float r = sqrt(1.0f - z * z);
    const float x = r * cos(a);
    const float y = r * sin(a);
    return normalize(float3(x, y, z));
}


/**
 * Create a random unit vector in the hemisphere aligned along the
 * z-axis.
 *
 * @arg seed: The random seed.
 *
 * @returns: A random unit vector.
 */
float3 uniformDirectionInZHemisphere(const float3 &seed)
{
    const float uniform = random(seed.x);
    const float r = sqrt(1.0f - uniform * uniform);
    const float phi = 2 * PI * random(seed.y);
 
    return float3(cos(phi) * r, sin(phi) * r, uniform);
}


/**
 * Create a random unit vector in the hemisphere aligned along the
 * given axis.
 *
 * @arg axis: The axis to align the hemisphere with.
 * @arg seed: The random seed.
 *
 * @returns: A random unit vector.
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
 * Create a random point that lies within the unit circle.
 *
 * @arg seed: The random seed.
 *
 * @returns: A random point, (radius, angle) in the unit circle.
 */
inline float2 uniformPointInUnitCircle(const float3 &seed)
{
    return float2(sqrt(random(seed.x)), 2.0f * PI * random(seed.y));
}


/**
 * Create a random unit vector in the hemisphere aligned along the
 * z-axis, with a distribution that is cosine weighted.
 *
 * @arg seed: The random seed.
 *
 * @returns: A random unit vector.
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
 * Create a random unit vector in the hemisphere aligned along the
 * given axis, with a distribution that is cosine weighted.
 *
 * @arg axis: The axis to align the hemisphere with.
 * @arg seed: The random seed.
 *
 * @returns: A random unit vector.
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
 * Get a random direction within a solid angle oriented along the
 * z-axis.
 *
 * https://math.stackexchange.com/questions/56784/generate-a-random-direction-within-a-cone
 *
 * @arg angle: The angle from the axis to the conical surface.
 * @arg seed: The random seed.
 *
 * @returns: A random unit vector.
 */
inline float3 uniformDirectionInZSolidAngle(const float angle, const float3 &seed)
{
    const float cosAngle = cos(angle);
    const float z = random(seed.x) * (1.0f - cosAngle) + cosAngle;
    const float phi = random(seed.y) * 2.0f * PI;

    const float rootOneMinusZSquared = sqrt(1.0f - z * z);

    return normalize(float3(
        rootOneMinusZSquared * cos(phi),
        rootOneMinusZSquared * sin(phi),
        z
    ));
}


/**
 * Get a random direction within a solid angle.
 *
 * @arg axis: The direction the solid angle is aligned with.
 * @arg angle: The angle from the axis to the conical surface.
 * @arg seed: The random seed.
 *
 * @returns: A random unit vector.
 */
float3 uniformDirectionInSolidAngle(
        const float3 &axis,
        const float angle,
        const float3 &seed)
{
    return normalize(alignWithDirection(
        float3(0, 0, 1),
        axis,
        uniformDirectionInZSolidAngle(angle, seed)
    ));
}

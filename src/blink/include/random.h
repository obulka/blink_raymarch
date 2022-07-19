// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Randomization functions
//


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
#define RAND_CONST_13 1.3438643303610738f
#define RAND_CONST_14 1.749733022214246f
#define RAND_CONST_15 1.8817419700212032f
#define RAND_CONST_16 1.5659502633083733f
#define RAND_CONST_17 1.0771187500076516f
#define RAND_CONST_18 1.2588977390336775f
#define RAND_CONST_19 1.424285929504117f


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
 *
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
    return normalize(float3(x, y, z));
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
inline float2 uniformPointInUnitCircle(const float3 &seed)
{
    return float2(sqrt(random(seed.x)), 2.0f * PI * random(seed.y));
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
 * Get a random direction within a solid angle oriented along the z-axis.
 *
 * https://math.stackexchange.com/questions/56784/generate-a-random-direction-within-a-cone
 *
 * @arg axis: The direction the solid angle is aligned with.
 * @arg angle: The angle from the axis to the conical surface.
 * @arg seed: The random seed.
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

// The MIT License (MIT)

// Copyright (c) 2012-2018 Sebastien Rombauts (sebastien.rombauts@gmail.com)

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is furnished
// to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
// IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

//
// Noise functions adapted from: https://github.com/SRombauts
//


// Skewing/Unskewing factors for 2D
#define F2 0.366025403f // F2 = (sqrt(3) - 1) / 2
#define G2 0.211324865f // G2 = (3 - sqrt(3)) / 6   = F2 / (1 + 2 * K)

// Skewing/Unskewing factors for 3D
#define F3 1.0f / 3.0f
#define G3 1.0f / 6.0f


/**
 * Helper function to compute gradients-dot-residual vectors (1D)
 *
 * @note that these generate gradients of more than unit length. To make
 * a close match with the value range of classic Perlin noise, the final
 * noise values need to be rescaled to fit nicely within [-1,1].
 * (The simplex noise functions as such also have different scaling.)
 * Note also that these noise functions are the most practical and useful
 * signed version of Perlin noise.
 *
 * @arg hash: Hash value.
 * @arg x: Distance to the corner.
 *
 * @returns: Gradient value.
 */
float grad(const int hash, const float x)
{
    const int h = hash & 0x0F;  // Convert low 4 bits of hash code
    float grad = 1.0f + (h & 7);    // Gradient value 1.0, 2.0, ..., 8.0
    if ((h & 8) != 0) grad = -grad; // Set a random sign for the gradient
    return (grad * x);              // Multiply the gradient with the distance
}


/**
 * Helper functions to compute gradients-dot-residual vectors (2D)
 *
 * @arg hash: Hash value.
 * @arg x: x coordinate of the distance to the corner.
 * @arg y: y coordinate of the distance to the corner.
 *
 * @returns: Gradient value.
 */
float grad(const int hash, const float x, const float y)
{
    const int h = hash & 0x3F;  // Convert low 3 bits of hash code
    const float u = h < 4 ? x : y;  // into 8 simple gradient directions,
    const float v = h < 4 ? y : x;
    // and compute the dot product with (x,y).
    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
}


/**
 * Helper functions to compute gradients-dot-residual vectors (3D)
 *
 * @arg hash: Hash value.
 * @arg x: x coordinate of the distance to the corner.
 * @arg y: y coordinate of the distance to the corner.
 * @arg z: z coordinate of the distance to the corner.
 *
 * @returns: Gradient value
 */
float grad(const int hash, const float x, const float y, const float z)
{
    const int h = hash & 15;     // Convert low 4 bits of hash code into 12 simple
    const float u = h < 8 ? x : y; // gradient directions, and compute dot product.
    const float v = h < 4 ? y : h == 12 || h == 14 ? x : z; // Fix repeats at h = 12 to 15
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}


/**
 * 1D Perlin simplex noise
 * Takes around 74ns on an AMD APU.
 *
 * @arg seed: The seed for the noise.
 *
 * @returns: Noise value in the range [-1, 1], value of 0 on all integer
 *     coordinates.
 */
float perlinSimplexNoise(const float seed)
{
    float n0, n1; // Noise contributions from the two "corners"

    // Corners coordinates (nearest integer values):
    const int i0 = floor(seed);
    const int i1 = i0 + 1;
    // Distances to corners (between 0 and 1):
    const float x0 = seed - i0;
    const float x1 = x0 - 1.0f;

    // Calculate the contribution from the first corner
    float t0 = 1.0f - x0 * x0;
    t0 *= t0;
    n0 = t0 * t0 * grad(wangHash(i0), x0);

    // Calculate the contribution from the second corner
    float t1 = 1.0f - x1 * x1;
    t1 *= t1;
    n1 = t1 * t1 * grad(wangHash(i1), x1);

    // The maximum value of this noise is 8*(3/4)^4 = 2.53125
    // A factor of 0.395 scales to fit exactly within [-1,1]
    return 0.395f * (n0 + n1);
}


/**
 * 2D Perlin simplex noise
 * Takes around 150ns on an AMD APU.
 *
 * @arg seed: The seed for the noise.
 *
 * @returns: Noise value in the range [-1, 1], value of 0 on all integer
 *     coordinates.
 */
float perlinSimplexNoise(const float2 &seed)
{
    float n0, n1, n2;   // Noise contributions from the three corners

    // Skew the input space to determine which simplex cell we're in
    const float s = sumComponent(seed) * F2;  // Hairy factor for 2D
    const float xs = seed.x + s;
    const float ys = seed.y + s;
    const int i = floor(xs);
    const int j = floor(ys);

    // Unskew the cell origin back to (x,y) space
    const float t = float(i + j) * G2;
    const float X0 = i - t;
    const float Y0 = j - t;
    const float x0 = seed.x - X0;  // The x,y distances from the cell origin
    const float y0 = seed.y - Y0;

    // For the 2D case, the simplex shape is an equilateral triangle.
    // Determine which simplex we are in.
    int i1, j1;  // Offsets for second (middle) corner of simplex in (i,j) coords
    if (x0 > y0)
    {   
        // lower triangle, XY order: (0,0)->(1,0)->(1,1)
        i1 = 1;
        j1 = 0;
    }
    else
    {   
        // upper triangle, YX order: (0,0)->(0,1)->(1,1)
        i1 = 0;
        j1 = 1;
    }

    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6

    // Offsets for middle corner in (x,y) unskewed coords
    const float x1 = x0 - i1 + G2;
    const float y1 = y0 - j1 + G2;
    // Offsets for last corner in (x,y) unskewed coords
    const float x2 = x0 - 1.0f + 2.0f * G2;
    const float y2 = y0 - 1.0f + 2.0f * G2;

    // Work out the hashed gradient indices of the three simplex corners
    const int gi0 = wangHash(i + wangHash(j));
    const int gi1 = wangHash(i + i1 + wangHash(j + j1));
    const int gi2 = wangHash(i + 1 + wangHash(j + 1));

    // Calculate the contribution from the first corner
    float t0 = 0.5f - x0 * x0 - y0 * y0;
    if (t0 < 0.0f)
    {
        n0 = 0.0f;
    }
    else
    {
        t0 *= t0;
        n0 = t0 * t0 * grad(gi0, x0, y0);
    }

    // Calculate the contribution from the second corner
    float t1 = 0.5f - x1 * x1 - y1 * y1;
    if (t1 < 0.0f)
    {
        n1 = 0.0f;
    }
    else
    {
        t1 *= t1;
        n1 = t1 * t1 * grad(gi1, x1, y1);
    }

    // Calculate the contribution from the third corner
    float t2 = 0.5f - x2 * x2 - y2 * y2;
    if (t2 < 0.0f)
    {
        n2 = 0.0f;
    }
    else
    {
        t2 *= t2;
        n2 = t2 * t2 * grad(gi2, x2, y2);
    }

    // Add contributions from each corner to get the final noise value.
    // The result is scaled to return values in the interval [-1,1].
    return 45.23065f * (n0 + n1 + n2);
}


/**
 * 3D Perlin simplex noise
 *
 * @arg seed: The seed for the noise.
 *
 * @returns: Noise value in the range [-1, 1], value of 0 on all integer
 *     coordinates.
 */
float perlinSimplexNoise(const float3 &seed)
{
    float n0, n1, n2, n3; // Noise contributions from the four corners

    // Skew the input space to determine which simplex cell we're in
    const float s = sumComponent(seed) * F3; // Very nice and simple skew factor for 3D
    const int i = floor(seed.x + s);
    const int j = floor(seed.y + s);
    const int k = floor(seed.z + s);
    const float t = (i + j + k) * G3;
    const float X0 = i - t; // Unskew the cell origin back to (x,y,z) space
    const float Y0 = j - t;
    const float Z0 = k - t;
    const float x0 = seed.x - X0; // The x,y,z distances from the cell origin
    const float y0 = seed.y - Y0;
    const float z0 = seed.z - Z0;

    // For the 3D case, the simplex shape is a slightly irregular tetrahedron.
    // Determine which simplex we are in.
    int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
    int i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords
    if (x0 >= y0)
    {
        if (y0 >= z0)
        {
            i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0; // X Y Z order
        }
        else if (x0 >= z0)
        {
            i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; // X Z Y order
        }
        else
        {
            i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; // Z X Y order
        }
    }
    else
    { // x0<y0
        if (y0 < z0)
        {
            i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; // Z Y X order
        }
        else if (x0 < z0)
        {
            i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; // Y Z X order
        }
        else
        {
            i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; // Y X Z order
        }
    }

    // A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
    // a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
    // a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
    // c = 1/6.
    const float x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
    const float y1 = y0 - j1 + G3;
    const float z1 = z0 - k1 + G3;
    const float x2 = x0 - i2 + 2.0f * G3; // Offsets for third corner in (x,y,z) coords
    const float y2 = y0 - j2 + 2.0f * G3;
    const float z2 = z0 - k2 + 2.0f * G3;
    const float x3 = x0 - 1.0f + 3.0f * G3; // Offsets for last corner in (x,y,z) coords
    const float y3 = y0 - 1.0f + 3.0f * G3;
    const float z3 = z0 - 1.0f + 3.0f * G3;

    // Work out the hashed gradient indices of the four simplex corners
    const int gi0 = wangHash(i + wangHash(j + wangHash(k)));
    const int gi1 = wangHash(i + i1 + wangHash(j + j1 + wangHash(k + k1)));
    const int gi2 = wangHash(i + i2 + wangHash(j + j2 + wangHash(k + k2)));
    const int gi3 = wangHash(i + 1 + wangHash(j + 1 + wangHash(k + 1)));

    // Calculate the contribution from the four corners
    float t0 = 0.6f - x0 * x0 - y0 * y0 - z0 * z0;
    if (t0 < 0)
    {
        n0 = 0.0;
    }
    else
    {
        t0 *= t0;
        n0 = t0 * t0 * grad(gi0, x0, y0, z0);
    }
    float t1 = 0.6f - x1 * x1 - y1 * y1 - z1 * z1;
    if (t1 < 0)
    {
        n1 = 0.0;
    }
    else
    {
        t1 *= t1;
        n1 = t1 * t1 * grad(gi1, x1, y1, z1);
    }
    float t2 = 0.6f - x2 * x2 - y2 * y2 - z2 * z2;
    if (t2 < 0)
    {
        n2 = 0.0;
    }
    else
    {
        t2 *= t2;
        n2 = t2 * t2 * grad(gi2, x2, y2, z2);
    }
    float t3 = 0.6f - x3 * x3 - y3 * y3 - z3 * z3;
    if (t3 < 0)
    {
        n3 = 0.0;
    }
    else
    {
        t3 *= t3;
        n3 = t3 * t3 * grad(gi3, x3, y3, z3);
    }
    // Add contributions from each corner to get the final noise value.
    // The result is scaled to stay just inside [-1,1]
    return 32.0f * (n0 + n1 + n2 + n3);
}


/**
 * 4D Perlin simplex noise
 *
 * @arg seed: The seed for the noise.
 *
 * @returns: Noise value in the range [-1, 1], value of 0 on all integer
 *     coordinates.
 */
inline float perlinSimplexNoise(
        const float4 &seed,
        const int simplex[64][4],
        const int perm[512],
        const int grad4[32][4])
{
    const float F4 = (sqrt(5.0f) - 1.0f) / 4.0f;
    const float G4 = (5.0f - sqrt(5.0f)) / 20.0f;
    const float s = sumComponent(seed) * F4;
    const int4 i = floatToInt(seed + s);
    const float t = sumComponent(i) * G4;
    const float4 X = intToFloat(i) - t;
    const float4 x = seed - X;

    const int c1 = (x.x > x.y) ? 32 : 0;
    const int c2 = (x.x > x.z) ? 16 : 0;
    const int c3 = (x.y > x.z) ? 8 : 0;
    const int c4 = (x.x > x.w) ? 4 : 0;
    const int c5 = (x.y > x.w) ? 2 : 0;
    const int c6 = (x.z > x.w) ? 1 : 0;
    const int c = c1 + c2 + c3 + c4 + c5 + c6;

    const int4 i1 = int4(
        simplex[c][0] >= 3 ? 1 : 0,
        simplex[c][1] >= 3 ? 1 : 0,
        simplex[c][2] >= 3 ? 1 : 0,
        simplex[c][3] >= 3 ? 1 : 0
    );

    const int4 i2 = int4(
        simplex[c][0] >= 2 ? 1 : 0,
        simplex[c][1] >= 2 ? 1 : 0,
        simplex[c][2] >= 2 ? 1 : 0,
        simplex[c][3] >= 2 ? 1 : 0
    );

    const int4 i3 = int4(
        simplex[c][0] >= 1 ? 1 : 0,
        simplex[c][1] >= 1 ? 1 : 0,
        simplex[c][2] >= 1 ? 1 : 0,
        simplex[c][3] >= 1 ? 1 : 0
    );

    const float4 x1 = x - intToFloat(i1) + G4;
    const float4 x2 = x - intToFloat(i2) + 2.0 * G4;
    const float4 x3 = x - intToFloat(i3) + 3.0 * G4;
    const float4 x4 = x - 1.0 + 4.0 * G4;

    const int4 ii = int4(
        i.x & 255,
        i.y & 255,
        i.z & 255,
        i.w & 255
    );

    const int gi0 = perm[
        ii.x + perm[ii.y + perm[ii.z + perm[ii.w]]]
    ] % 32;

    const int gi1 = perm[
        ii.x + i1.x + perm[ii.y + i1.y + perm[ii.z + i1.z + perm[ii.w + i1.w]]]
    ] % 32;

    const int gi2 = perm[
        ii.x + i2.x + perm[ii.y + i2.y + perm[ii.z + i2.z + perm[ii.w + i2.w]]]
    ] % 32;

    const int gi3 = perm[
        ii.x + i3.x + perm[ii.y + i3.y + perm[ii.z + i3.z + perm[ii.w + i3.w]]]
    ] % 32;

    const int gi4 = perm[
        ii.x + 1 + perm[ii.y + 1 + perm[ii.z + 1 + perm[ii.w + 1]]]
    ] % 32;

    float n0, n1, n2, n3, n4;

    float t0 = 0.6 - x.x * x.x - x.y * x.y - x.z * x.z - x.w * x.w;
    if (t0 < 0)
    {
        n0 = 0.0;
    }
    else
    {
        t0 *= t0;
        n0 = t0 * t0 * dot(
            float4(grad4[gi0][0], grad4[gi0][2], grad4[gi0][3], grad4[gi0][3]),
            x
        );
    }

    float t1 = 0.6 - x1.x * x1.x - x1.y * x1.y - x1.z * x1.z - x1.w * x1.w;
    if (t1 < 0)
    {
        n1 = 0.0;
    }
    else
    {
        t1 *= t1;
        n1 = t1 * t1 * dot(
            float4(grad4[gi1][0] , grad4[gi1][2], grad4[gi1][3], grad4[gi1][3]),
            x1
        );
    }

    float t2 = 0.6 - x2.x * x2.x - x2.y * x2.y - x2.z * x2.z - x2.w * x2.w;
    if (t2 < 0)
    {
        n2 = 0.0;
    }
    else
    {
        t2 *= t2;
        n2 = t2 * t2 * dot(
            float4(grad4[gi2][0], grad4[gi2][2], grad4[gi2][3], grad4[gi2][3]),
            x2
        );
    }

    float t3 = 0.6 - x3.x * x3.x - x3.y * x3.y - x3.z * x3.z - x3.w * x3.w;
    if (t3 < 0)
    {
        n3 = 0.0;
    }
    else
    {
        t3 *= t3;
        n3 = t3 * t3 * dot(
            float4(grad4[gi3][0], grad4[gi3][2], grad4[gi3][3], grad4[gi3][3]),
            x3
        );
    }

    float t4 = 0.6 - x4.x * x4.x - x4.y * x4.y - x4.z * x4.z - x4.w * x4.w;
    if (t4 < 0)
    {
        n4 = 0.0;
    }
    else {
        t4 *= t4;
        n4 = t4 * t4 * dot(
            float4(grad4[gi4][0], grad4[gi4][2], grad4[gi4][3], grad4[gi4][3]),
            x4
        );
    }

    return 27.0 * (n0 + n1 + n2 + n3 + n4);
}


// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Use the perlin simplex noise
//


/**
 * fBM noise.
 *
 * @arg octaves: The number of different frequencies to use.
 * @arg lacunarity: The per octave frequency multiplier.
 * @arg size: The size of the noise.
 * @arg gain: The per octave amplitude multiplier.
 * @arg gamma: The result will be raised to 1 over this power.
 * @arg position: The position to seed the noise.
 *
 * @returns: The noise value in the range [-1, 1], with a value of 0 on
 *     all integer coordinates.
 */
float fractalBrownianMotionNoise(
        const int octaves,
        const float lacunarity,
        const float size,
        const float gain,
        const float gamma,
        const float position)
{
    float output = 0.0f;
    float denom = 0.0f;
    float frequency = lacunarity;
    float amplitude = gain;

    for (int octave = 0; octave < octaves; octave++)
    {
        output += amplitude * perlinSimplexNoise(position * frequency / size);
        denom += amplitude;

        frequency *= lacunarity;
        amplitude *= gain;
    }

    if (denom == 0.0f || gamma == 0.0f)
    {
        return 1.0f;
    }
    return pow(output / denom, 1.0f / gamma);
}


/**
 * fBM noise.
 *
 * @arg octaves: The number of different frequencies to use.
 * @arg lacunarity: The per octave frequency multiplier.
 * @arg size: The size of the noise.
 * @arg gain: The per octave amplitude multiplier.
 * @arg gamma: The result will be raised to 1 over this power.
 * @arg position: The position to seed the noise.
 *
 * @returns: The noise value in the range [-1, 1], with a value of 0 on
 *     all integer coordinates.
 */
float fractalBrownianMotionNoise(
        const int octaves,
        const float lacunarity,
        const float size,
        const float gain,
        const float gamma,
        const float2 &position)
{
    float output = 0.0f;
    float denom = 0.0f;
    float frequency = lacunarity;
    float amplitude = gain;

    for (int octave = 0; octave < octaves; octave++)
    {
        output += amplitude * perlinSimplexNoise(position * frequency / size);
        denom += amplitude;

        frequency *= lacunarity;
        amplitude *= gain;
    }

    if (denom == 0.0f || gamma == 0.0f)
    {
        return 1.0f;
    }
    return pow(output / denom, 1.0f / gamma);
}


/**
 * fBM noise.
 *
 * @arg octaves: The number of different frequencies to use.
 * @arg lacunarity: The per octave frequency multiplier.
 * @arg size: The size of the noise.
 * @arg gain: The per octave amplitude multiplier.
 * @arg gamma: The result will be raised to 1 over this power.
 * @arg position: The position to seed the noise.
 *
 * @returns: The noise value in the range [-1, 1], with a value of 0 on
 *     all integer coordinates.
 */
float fractalBrownianMotionNoise(
        const int octaves,
        const float lacunarity,
        const float size,
        const float gain,
        const float gamma,
        const float3 &position)
{
    float output = 0.0f;
    float denom = 0.0f;
    float frequency = lacunarity;
    float amplitude = gain;

    for (int octave = 0; octave < octaves; octave++)
    {
        output += amplitude * perlinSimplexNoise(position * frequency / size);
        denom += amplitude;

        frequency *= lacunarity;
        amplitude *= gain;
    }

    if (denom == 0.0f || gamma == 0.0f)
    {
        return 1.0f;
    }
    return pow(output / denom, 1.0f / gamma);
}


/**
 * fBM noise.
 */
float fractalBrownianMotionNoise(
        const float octaves,
        const float lacunarity,
        const float gain,
        const float gamma,
        const float4 &position,
        const float4 &lowFrequencyScale,
        const float4 &highFrequencyScale,
        const float4 &lowFrequencyEvolution,
        const float4 &highFrequencyEvolution,
        const int simplex[64][4],
        const int perm[512],
        const int grad4[32][4])
{
    float output = 0.0f;
    float frequency = lacunarity;
    float amplitude = 1.0f;
    float denom = 0.0f;
    float4 evolution;
    float4 frequencyScale;

    for (int octave=0; octave < octaves; octave++)
    {
        const float octaveFraction = octave / (octaves - 1.0f);
        frequencyScale = (
            (highFrequencyScale * octaveFraction)
            + (lowFrequencyScale * (1 - octaveFraction))
        );       
        evolution = (
            (highFrequencyEvolution * octaveFraction)
            + (lowFrequencyEvolution * (1 - octaveFraction))
        );

        output += amplitude * perlinSimplexNoise(
            (position * frequencyScale + evolution) * frequency,
            simplex,
            perm,
            grad4
        );

        frequency *= 2.0f;
        denom += amplitude;
        amplitude *= gain;
    }

    if (denom == 0.0f || gamma == 0.0f)
    {
        return 1.0f;
    }
    return pow(output / denom, 1.0f / gamma);
}


/**
 * Turbulence noise.
 *
 * @arg octaves: The number of different frequencies to use.
 * @arg lacunarity: The per octave frequency multiplier.
 * @arg size: The size of the noise.
 * @arg gain: The per octave amplitude multiplier.
 * @arg gamma: The result will be raised to 1 over this power.
 * @arg position: The position to seed the noise.
 *
 * @returns: The noise value in the range [0, 1], with a value of 0 on
 *     all integer coordinates.
 */
float turbulenceNoise(
        const int octaves,
        const float lacunarity,
        const float size,
        const float gain,
        const float gamma,
        const float position)
{
    float output = 0.0f;
    float denom = 0.0f;
    float frequency = lacunarity;
    float amplitude = gain;

    for (int octave = 0; octave < octaves; octave++)
    {
        output += fabs(amplitude * perlinSimplexNoise(position * frequency / size));
        denom += amplitude;

        frequency *= lacunarity;
        amplitude *= gain;
    }

    if (denom == 0.0f || gamma == 0.0f)
    {
        return 1.0f;
    }
    return pow(output / denom, 1.0f / gamma);
}


/**
 * Turbulence noise.
 *
 * @arg octaves: The number of different frequencies to use.
 * @arg lacunarity: The per octave frequency multiplier.
 * @arg size: The size of the noise.
 * @arg gain: The per octave amplitude multiplier.
 * @arg gamma: The result will be raised to 1 over this power.
 * @arg position: The position to seed the noise.
 *
 * @returns: The noise value in the range [0, 1], with a value of 0 on
 *     all integer coordinates.
 */
float turbulenceNoise(
        const int octaves,
        const float lacunarity,
        const float size,
        const float gain,
        const float gamma,
        const float2 &position)
{
    float output = 0.0f;
    float denom = 0.0f;
    float frequency = lacunarity;
    float amplitude = gain;

    for (int octave = 0; octave < octaves; octave++)
    {
        output += fabs(amplitude * perlinSimplexNoise(position * frequency / size));
        denom += amplitude;

        frequency *= lacunarity;
        amplitude *= gain;
    }

    if (denom == 0.0f || gamma == 0.0f)
    {
        return 1.0f;
    }
    return pow(output / denom, 1.0f / gamma);
}


/**
 * Turbulence noise.
 *
 * @arg octaves: The number of different frequencies to use.
 * @arg lacunarity: The per octave frequency multiplier.
 * @arg size: The size of the noise.
 * @arg gain: The per octave amplitude multiplier.
 * @arg gamma: The result will be raised to 1 over this power.
 * @arg position: The position to seed the noise.
 *
 * @returns: The noise value in the range [0, 1], with a value of 0 on
 *     all integer coordinates.
 */
float turbulenceNoise(
        const int octaves,
        const float lacunarity,
        const float size,
        const float gain,
        const float gamma,
        const float3 &position)
{
    float output = 0.0f;
    float denom = 0.0f;
    float frequency = lacunarity;
    float amplitude = gain;

    for (int octave = 0; octave < octaves; octave++)
    {
        output += fabs(amplitude * perlinSimplexNoise(position * frequency / size));
        denom += amplitude;

        frequency *= lacunarity;
        amplitude *= gain;
    }

    if (denom == 0.0f || gamma == 0.0f)
    {
        return 1.0f;
    }
    return pow(output / denom, 1.0f / gamma);
}


/**
 * Turbulence noise.
 */
float turbulenceNoise(
        const float octaves,
        const float lacunarity,
        const float gain,
        const float gamma,
        const float4 &position,
        const float4 &lowFrequencyScale,
        const float4 &highFrequencyScale,
        const float4 &lowFrequencyEvolution,
        const float4 &highFrequencyEvolution,
        const int simplex[64][4],
        const int perm[512],
        const int grad4[32][4])
{
    float output = 0.0f;
    float frequency = lacunarity;
    float amplitude = 1.0f;
    float denom = 0.0f;
    float4 evolution;
    float4 frequencyScale;

    for (int octave=0; octave < octaves; octave++)
    {
        const float octaveFraction = octave / (octaves - 1.0f);
        frequencyScale = (
            (highFrequencyScale * octaveFraction)
            + (lowFrequencyScale * (1 - octaveFraction))
        );       
        evolution = (
            (highFrequencyEvolution * octaveFraction)
            + (lowFrequencyEvolution * (1 - octaveFraction))
        );

        output += fabs(
            amplitude * perlinSimplexNoise(
                (position * frequencyScale + evolution) * frequency,
                simplex,
                perm,
                grad4
            )
        );

        frequency *= 2.0f;
        denom += amplitude;
        amplitude *= gain;
    }

    if (denom == 0.0f || gamma == 0.0f)
    {
        return 1.0f;
    }
    return pow(output / denom, 1.0f / gamma);
}

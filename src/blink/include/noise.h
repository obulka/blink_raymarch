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
 * @param[in] hash  hash value
 * @param[in] x     distance to the corner
 *
 * @return gradient value
 */
static float grad(int hash, float x)
{
    const int h = hash & 0x0F;  // Convert low 4 bits of hash code
    float grad = 1.0f + (h & 7);    // Gradient value 1.0, 2.0, ..., 8.0
    if ((h & 8) != 0) grad = -grad; // Set a random sign for the gradient
//  float grad = gradients1D[h];    // NOTE : Test of Gradient look-up table instead of the above
    return (grad * x);              // Multiply the gradient with the distance
}


/**
 * Helper functions to compute gradients-dot-residual vectors (2D)
 *
 * @param[in] hash  hash value
 * @param[in] x     x coord of the distance to the corner
 * @param[in] y     y coord of the distance to the corner
 *
 * @return gradient value
 */
static float grad(int hash, float x, float y)
{
    const int h = hash & 0x3F;  // Convert low 3 bits of hash code
    const float u = h < 4 ? x : y;  // into 8 simple gradient directions,
    const float v = h < 4 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v); // and compute the dot product with (x,y).
}


/**
 * Helper functions to compute gradients-dot-residual vectors (3D)
 *
 * @param[in] hash  hash value
 * @param[in] x     x coord of the distance to the corner
 * @param[in] y     y coord of the distance to the corner
 * @param[in] z     z coord of the distance to the corner
 *
 * @return gradient value
 */
static float grad(int hash, float x, float y, float z)
{
    int h = hash & 15;     // Convert low 4 bits of hash code into 12 simple
    float u = h < 8 ? x : y; // gradient directions, and compute dot product.
    float v = h < 4 ? y : h == 12 || h == 14 ? x : z; // Fix repeats at h = 12 to 15
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}


/**
 * 1D Perlin simplex noise
 *
 *  Takes around 74ns on an AMD APU.
 *
 * @param[in] x float coordinate
 *
 * @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
 */
float perlinSimplexNoise(const float seed)
{
    float n0, n1;   // Noise contributions from the two "corners"

    // Corners coordinates (nearest integer values):
    int i0 = floor(seed);
    int i1 = i0 + 1;
    // Distances to corners (between 0 and 1):
    float x0 = seed - i0;
    float x1 = x0 - 1.0f;

    // Calculate the contribution from the first corner
    float t0 = 1.0f - x0*x0;
//  if(t0 < 0.0f) t0 = 0.0f; // not possible
    t0 *= t0;
    n0 = t0 * t0 * grad(wangHash(i0), x0);

    // Calculate the contribution from the second corner
    float t1 = 1.0f - x1*x1;
//  if(t1 < 0.0f) t1 = 0.0f; // not possible
    t1 *= t1;
    n1 = t1 * t1 * grad(wangHash(i1), x1);

    // The maximum value of this noise is 8*(3/4)^4 = 2.53125
    // A factor of 0.395 scales to fit exactly within [-1,1]
    return 0.395f * (n0 + n1);
}


/**
 * 2D Perlin simplex noise
 *
 *  Takes around 150ns on an AMD APU.
 *
 * @param[in] x float coordinate
 * @param[in] y float coordinate
 *
 * @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
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
    if (x0 > y0) {   // lower triangle, XY order: (0,0)->(1,0)->(1,1)
        i1 = 1;
        j1 = 0;
    } else {   // upper triangle, YX order: (0,0)->(0,1)->(1,1)
        i1 = 0;
        j1 = 1;
    }

    // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
    // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
    // c = (3-sqrt(3))/6

    const float x1 = x0 - i1 + G2;            // Offsets for middle corner in (x,y) unskewed coords
    const float y1 = y0 - j1 + G2;
    const float x2 = x0 - 1.0f + 2.0f * G2;   // Offsets for last corner in (x,y) unskewed coords
    const float y2 = y0 - 1.0f + 2.0f * G2;

    // Work out the hashed gradient indices of the three simplex corners
    const int gi0 = wangHash(i + wangHash(j));
    const int gi1 = wangHash(i + i1 + wangHash(j + j1));
    const int gi2 = wangHash(i + 1 + wangHash(j + 1));

    // Calculate the contribution from the first corner
    float t0 = 0.5f - x0*x0 - y0*y0;
    if (t0 < 0.0f) {
        n0 = 0.0f;
    } else {
        t0 *= t0;
        n0 = t0 * t0 * grad(gi0, x0, y0);
    }

    // Calculate the contribution from the second corner
    float t1 = 0.5f - x1*x1 - y1*y1;
    if (t1 < 0.0f) {
        n1 = 0.0f;
    } else {
        t1 *= t1;
        n1 = t1 * t1 * grad(gi1, x1, y1);
    }

    // Calculate the contribution from the third corner
    float t2 = 0.5f - x2*x2 - y2*y2;
    if (t2 < 0.0f) {
        n2 = 0.0f;
    } else {
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
 * @param[in] x float coordinate
 * @param[in] y float coordinate
 * @param[in] z float coordinate
 *
 * @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
 */
float perlinSimplexNoise(const float3 &seed)
{
    float n0, n1, n2, n3; // Noise contributions from the four corners

    // Skew the input space to determine which simplex cell we're in
    float s = sumComponent(seed) * F3; // Very nice and simple skew factor for 3D
    int i = floor(seed.x + s);
    int j = floor(seed.y + s);
    int k = floor(seed.z + s);
    float t = (i + j + k) * G3;
    float X0 = i - t; // Unskew the cell origin back to (x,y,z) space
    float Y0 = j - t;
    float Z0 = k - t;
    float x0 = seed.x - X0; // The x,y,z distances from the cell origin
    float y0 = seed.y - Y0;
    float z0 = seed.z - Z0;

    // For the 3D case, the simplex shape is a slightly irregular tetrahedron.
    // Determine which simplex we are in.
    int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
    int i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords
    if (x0 >= y0) {
        if (y0 >= z0) {
            i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0; // X Y Z order
        } else if (x0 >= z0) {
            i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; // X Z Y order
        } else {
            i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; // Z X Y order
        }
    } else { // x0<y0
        if (y0 < z0) {
            i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; // Z Y X order
        } else if (x0 < z0) {
            i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; // Y Z X order
        } else {
            i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; // Y X Z order
        }
    }

    // A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
    // a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
    // a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
    // c = 1/6.
    float x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
    float y1 = y0 - j1 + G3;
    float z1 = z0 - k1 + G3;
    float x2 = x0 - i2 + 2.0f * G3; // Offsets for third corner in (x,y,z) coords
    float y2 = y0 - j2 + 2.0f * G3;
    float z2 = z0 - k2 + 2.0f * G3;
    float x3 = x0 - 1.0f + 3.0f * G3; // Offsets for last corner in (x,y,z) coords
    float y3 = y0 - 1.0f + 3.0f * G3;
    float z3 = z0 - 1.0f + 3.0f * G3;

    // Work out the hashed gradient indices of the four simplex corners
    int gi0 = wangHash(i + wangHash(j + wangHash(k)));
    int gi1 = wangHash(i + i1 + wangHash(j + j1 + wangHash(k + k1)));
    int gi2 = wangHash(i + i2 + wangHash(j + j2 + wangHash(k + k2)));
    int gi3 = wangHash(i + 1 + wangHash(j + 1 + wangHash(k + 1)));

    // Calculate the contribution from the four corners
    float t0 = 0.6f - x0*x0 - y0*y0 - z0*z0;
    if (t0 < 0) {
        n0 = 0.0;
    } else {
        t0 *= t0;
        n0 = t0 * t0 * grad(gi0, x0, y0, z0);
    }
    float t1 = 0.6f - x1*x1 - y1*y1 - z1*z1;
    if (t1 < 0) {
        n1 = 0.0;
    } else {
        t1 *= t1;
        n1 = t1 * t1 * grad(gi1, x1, y1, z1);
    }
    float t2 = 0.6f - x2*x2 - y2*y2 - z2*z2;
    if (t2 < 0) {
        n2 = 0.0;
    } else {
        t2 *= t2;
        n2 = t2 * t2 * grad(gi2, x2, y2, z2);
    }
    float t3 = 0.6f - x3*x3 - y3*y3 - z3*z3;
    if (t3 < 0) {
        n3 = 0.0;
    } else {
        t3 *= t3;
        n3 = t3 * t3 * grad(gi3, x3, y3, z3);
    }
    // Add contributions from each corner to get the final noise value.
    // The result is scaled to stay just inside [-1,1]
    return 32.0f*(n0 + n1 + n2 + n3);
}


/**
 * Fractal/Fractional Brownian Motion (fBm) summation of 1D Perlin Simplex noise
 *
 * @param[in] octaves   number of fraction of noise to sum
 * @param[in] x         float coordinate
 *
 * @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
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
    float amplitude = size;

    for (int i = 0; i < octaves; i++) {
        output += (amplitude * perlinSimplexNoise(position * frequency));
        denom += amplitude;

        frequency *= lacunarity;
        amplitude *= size;
    }

    return pow(gain * output / denom, 1.0f / gamma);
}


/**
 * Fractal/Fractional Brownian Motion (fBm) summation of 1D Perlin Simplex noise
 *
 * @param[in] octaves   number of fraction of noise to sum
 * @param[in] x         float coordinate
 *
 * @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
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
    float amplitude = size;

    for (int i = 0; i < octaves; i++) {
        output += (amplitude * perlinSimplexNoise(position * frequency));
        denom += amplitude;

        frequency *= lacunarity;
        amplitude *= size;
    }

    return pow(gain * output / denom, 1.0f / gamma);
}


/**
 * Fractal/Fractional Brownian Motion (fBm) summation of 1D Perlin Simplex noise
 *
 * @param[in] octaves   number of fraction of noise to sum
 * @param[in] x         float coordinate
 *
 * @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
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
    float amplitude = size;

    for (int i = 0; i < octaves; i++) {
        output += (amplitude * perlinSimplexNoise(position * frequency));
        denom += amplitude;

        frequency *= lacunarity;
        amplitude *= size;
    }

    return pow(gain * output / denom, 1.0f / gamma);
}


/**
 * Fractal/Fractional Brownian Motion (fBm) summation of 1D Perlin Simplex noise
 *
 * @param[in] octaves   number of fraction of noise to sum
 * @param[in] x         float coordinate
 *
 * @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
 */
float turbulanceNoise(
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
    float amplitude = size;

    for (int i = 0; i < octaves; i++) {
        output += fabs(amplitude * perlinSimplexNoise(position * frequency));
        denom += amplitude;

        frequency *= lacunarity;
        amplitude *= size;
    }

    return pow(gain * output / denom, 1.0f / gamma);
}


/**
 * Fractal/Fractional Brownian Motion (fBm) summation of 1D Perlin Simplex noise
 *
 * @param[in] octaves   number of fraction of noise to sum
 * @param[in] x         float coordinate
 *
 * @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
 */
float turbulanceNoise(
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
    float amplitude = size;

    for (int i = 0; i < octaves; i++) {
        output += fabs(amplitude * perlinSimplexNoise(position * frequency));
        denom += amplitude;

        frequency *= lacunarity;
        amplitude *= size;
    }

    return pow(gain * output / denom, 1.0f / gamma);
}


/**
 * Fractal/Fractional Brownian Motion (fBm) summation of 1D Perlin Simplex noise
 *
 * @param[in] octaves   number of fraction of noise to sum
 * @param[in] x         float coordinate
 *
 * @return Noise value in the range[-1; 1], value of 0 on all integer coordinates.
 */
float turbulanceNoise(
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
    float amplitude = size;

    for (int i = 0; i < octaves; i++) {
        output += fabs(amplitude * perlinSimplexNoise(position * frequency));
        denom += amplitude;

        frequency *= lacunarity;
        amplitude *= size;
    }

    return pow(gain * output / denom, 1.0f / gamma);
}

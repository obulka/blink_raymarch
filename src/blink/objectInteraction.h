// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Signed Distance Transformations
//
// These operate on the signed distances that have been computed
//


inline float union_(
        const float distance0,
        const float distance1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &surface0,
        float4 &surface1)
{
    if (fabs(distance0) < fabs(distance1))
    {
        colour1 = colour0;
        surface1 = surface0;
        return distance0;
    }
    return distance1;
}


inline float subtraction(
        const float distance0,
        const float distance1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &surface0,
        float4 &surface1)
{
    if (-distance0 > distance1)
    {
        colour1 = colour0;
        surface1 = surface0;
        return -distance0;
    }
    return distance1;
}


inline float intersection(
        const float distance0,
        const float distance1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &surface0,
        float4 &surface1)
{
    if (distance0 > distance1)
    {
        colour1 = colour0;
        surface1 = surface0;
        return distance0;
    }
    return distance1;
}


inline float smoothUnion(
        const float distance0,
        const float distance1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &surface0,
        float4 &surface1,
        const float blendSize)
{
    float amount = saturate(0.5f + 0.5f * (fabs(distance1) - fabs(distance0)) / blendSize);

    colour1 = blend(colour0, colour1, amount);
    surface1 = blend(surface0, surface1, amount);

    return blend(distance0, distance1, amount) - blendSize * amount * (1.0f - amount);
}


inline float smoothSubtraction(
        const float distance0,
        const float distance1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &surface0,
        float4 &surface1,
        const float blendSize)
{
    float amount = saturate(0.5f - 0.5f * (distance1 + distance0) / blendSize);

    colour1 = blend(colour0, colour1, amount);
    surface1 = blend(surface0, surface1, amount);

    return blend(-distance0, distance1, amount) + blendSize * amount * (1.0f - amount);
}


inline float smoothIntersection(
        const float distance0,
        const float distance1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &surface0,
        float4 &surface1,
        const float blendSize)
{
    float amount = saturate(0.5f - 0.5f * (distance1 - distance0) / blendSize);

    colour1 = blend(colour0, colour1, amount);
    surface1 = blend(surface0, surface1, amount);

    return blend(distance0, distance1, amount) + blendSize * amount * (1.0f - amount);
}


float performChildInteraction(
        const int modifications,
        const float distance0,
        const float distance1,
        const float4 &colour0,
        float4 &colour1,
        const float4 &surface0,
        float4 &surface1,
        const float blendSize)
{
    if (modifications & 128)
    {
        return subtraction(
            distance0,
            distance1,
            colour0,
            colour1,
            surface0,
            surface1
        );
    }
    if (modifications & 256)
    {
        return intersection(
            distance0,
            distance1,
            colour0,
            colour1,
            surface0,
            surface1
        );
    }
    if (modifications & 512)
    {
        return smoothUnion(
            distance0,
            distance1,
            colour0,
            colour1,
            surface0,
            surface1,
            blendSize
        );
    }
    if (modifications & 1024)
    {
        return smoothSubtraction(
            distance0,
            distance1,
            colour0,
            colour1,
            surface0,
            surface1,
            blendSize
        );
    }
    if (modifications & 2048)
    {
        return smoothIntersection(
            distance0,
            distance1,
            colour0,
            colour1,
            surface0,
            surface1,
            blendSize
        );
    }
    return union_(
        distance0,
        distance1,
        colour0,
        colour1,
        surface0,
        surface1
    );
}

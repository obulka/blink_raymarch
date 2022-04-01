//
// Signed Distance Transformations
//
// These operate on the signed distances that have been computed
//


inline float union_(
        const float distance0,
        const float distance1)
{
    return min(distance0, distance1);
}


inline float subtraction(
        const float distance0,
        const float distance1)
{
    return max(-distance0, distance1);
}


inline float intersection(
        const float distance0,
        const float distance1)
{
    return max(distance0, distance1);
}


inline float smoothUnion(
        const float distance0,
        const float distance1,
        const float blendSize)
{
    float amount = clamp(0.5f + 0.5f * (distance1 - distance0) / blendSize, 0.0f, 1.0f);

    return blend(distance0, distance1, amount) - blendSize * amount * (1.0f - amount);
}


inline float smoothSubtraction(
        const float distance0,
        const float distance1,
        const float blendSize)
{
    float amount = clamp(0.5f - 0.5f * (distance1 + distance0) / blendSize, 0.0f, 1.0f);

    return blend(-distance0, distance1, amount) + blendSize * amount * (1.0f - amount);
}


inline float smoothIntersection(
        const float distance0,
        const float distance1,
        const float blendSize)
{
    float amount = clamp(0.5f - 0.5f * (distance1 - distance0) / blendSize, 0.0f, 1.0f);

    return blend(distance0, distance1, amount) + blendSize * amount * (1.0f - amount);
}


float performChildInteraction(
        const int modifications,
        const float distance0,
        const float distance1,
        const float blendSize)
{
    if (modifications & 128)
    {
        return subtraction(distance0, distance1);
    }
    if (modifications & 256)
    {
        return intersection(distance0, distance1);
    }
    if (modifications & 512)
    {
        return smoothUnion(distance0, distance1, blendSize);
    }
    if (modifications & 1024)
    {
        return smoothSubtraction(distance0, distance1, blendSize);
    }
    if (modifications & 2048)
    {
        return smoothIntersection(distance0, distance1, blendSize);
    }
    return union_(distance0, distance1);
}

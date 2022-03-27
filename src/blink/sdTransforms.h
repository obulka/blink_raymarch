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
        const float amount)
{
    float h = clamp(0.5f + 0.5f * (distance1 - distance0) / amount, 0.0f, 1.0f);

    return blend(distance0, distance1, h) - amount * h * (1.0f - h);
}


inline float smoothSubtraction(
        const float distance0,
        const float distance1,
        const float amount)
{
    float h = clamp(0.5f - 0.5f * (distance1 + distance0) / amount, 0.0f, 1.0f);

    return blend(-distance0, distance1, h) + amount * h * (1.0f - h);
}


inline float smoothIntersection(
        const float distance0,
        const float distance1,
        const float amount)
{
    float h = clamp(0.5f - 0.5f * (distance1 - distance0) / amount, 0.0f, 1.0f);

    return blend(distance0, distance1, h) + amount * h * (1.0f - h);
}

//
// Signed Distance Transformations
//
// These operate on the signed distances that have been computed
//


float _mix(float x, float y, float a)
{
    return x + (y - x) * a;
}


float smoothUnion(float distance0, float distance1, float amount)
{
    float h = clamp(0.5f + 0.5f * (distance1 - distance0) / amount, 0.0f, 1.0f);

    return _mix(distance1, distance0, h) - amount * h * (1.0f - h);
}


float smoothSubtraction(float distance0, float distance1, float amount)
{
    float h = clamp(0.5f - 0.5f * (distance1 + distance0) / amount, 0.0f, 1.0f);

    return _mix(distance1, -distance0, h) + amount * h * (1.0f - h);
}


float smoothIntersection(float distance0, float distance1, float amount)
{
    float h = clamp(0.5f - 0.5f * (distance1 - distance0) / amount, 0.0f, 1.0f);

    return _mix(distance1, distance0, h) + amount * h * (1.0f - h);
}

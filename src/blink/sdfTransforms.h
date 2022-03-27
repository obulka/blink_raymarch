//
// SDF Modifiers
//
// These modify the ray position before computing the SD
//


inline float3 infiniteRepetition(const float3 &position, const float3 &spacing)
{
    return fmod(position + 0.5f * spacing, spacing) - 0.5f * spacing;
}


inline float3 finiteRepetition(const float3 &position, const float3 &limits, const float c)
{
    const int3 intLimits = round(limits);
    const int3 repeat = clamp(round(position / c), -intLimits, intLimits);
    return position - c * float3(repeat.x, repeat.y, repeat.z);
}


inline float3 elongate(const float3 &position, const float3 &elongation)
{
    return position - clamp(position, -elongation, elongation);
}


inline float3 mirrorX(const float3 &position)
{
    return float3(fabs(position.x), position.y, position.z);
}


inline float3 mirrorY(const float3 &position)
{
    return float3(position.x, fabs(position.y), position.z);
}


inline float3 mirrorZ(const float3 &position)
{
    return float3(position.x, position.y, fabs(position.z));
}

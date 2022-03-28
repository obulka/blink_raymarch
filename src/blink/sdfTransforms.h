//
// SDF Modifiers
//
// These modify the ray position before computing the SD
//


inline float3 infiniteRepetition(const float3 &position, const float3 &spacing)
{
    return fmod(position + 0.5f * spacing, spacing) - 0.5f * spacing;
}


inline float3 finiteRepetition(
        const float3 &position,
        const float3 &limits,
        const float spacing)
{
    const int3 intLimits = round_(limits);
    const int3 repeat = clamp_(round_(fabs(position) / spacing), -intLimits, intLimits);
    return position - spacing * float3(repeat.x, repeat.y, repeat.z);
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


inline float roundEdges(const float distance, float radius)
{
    return distance - radius;
}


inline float hollow(const float distance, const float thickness)
{
    return fabs(distance) - thickness;
}


void performShapeModification(
        const int modifications,
        const float4 &modParameters0,
        const float4 &modParameters1,
        float3 &position)
{
    if (modifications & 1)
    {
        position = finiteRepetition(
            position,
            float3(modParameters0.x, modParameters0.y, modParameters0.z),
            modParameters0.w
        );
    }
    else if (modifications & 2)
    {
        position = infiniteRepetition(
            position,
            float3(modParameters0.x, modParameters0.y, modParameters0.z)
        );
    }
    if (modifications & 4)
    {
        position = elongate(
            position,
            float3(modParameters1.x, modParameters1.y, modParameters1.z)
        );
    }
    if (modifications & 8)
    {
        position = mirrorX(position);
    }
    if (modifications & 16)
    {
        position = mirrorY(position);
    }
    if (modifications & 32)
    {
        position = mirrorZ(position);
    }
}


float performDistanceModification(
        const int modifications,
        const float edgeRadius,
        const float wallThickness,
        const float distance)
{
    float result = distance;
    if (modifications & 64)
    {
        result = roundEdges(result, edgeRadius);
    }
    if (modifications & 128)
    {
        result = hollow(result, wallThickness);
    }
    return result;
}

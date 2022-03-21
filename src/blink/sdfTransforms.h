//
// SDF Modifiers
//
// These modify the ray position before computing the SD
//


inline float3 positionToInfiniteRepetition(const float3 &position, const float3 &spacing)
{
    return fmod(position + 0.5f * spacing, spacing) - 0.5f * spacing;
}

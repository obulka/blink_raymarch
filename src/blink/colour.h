//
// Colour functions
//


float2 smoothMinN(float colour0, float colour1, float blendSize, float exponent)
{
    float m = 0.5f * pow(
        max(blendSize - fabs(colour0 - colour1), 0.0f) / blendSize,
        exponent
    );

    float s = m * blendSize / exponent;

    return (colour0 < colour1) ? float2(colour0 - s, m) : float2(colour1 - s, m - 1.0f);
}

//
// Handle Various Lights
//


void directionalLightData(
        const float3 &pointOnSurface,
        const float3 &surfaceNormal,
        const float3 &light,
        const float hitTolerance,
        const float maxRayDistance,
        const float shadowBias,
        float3 &lightDirection,
        float3 &surfaceOffset,
        float3 &shadowOffsetLightDirection,
        float &distanceToLight)
{
    lightDirection = -light;
    surfaceOffset = (
        hitTolerance * shadowBias * (lightDirection + surfaceNormal) + pointOnSurface
    );
    shadowOffsetLightDirection = lightDirection;

    distanceToLight = maxRayDistance;
}


void pointLightData(
        const float3 &pointOnSurface,
        const float3 &surfaceNormal,
        const float3 &light,
        const float hitTolerance,
        const float shadowBias,
        float3 &lightDirection,
        float3 &surfaceOffset,
        float3 &shadowOffsetLightDirection,
        float &distanceToLight)
{
    lightDirection = light - pointOnSurface;
    surfaceOffset = (
        hitTolerance * shadowBias * (lightDirection + surfaceNormal) + pointOnSurface
    );
    shadowOffsetLightDirection = light - surfaceOffset;

    distanceToLight = length(shadowOffsetLightDirection);
}


float getLightData(
        const float3 &pointOnSurface,
        const float3 &surfaceNormal,
        const float3 &light,
        const int lightType,
        const float intensity,
        const float hitTolerance,
        const float maxRayDistance,
        const float shadowBias,
        float &distanceToLight,
        float3 &surfaceOffset,
        float3 &lightDirection,
        float3 &shadowOffsetLightDirection)
{
    if (lightType == 0)
    {
        // directional
        directionalLightData(
            pointOnSurface,
            surfaceNormal,
            light,
            hitTolerance,
            maxRayDistance,
            shadowBias,
            lightDirection,
            surfaceOffset,
            shadowOffsetLightDirection,
            distanceToLight
        );
        return intensity;
    }
    else
    {
        // point
        pointLightData(
            pointOnSurface,
            surfaceNormal,
            light,
            hitTolerance,
            shadowBias,
            lightDirection,
            surfaceOffset,
            shadowOffsetLightDirection,
            distanceToLight
        );
        return intensity / pow(distanceToLight, 4);
    }
}

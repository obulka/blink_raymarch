//
// Handle Various Lights
//


inline void directionalLightData(
        const float3 &surfaceNormal,
        const float3 &light,
        const float maxRayDistance,
        float3 &lightDirection,
        float3 &shadowOffsetLightDirection,
        float &distanceToLight)
{
    lightDirection = -light;
    shadowOffsetLightDirection = lightDirection;
    distanceToLight = maxRayDistance;
}


inline void pointLightData(
        const float3 &pointOnSurface,
        const float3 &surfaceNormal,
        const float3 &light,
        float3 &lightDirection,
        float3 &shadowOffsetLightDirection,
        float &distanceToLight)
{
    lightDirection = light - pointOnSurface;
    shadowOffsetLightDirection = lightDirection;
    distanceToLight = length(shadowOffsetLightDirection);
}


inline void domeLightData(
        const float3 &surfaceNormal,
        const float maxRayDistance,
        const float3 &light,
        float3 &lightDirection,
        float3 &shadowOffsetLightDirection,
        float &distanceToLight)
{
    lightDirection = surfaceNormal;
    shadowOffsetLightDirection = lightDirection;
    distanceToLight = maxRayDistance;
}


float getLightData(
        const float3 &pointOnSurface,
        const float3 &surfaceNormal,
        const float3 &light,
        const int lightType,
        const float intensity,
        const float falloff,
        const float maxRayDistance,
        float &distanceToLight,
        float3 &lightDirection,
        float3 &shadowOffsetLightDirection)
{
    if (lightType == 1)
    {
        // directional
        directionalLightData(
            surfaceNormal,
            light,
            maxRayDistance,
            lightDirection,
            shadowOffsetLightDirection,
            distanceToLight
        );
    }
    else if (lightType == 2)
    {
        // point
        pointLightData(
            pointOnSurface,
            surfaceNormal,
            light,
            lightDirection,
            shadowOffsetLightDirection,
            distanceToLight
        );
    }
    else if (lightType == 100)
    {
        // dome
        domeLightData(
            surfaceNormal,
            maxRayDistance,
            lightDirection,
            shadowOffsetLightDirection,
            distanceToLight
        );
    }

    return intensity / pow(distanceToLight, falloff);
}

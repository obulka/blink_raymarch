//
// Camera Utilities
//
//#include "mmath.h"


float fieldOfView(const float focalLength)
{
    return 2 * atan(1 / focalLength);
}


float aspectRatio(const float height_, const float width_)
{
    return height_ / width_;
}


float2 pixelsToUV(const float2 &pixelLocation, const float2 &format)
{
    return float2(
        2.0f * pixelLocation.x / format.x - 1.0f,
        2.0f * pixelLocation.y / format.y - 1.0f
    );
}


/**
 * Create a projection matrix for the camera
 */
float4x4 projectionMatrix(
        const float focalLength,
        const float horizontalAperture,
        const float aspect,
        const float nearPlane,
        const float farPlane)
{
    float farMinusNear = farPlane - nearPlane;
    return float4x4(
        2 * focalLength / horizontalAperture, 0, 0, 0,
        0, 2 * focalLength / horizontalAperture / aspect, 0, 0,
        0, 0, -(farPlane + nearPlane) / farMinusNear, -2 * (farPlane * nearPlane) / farMinusNear,
        0, 0, -1, 0
    );
}

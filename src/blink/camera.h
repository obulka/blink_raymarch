// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Camera Utilities
//


float fieldOfView(const float focalLength)
{
    return 2 * atan(1 / focalLength);
}


float aspectRatio(const float height_, const float width_)
{
    return height_ / width_;
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

// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Camera Utilities
//


/**
 * Compute the field of view from focal length.
 *
 * @arg focalLength: The focal length.
 *
 * @returns: The equivalent field of view.
 */
inline float fieldOfView(const float &focalLength)
{
    return 2 * atan(1 / focalLength);
}


/**
 * Compute the aspect ratio from image format.
 *
 * @arg height_: The height of the image.
 * @arg width_: The width of the image.
 *
 * @returns: The aspect ratio.
 */
inline float aspectRatio(const float &height_, const float &width_)
{
    return height_ / width_;
}


/**
 *
 */
inline float fStopToAperture(const float &fStop, const float &focalLength)
{
    return focalLength / fStop / 1000.0f;
}


/**
 * Create a projection matrix for a camera.
 *
 * @arg focalLength: The focal length of the camera.
 * @arg horizontalAperture: The horizontal aperture of the camera.
 * @arg aspect: The aspect ratio of the camera.
 * @arg nearPlane: The distance to the near plane of the camera.
 * @arg farPlane: The distance to the far plane of the camera.
 *
 * @returns: The camera's projection matrix.
 */
float4x4 projectionMatrix(
        const float &focalLength,
        const float &horizontalAperture,
        const float &aspect,
        const float &nearPlane,
        const float &farPlane)
{
    float farMinusNear = farPlane - nearPlane;
    return float4x4(
        2 * focalLength / horizontalAperture, 0, 0, 0,
        0, 2 * focalLength / horizontalAperture / aspect, 0, 0,
        0, 0, -(farPlane + nearPlane) / farMinusNear, -2 * (farPlane * nearPlane) / farMinusNear,
        0, 0, -1, 0
    );
}


/**
 * Generate a ray out of a camera.
 *
 * @arg cameraWorldMatrix: The camera matrix.
 * @arg inverseProjectionMatrix: The inverse of the projection matrix.
 * @arg uvPosition: The UV position in the resulting image.
 * @arg rayOrigin: Will store the origin of the ray.
 * @arg rayDirection: Will store the direction of the ray.
 */
void createCameraRay(
        const float4x4 &cameraWorldMatrix,
        const float4x4 &inverseProjectionMatrix,
        const float2 &uvPosition,
        float3 &rayOrigin,
        float3 &rayDirection)
{
    positionFromWorldMatrix(cameraWorldMatrix, rayOrigin);
    float4 direction = matmul(
        inverseProjectionMatrix,
        float4(uvPosition.x, uvPosition.y, 0, 1)
    );
    matmul(
        cameraWorldMatrix,
        float4(direction.x, direction.y, direction.z, 0),
        direction
    );
    rayDirection = normalize(float3(direction.x, direction.y, direction.z));
}


/**
 * Generate a ray out of a camera.
 *
 * @arg cameraWorldMatrix: The camera matrix.
 * @arg inverseProjectionMatrix: The inverse of the projection matrix.
 * @arg uvPosition: The UV position in the resulting image.
 * @arg rayOrigin: Will store the origin of the ray.
 * @arg rayDirection: Will store the direction of the ray.
 */
void createCameraRay(
        const float4x4 &cameraWorldMatrix,
        const float4x4 &inverseProjectionMatrix,
        const float2 &uvPosition,
        const float &aperture,
        const float &focalDistance,
        const float3 &seed,
        float3 &rayOrigin,
        float3 &rayDirection)
{
    createCameraRay(
        cameraWorldMatrix,
        inverseProjectionMatrix,
        uvPosition,
        rayOrigin,
        rayDirection
    );

    const float4 cameraForward4 = matmul(
        cameraWorldMatrix,
        float4(0, 0, -1, 0)
    );
    const float3 cameraForward = float3(
        cameraForward4.x,
        cameraForward4.y,
        cameraForward4.z
    );
    const float4 cameraRight4 = matmul(
        cameraWorldMatrix,
        float4(1, 0, 0, 0)
    );
    const float3 cameraRight = float3(
        cameraRight4.x,
        cameraRight4.y,
        cameraRight4.z
    );
    const float4 cameraUp4 = matmul(
        cameraWorldMatrix,
        float4(0, 1, 0, 0)
    );
    const float3 cameraUp = float3(
        cameraUp4.x,
        cameraUp4.y,
        cameraUp4.z
    );

    const float3 focalPlanePoint = rayOrigin + cameraForward * focalDistance;
    const float3 focalPlaneNormal = -cameraForward;

    const float focalPointDistance = (
        (dot(focalPlaneNormal, focalPlanePoint) - dot(rayOrigin, focalPlaneNormal))
        / dot(rayDirection, focalPlaneNormal)
    );
    const float3 focalPoint = rayOrigin + focalPointDistance * rayDirection;

    const float2 pointInUnitCircle = uniformPointInUnitCircle(seed);
    const float2 offset = pointInUnitCircle.x * aperture * float2(
        cos(pointInUnitCircle.y),
        sin(pointInUnitCircle.y)
    );

    rayOrigin += cameraRight * offset.x + cameraUp * offset.y;
    rayDirection = normalize(focalPoint - rayOrigin);
}


/**
 * Generate a LatLong ray out of a camera.
 *
 * @arg cameraWorldMatrix: The camera matrix.
 * @arg uvPosition: The UV position in the resulting image.
 * @arg rayOrigin: Will store the origin of the ray.
 * @arg rayDirection: Will store the direction of the ray.
 */
void createLatLongCameraRay(
        const float4x4 &cameraWorldMatrix,
        const float2 &uvPosition,
        float3 &rayOrigin,
        float3 &rayDirection)
{
    positionFromWorldMatrix(cameraWorldMatrix, rayOrigin);
    rayDirection = sphericalUnitVectorToCartesion(uvPositionToAngles(uvPosition));

    float3x3 cameraRotation;
    rotationFromWorldMatrix(cameraWorldMatrix, cameraRotation);
    rayDirection = matmul(cameraRotation, rayDirection);
}

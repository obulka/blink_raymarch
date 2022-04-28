// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Matrix and vector math operations
//


inline float degreesToRadians(const float angle)
{
    return angle * PI / 180.0f;
}


inline float radiansToDegrees(const float angle)
{
    return angle * 180.0f / PI;
}


inline float2 degreesToRadians(const float2 &angle)
{
    return angle * PI / 180.0f;
}


inline float2 radiansToDegrees(const float2 &angle)
{
    return angle * 180.0f / PI;
}


inline float3 degreesToRadians(const float3 &angle)
{
    return angle * PI / 180.0f;
}


inline float3 radiansToDegrees(const float3 &angle)
{
    return angle * 180.0f / PI;
}


inline float fract(float value)
{
    return value - floor(value);
}


inline float random(float seed)
{
    return fract(sin(seed * 91.3458f) * 47453.5453f);
}


inline float min(const float value0, const float value1, const float value2)
{
    return min(value0, min(value1, value2));
}


inline float max(const float value0, const float value1, const float value2)
{
    return max(value0, max(value1, value2));
}


/**
 * Why tf is this necessary? Built-in clamp fails to compile for int3s
 */
inline int3 clamp_(const int3 value, const int3 lower, const int3 upper)
{
    int3 result;
    result.x = clamp(value.x, lower.x, upper.x);
    result.y = clamp(value.y, lower.y, upper.y);
    result.z = clamp(value.z, lower.y, upper.z);

    return result;
}


/**
 * Why tf is this necessary? Built-in round fails to compile for int3s
 */
inline int3 round_(const float3 value0)
{
    return int3(round(value0.x), round(value0.y), round(value0.z));
}


inline float maxComponent(const float3 &vector)
{
    return max(vector.x, max(vector.y, vector.z));
}


inline float maxComponent(const float2 &vector)
{
    return max(vector.x, vector.y);
}


inline float minComponent(const float3 &vector)
{
    return min(vector.x, min(vector.y, vector.z));
}


inline float minComponent(const float2 &vector)
{
    return min(vector.x, vector.y);
}


inline float4 positivePart(const float4 &vector)
{
    return max(vector, float4(0));
}


inline float3 positivePart(const float3 &vector)
{
    return max(vector, float3(0));
}


inline float2 positivePart(const float2 &vector)
{
    return max(vector, float2(0));
}


inline float positivePart(const float value)
{
    return max(value, 0.0f);
}


inline float3 negativePart(const float3 &vector)
{
    return -min(vector, float3(0));
}


inline float2 negativePart(const float2 &vector)
{
    return -min(vector, float2(0));
}


inline float negativePart(const float value)
{
    return -min(value, 0.0f);
}


inline void rotationMatrix(const float3 &angles, float3x3 &out)
{
    // Why tf can I not init a float3x3 normally??
    out[0][0] = cos(angles.y) * cos(angles.z);
    out[0][1] = sin(angles.x) * sin(angles.y) * cos(angles.z) - cos(angles.x) * sin(angles.z);
    out[0][2] = cos(angles.x) * sin(angles.y) * cos(angles.z) + sin(angles.x) * sin(angles.z);
    out[1][0] = cos(angles.y) * sin(angles.z);
    out[1][1] = sin(angles.x) * sin(angles.y) * sin(angles.z) + cos(angles.x) * cos(angles.z);
    out[1][2] = cos(angles.x) * sin(angles.y) * sin(angles.z) - sin(angles.x) * cos(angles.z);
    out[2][0] = -sin(angles.y);
    out[2][1] = sin(angles.x) * cos(angles.y);
    out[2][2] = cos(angles.x) * cos(angles.y);
}


/**
 * Multiply a 4d vector by a 4x4 matrix.
 *
 * @arg m: The matrix that will transform the vector
 * @arg v: The vector to transform
 * @arg out: The location to store the resulting vector
 */
inline void matmul(const float4x4 &m, const float4 &v, float4 &out)
{
    for (int i=0; i < 4; i++)
    {
        out[i] = 0;

        for (int j=0; j < 4; j++)
        {
            out[i] += m[i][j] * v[j];
        }
    }
}


/**
 * Multiply a 3d vector by a 3x3 matrix.
 *
 * @arg m: The matrix that will transform the vector
 * @arg v: The vector to transform
 * @arg out: The location to store the resulting vector
 */
inline void matmul(const float3x3 &m, const float3 &v, float3 &out)
{
    out = float3(
        m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
        m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
        m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
    );
}


inline float3 offsetPoint(
        const float3 &point,
        const float3 &direction,
        const float offset)
{
    return offset * direction + point;
}


inline float3 surfaceOffsetPoint(
        const float3 &surfacePoint,
        const float3 &direction,
        const float3 &normal,
        const float bias,
        const float tolerance)
{
    return tolerance * bias * (direction + normal) + surfacePoint;
}


float3 roughen(
    const float3 &normal,
    const float roughness,
    const float3 &seed)
{
    const float xAngle = random(seed.x);
    const float yAngle = random(seed.y);
    const float zAngle = random(seed.z);

    float3x3 rotation;
    rotationMatrix(
        PI * roughness * (float3(xAngle, yAngle, zAngle) - 0.5f) / 2.0f,
        rotation
    );

    float3 roughened;
    matmul(rotation, normal, roughened);

    return normalize(roughened);
}


float3 roughen1(
    const float3 &normal,
    const float roughness,
    const float seed)
{
    // Use roughness to modify surface normal
    const float radialOffset = roughness * random(seed);
    const float angularOffset = 2.0f * PI * random(radialOffset * seed);

    // Update the ray direction to send a scattered cluster of rays
    float3 rayNormal = normalize(cross(normal, float3(1, 0, 0)));
    if (length(rayNormal) == 0.0f)
    {
        rayNormal = cross(normal, float3(0, 1, 0));
    }
    rayNormal = (
        cos(angularOffset)
        * rayNormal
        + sin(angularOffset)
        * cross(normal, rayNormal)
    );

    return normalize(radialOffset * rayNormal + normal);
}


inline float distanceToYAxis(const float3 &position)
{
    return length(float2(position.x, position.z));
}


/**
 * Compute the signed distance along a vector
 *
 * @arg vector: A vector from a point to the nearest surface of an
 *     object.
 *
 * @returns: The signed length of the vector.
 */
inline float sdfLength(const float2 &vector)
{
    return (
        length(positivePart(vector))
        - negativePart(maxComponent(vector))
    );
}


/**
 * Compute the signed distance along a vector
 *
 * @arg vector: A vector from a point to the nearest surface of an
 *     object.
 *
 * @returns: The signed length of the vector.
 */
inline float sdfLength(const float3 &vector)
{
    return (
        length(positivePart(vector))
        - negativePart(maxComponent(vector))
    );
}


/**
 * Get the length of the shorter of two vectors.
 *
 * @arg vector0: The first vector to get the length of if it is the
 *     shortest option
 * @arg vector1: The second vector to get the length of if it is the
 *     shortest option
 *
 * @returns: The shorter of the two lengths
 */
inline float minLength(const float2 &vector0, const float2 &vector1)
{
    return sqrt(min(dot(vector0, vector0), dot(vector1, vector1)));
}


/**
 * Get the length of the shorter of two vectors.
 *
 * @arg vector0: The first vector to get the length of if it is the
 *     shortest option
 * @arg vector1: The second vector to get the length of if it is the
 *     shortest option
 *
 * @returns: The shorter of the two lengths
 */
inline float minLength(const float3 &vector0, const float3 &vector1)
{
    return sqrt(min(dot(vector0, vector0), dot(vector1, vector1)));
}


/**
 * Get the value of sky the ray would hit at infinite distance
 *
 * @returns: Cylindrical coordinates without angle, (r, h)
 */
inline float2 cartesianToCylindrical(const float3 &coordinates)
{
    return float2(distanceToYAxis(coordinates), coordinates.y);
}


inline float3 sphericalUnitVectorToCartesion(const float2 &angles)
{
    const float sinPhi = sin(angles.y);
    return float3(
        cos(angles.x) * sinPhi,
        sin(angles.x) * sinPhi,
        cos(angles.y)
    );
}


inline float2 normalizeAngles(const float2 &angles)
{
    float2 normalizedAngles = float2(
        fmod(angles.x, 2.0f * PI),
        fmod(angles.y, PI)
    );
    normalizedAngles.x += 2 * PI * (normalizedAngles.x < 0);
    normalizedAngles.y += PI * (normalizedAngles.y < 0);

    return normalizedAngles;
}


float2 cartesionUnitVectorToSpherical(const float3 &rayDirection, const float thetaOffset)
{
    return normalizeAngles(float2(
        atan2(rayDirection.z, rayDirection.x) + thetaOffset,
        acos(rayDirection.y)
    ));
}


float2 cartesionUnitVectorToSpherical(const float3 &rayDirection)
{
    return normalizeAngles(float2(
        atan2(rayDirection.y, rayDirection.x),
        acos(rayDirection.z)
    ));
}


float sphericalUnitDot(const float2 &vector0, const float2 &vector1)
{
    return (
        cos(vector0.x) * cos(vector1.x)
        + cos(vector0.y - vector1.y) * sin(vector0.x) * sin(vector1.x)
    );
}


inline float2 uvPositionToAngles(const float2 &uvPosition)
{
    return float2(
        (uvPosition.x + 1.0f) * PI,
        (1.0f - uvPosition.y) * PI / 2.0f
    );
}


inline float2 pixelsToUV(const float2 &pixelLocation, const float2 &format)
{
    return float2(
        2.0f * pixelLocation.x / format.x - 1.0f,
        2.0f * pixelLocation.y / format.y - 1.0f
    );
}


/**
 * Saturate a value ie. clamp between 0 and 1
 *
 * @args value: The value to saturate
 *
 * @returns: The clamped value
 */
inline float saturate(float value)
{
    return clamp(value, 0.0f, 1.0f);
}


/**
 * Saturate a value ie. clamp between 0 and 1
 *
 * @args value: The value to saturate
 *
 * @returns: The clamped value
 */
inline float2 saturate(const float2 &value)
{
    return clamp(value, float2(0), float2(1));
}


/**
 * Saturate a value ie. clamp between 0 and 1
 *
 * @args value: The value to saturate
 *
 * @returns: The clamped value
 */
inline float3 saturate(const float3 &value)
{
    return clamp(value, float3(0), float3(1));
}


/**
 * Saturate a value ie. clamp between 0 and 1
 *
 * @args value: The value to saturate
 *
 * @returns: The clamped value
 */
inline float4 saturate(const float4 &value)
{
    return clamp(value, float4(0), float4(1));
}


float2 smoothMinN(float value0, float value1, float blendSize, float exponent)
{
    float m = 0.5f * pow(
        positivePart(blendSize - fabs(value0 - value1)) / blendSize,
        exponent
    );

    float s = m * blendSize / exponent;

    return (value0 < value1) ? float2(value0 - s, m) : float2(value1 - s, m - 1.0f);
}


inline float blend(const float value0, const float value1, const float weight)
{
    return value1 + weight * (value0 - value1);
}


inline float3 blend(const float3 &value0, const float3 &value1, const float weight)
{
    return value1 + weight * (value0 - value1);
}


inline float4 blend(const float4 &value0, const float4 &value1, const float weight)
{
    return value1 + weight * (value0 - value1);
}


/**
 * Get the position component of a world matrix.
 *
 * @arg worldMatrix: The world matrix
 * @arg position: The location to store the position
 */
inline void positionFromWorldMatrix(const float4x4 &worldMatrix, float3 &position)
{
    position = float3(
        worldMatrix[0][3],
        worldMatrix[1][3],
        worldMatrix[2][3]
    );
}


inline float3 reflectRayOffSurface(
    const float3 &incidentRayDirection,
    const float3 &surfaceNormalDirection)
{
    return (
        incidentRayDirection
        - 2 * dot(incidentRayDirection, surfaceNormalDirection) * surfaceNormalDirection
    );
}


inline float3 refractRayThroughSurface(
    const float3 &incidentRayDirection,
    const float3 &surfaceNormalDirection,
    const float incidentRefractiveIndex,
    const float refractedRefractiveIndex)
{
    const float refractiveRatio = incidentRefractiveIndex / refractedRefractiveIndex;
    const float cosIncident = -dot(incidentRayDirection, surfaceNormalDirection);
    const float sinTransmittedSquared = refractiveRatio * refractiveRatio * (
        1.0f - cosIncident * cosIncident
    );
    if (sinTransmittedSquared > 1.0f)
    {
        return reflectRayOffSurface(incidentRayDirection, surfaceNormalDirection);
    }
    const float cosTransmitted = sqrt(1.0f - sinTransmittedSquared);
    return (
        refractiveRatio * incidentRayDirection
        + (refractiveRatio * cosIncident - cosTransmitted) * surfaceNormalDirection
    );
}


inline float schlickReflectionCoefficient(
    const float3 &incidentRayDirection,
    const float3 &surfaceNormalDirection,
    const float incidentRefractiveIndex,
    const float refractedRefractiveIndex)
{
    const float parallelCoefficient = pow(
        (incidentRefractiveIndex - refractedRefractiveIndex)
        / (incidentRefractiveIndex + refractedRefractiveIndex),
        2
    );
    float cosX = -dot(incidentRayDirection, surfaceNormalDirection);
    if (incidentRefractiveIndex > refractedRefractiveIndex)
    {
        const float refractiveRatio = incidentRefractiveIndex / refractedRefractiveIndex;
        const float sinTransmittedSquared = refractiveRatio * refractiveRatio * (
            1.0f - cosX * cosX
        );
        if (sinTransmittedSquared > 1.0f)
        {
            return 1.0f;
        }
        cosX = sqrt(1.0f - sinTransmittedSquared);
    }
    return parallelCoefficient + (1 - parallelCoefficient) * pow(1.0f - cosX, 5);
}


inline float reflectionCoefficient(
    const float3 &incidentRayDirection,
    const float3 &surfaceNormalDirection,
    const float incidentRefractiveIndex,
    const float refractedRefractiveIndex)
{
    const float refractiveRatio = incidentRefractiveIndex / refractedRefractiveIndex;

    const float cosIncident = -dot(incidentRayDirection, surfaceNormalDirection);

    const float sinTransmittedSquared = refractiveRatio * refractiveRatio * (
        1.0f - cosIncident * cosIncident
    );

    if (sinTransmittedSquared > 1.0f)
    {
        return 1.0f;
    }

    const float cosTransmitted = sqrt(1.0f - sinTransmittedSquared);

    const float orthogonalReflectance = (
        (incidentRefractiveIndex * cosIncident - refractedRefractiveIndex * cosTransmitted)
        / (incidentRefractiveIndex * cosIncident + refractedRefractiveIndex * cosTransmitted)
    );

    const float parallelReflectance = (
        (refractedRefractiveIndex * cosIncident - incidentRefractiveIndex * cosTransmitted)
        / (refractedRefractiveIndex * cosIncident + incidentRefractiveIndex * cosTransmitted)
    );

    return (
        orthogonalReflectance * orthogonalReflectance
        + parallelReflectance * parallelReflectance
    ) / 2.0f;
}


/**
 * Generate a ray out of the camera
 */
void createCameraRay(
        const float4x4 &cameraWorldMatrix,
        const float4x4 &inverseProjectionMatrix,
        const float2 &uvPosition,
        float3 &rayOrigin,
        float3 &rayDirection)
{
    positionFromWorldMatrix(cameraWorldMatrix, rayOrigin);
    float4 direction;
    matmul(
        inverseProjectionMatrix,
        float4(uvPosition.x, uvPosition.y, 0, 1),
        direction
    );
    matmul(
        cameraWorldMatrix,
        float4(direction.x, direction.y, direction.z, 0),
        direction
    );
    rayDirection = normalize(float3(direction.x, direction.y, direction.z));
}


/**
 * Invert a 4x4 matrix.
 * 
 * Turns out float4x4 has a .invert() method so this is 
 *
 * @arg m: The matrix to invert
 * @arg invOut: The location to store the inverted matrix
 *
 * @returns: true if it was possible to invert the matrix, flase otherwise
 */
bool invert4x4(const float4x4 &m, float4x4 &invOut)
{
    float4x4 inv;
    float det;

    inv[0][0] =
        m[1][1] * m[2][2] * m[3][3] - 
        m[1][1] * m[2][3] * m[3][2] - 
        m[2][1] * m[1][2] * m[3][3] + 
        m[2][1] * m[1][3] * m[3][2] +
        m[3][1] * m[1][2] * m[2][3] - 
        m[3][1] * m[1][3] * m[2][2];

    inv[1][0] =
        -m[1][0] * m[2][2] * m[3][3] + 
        m[1][0] * m[2][3] * m[3][2] + 
        m[2][0] * m[1][2] * m[3][3] - 
        m[2][0] * m[1][3] * m[3][2] - 
        m[3][0] * m[1][2] * m[2][3] + 
        m[3][0] * m[1][3] * m[2][2];

    inv[2][0] =
        m[1][0] * m[2][1] * m[3][3] - 
        m[1][0] * m[2][3] * m[3][1] - 
        m[2][0] * m[1][1] * m[3][3] + 
        m[2][0] * m[1][3] * m[3][1] + 
        m[3][0] * m[1][1] * m[2][3] - 
        m[3][0] * m[1][3] * m[2][1];

    inv[3][0] =
        -m[1][0] * m[2][1] * m[3][2] + 
        m[1][0] * m[2][2] * m[3][1] +
        m[2][0] * m[1][1] * m[3][2] - 
        m[2][0] * m[1][2] * m[3][1] - 
        m[3][0] * m[1][1] * m[2][2] + 
        m[3][0] * m[1][2] * m[2][1];

    inv[0][1] =
        -m[0][1] * m[2][2] * m[3][3] + 
        m[0][1] * m[2][3] * m[3][2] + 
        m[2][1] * m[0][2] * m[3][3] - 
        m[2][1] * m[0][3] * m[3][2] - 
        m[3][1] * m[0][2] * m[2][3] + 
        m[3][1] * m[0][3] * m[2][2];

    inv[1][1] =
        m[0][0] * m[2][2] * m[3][3] - 
        m[0][0] * m[2][3] * m[3][2] - 
        m[2][0] * m[0][2] * m[3][3] + 
        m[2][0] * m[0][3] * m[3][2] + 
        m[3][0] * m[0][2] * m[2][3] - 
        m[3][0] * m[0][3] * m[2][2];

    inv[2][1] = 
        m[0][0] * m[2][1] * m[3][3] + 
        m[0][0] * m[2][3] * m[3][1] + 
        m[2][0] * m[0][1] * m[3][3] - 
        m[2][0] * m[0][3] * m[3][1] - 
        m[3][0] * m[0][1] * m[2][3] + 
        m[3][0] * m[0][3] * m[2][1];

    inv[3][1] =
        m[0][0] * m[2][1] * m[3][2] - 
        m[0][0] * m[2][2] * m[3][1] - 
        m[2][0] * m[0][1] * m[3][2] + 
        m[2][0] * m[0][2] * m[3][1] + 
        m[3][0] * m[0][1] * m[2][2] - 
        m[3][0] * m[0][2] * m[2][1];

    inv[0][2] =
        m[0][1] * m[1][2] * m[3][3] - 
        m[0][1] * m[1][3] * m[3][2] - 
        m[1][1] * m[0][2] * m[3][3] + 
        m[1][1] * m[0][3] * m[3][2] + 
        m[3][1] * m[0][2] * m[1][3] - 
        m[3][1] * m[0][3] * m[1][2];

    inv[1][2] =
        -m[0][0] * m[1][2] * m[3][3] + 
        m[0][0] * m[1][3] * m[3][2] + 
        m[1][0] * m[0][2] * m[3][3] - 
        m[1][0] * m[0][3] * m[3][2] - 
        m[3][0] * m[0][2] * m[1][3] + 
        m[3][0] * m[0][3] * m[1][2];

    inv[2][2] =
        m[0][0] * m[1][1] * m[3][3] - 
        m[0][0] * m[1][3] * m[3][1] - 
        m[1][0] * m[0][1] * m[3][3] + 
        m[1][0] * m[0][3] * m[3][1] + 
        m[3][0] * m[0][1] * m[1][3] - 
        m[3][0] * m[0][3] * m[1][1];

    inv[3][2] =
        -m[0][0] * m[1][1] * m[3][2] + 
        m[0][0] * m[1][2] * m[3][1] + 
        m[1][0] * m[0][1] * m[3][2] - 
        m[1][0] * m[0][2] * m[3][1] - 
        m[3][0] * m[0][1] * m[1][2] + 
        m[3][0] * m[0][2] * m[1][1];

    inv[0][3] =
        -m[0][1] * m[1][2] * m[2][3] + 
        m[0][1] * m[1][3] * m[2][2] + 
        m[1][1] * m[0][2] * m[2][3] - 
        m[1][1] * m[0][3] * m[2][2] - 
        m[2][1] * m[0][2] * m[1][3] + 
        m[2][1] * m[0][3] * m[1][2];

    inv[1][3] =
        m[0][0] * m[1][2] * m[2][3] - 
        m[0][0] * m[1][3] * m[2][2] - 
        m[1][0] * m[0][2] * m[2][3] + 
        m[1][0] * m[0][3] * m[2][2] + 
        m[2][0] * m[0][2] * m[1][3] - 
        m[2][0] * m[0][3] * m[1][2];

    inv[2][3] =
        -m[0][0] * m[1][1] * m[2][3] + 
        m[0][0] * m[1][3] * m[2][1] + 
        m[1][0] * m[0][1] * m[2][3] - 
        m[1][0] * m[0][3] * m[2][1] - 
        m[2][0] * m[0][1] * m[1][3] + 
        m[2][0] * m[0][3] * m[1][1];

    inv[3][3] =
        m[0][0] * m[1][1] * m[2][2] - 
        m[0][0] * m[1][2] * m[2][1] - 
        m[1][0] * m[0][1] * m[2][2] + 
        m[1][0] * m[0][2] * m[2][1] + 
        m[2][0] * m[0][1] * m[1][2] - 
        m[2][0] * m[0][2] * m[1][1];

    det = m[0][0] * inv[0][0] + m[0][1] * inv[1][0] + m[0][2] * inv[2][0] + m[0][3] * inv[3][0];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (int i=0; i < 4; i++)
    {
        for (int j=0; j < 4; j++)
        {
            invOut[i][j] = inv[i][j] * det;
        }
    }

    return true;
}

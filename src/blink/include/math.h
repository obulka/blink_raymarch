// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Matrix and vector math operations
//


/**
 * Convert degrees to radians.
 *
 * @arg angle: The angle in degrees.
 *
 * @returns: The angle in radians.
 */
inline float degreesToRadians(const float angle)
{
    return angle * PI / 180.0f;
}


/**
 * Convert radians to degrees.
 *
 * @arg angle: The angle in radians.
 *
 * @returns: The angle in degrees.
 */
inline float radiansToDegrees(const float angle)
{
    return angle * 180.0f / PI;
}


/**
 * Convert degrees to radians.
 *
 * @arg angle: The angle in degrees.
 *
 * @returns: The angle in radians.
 */
inline float2 degreesToRadians(const float2 &angle)
{
    return angle * PI / 180.0f;
}


/**
 * Convert radians to degrees.
 *
 * @arg angle: The angle in radians.
 *
 * @returns: The angle in degrees.
 */
inline float2 radiansToDegrees(const float2 &angle)
{
    return angle * 180.0f / PI;
}


/**
 * Convert degrees to radians.
 *
 * @arg angle: The angle in degrees.
 *
 * @returns: The angle in radians.
 */
inline float3 degreesToRadians(const float3 &angle)
{
    return angle * PI / 180.0f;
}


/**
 * Convert radians to degrees.
 *
 * @arg angle: The angle in radians.
 *
 * @returns: The angle in degrees.
 */
inline float3 radiansToDegrees(const float3 &angle)
{
    return angle * 180.0f / PI;
}


/**
 * Compute the fractional portion of the value. Ex. 3.5 returns 0.5
 *
 * @arg value: The value to get the fractional portion of.
 *
 * @returns: The fractional portion of the value.
 */
inline float fract(const float value)
{
    return value - floor(value);
}


/**
 * The minimum of three values.
 *
 * @arg value0: The first value.
 * @arg value1: The second value.
 * @arg value2: The third value.
 *
 * @returns: The minimum of the three values.
 */
inline float min(const float value0, const float value1, const float value2)
{
    return min(value0, min(value1, value2));
}


/**
 * The maximum of three values.
 *
 * @arg value0: The first value.
 * @arg value1: The second value.
 * @arg value2: The third value.
 *
 * @returns: The maximum of the three values.
 */
inline float max(const float value0, const float value1, const float value2)
{
    return max(value0, max(value1, value2));
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


/**
 * Custom clamp for int3 because the builtin clamp fails to compile for
 * int3.
 *
 * @arg value: The value to clamp.
 * @arg lower: The lower bound to clamp to.
 * @arg upper: The upper bound to clamp to.
 *
 * @returns: The clamped value.
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
 * Clamp vector components between float values.
 *
 * @arg value: The value to clamp.
 * @arg lower: The lower bound to clamp to.
 * @arg upper: The upper bound to clamp to.
 *
 * @returns: The clamped value.
 */
inline float2 clamp(const float2 &value, const float lower, const float upper)
{
    return clamp(value, float2(lower), float2(upper));
}


/**
 * Clamp vector components between float values.
 *
 * @arg value: The value to clamp.
 * @arg lower: The lower bound to clamp to.
 * @arg upper: The upper bound to clamp to.
 *
 * @returns: The clamped value.
 */
inline float3 clamp(const float3 &value, const float lower, const float upper)
{
    return clamp(value, float3(lower), float3(upper));
}


/**
 * Clamp vector components between float values.
 *
 * @arg value: The value to clamp.
 * @arg lower: The lower bound to clamp to.
 * @arg upper: The upper bound to clamp to.
 *
 * @returns: The clamped value.
 */
inline float4 clamp(const float4 &value, const float lower, const float upper)
{
    return clamp(value, float4(lower), float4(upper));
}


/**
 * Custom round for int3 because the builtin round fails to compile for
 * int3.
 *
 * @arg value: The value to round.
 *
 * @returns: The rounded value.
 */
inline int3 round_(const float3 value)
{
    return int3(round(value.x), round(value.y), round(value.z));
}


/**
 * Sum the components of a vector.
 *
 * @arg vector: The vector to sum the components of.
 *
 * @returns: The sum of the components.
 */
inline float sumComponent(const float2 &vector)
{
    return vector.x + vector.y;
}


/**
 * Sum the components of a vector.
 *
 * @arg vector: The vector to sum the components of.
 *
 * @returns: The sum of the components.
 */
inline float sumComponent(const float3 &vector)
{
    return vector.x + vector.y + vector.z;
}


/**
 * Sum the components of a vector.
 *
 * @arg vector: The vector to sum the components of.
 *
 * @returns: The sum of the components.
 */
inline float sumComponent(const float4 &vector)
{
    return vector.x + vector.y + vector.z + vector.w;
}


/**
 * The maximum component of a vector.
 *
 * @arg vector: The vector.
 *
 * @returns: The maximum component of the vector.
 */
inline float maxComponent(const float2 &vector)
{
    return max(vector.x, vector.y);
}


/**
 * The maximum component of a vector.
 *
 * @arg vector: The vector.
 *
 * @returns: The maximum component of the vector.
 */
inline float maxComponent(const float3 &vector)
{
    return max(vector.x, max(vector.y, vector.z));
}


/**
 * The maximum component of a vector.
 *
 * @arg vector: The vector.
 *
 * @returns: The maximum component of the vector.
 */
inline float maxComponent(const float4 &vector)
{
    return max(vector.x, max(vector.y, max(vector.z, vector.w)));
}


/**
 * The minimum component of a vector.
 *
 * @arg vector: The vector.
 *
 * @returns: The minimum component of the vector.
 */
inline float minComponent(const float2 &vector)
{
    return min(vector.x, vector.y);
}


/**
 * The minimum component of a vector.
 *
 * @arg vector: The vector.
 *
 * @returns: The minimum component of the vector.
 */
inline float minComponent(const float3 &vector)
{
    return min(vector.x, min(vector.y, vector.z));
}


/**
 * The minimum component of a vector.
 *
 * @arg vector: The vector.
 *
 * @returns: The minimum component of the vector.
 */
inline float minComponent(const float4 &vector)
{
    return min(vector.x, min(vector.y, min(vector.z, vector.w)));
}


/**
 * The positive part of the vector. Ie. any negative values will be 0.
 *
 * @arg vector: The vector.
 *
 * @returns: The positive part of the vector.
 */
inline float4 positivePart(const float4 &vector)
{
    return max(vector, float4(0));
}


/**
 * The positive part of the vector. Ie. any negative values will be 0.
 *
 * @arg vector: The vector.
 *
 * @returns: The positive part of the vector.
 */
inline float3 positivePart(const float3 &vector)
{
    return max(vector, float3(0));
}


/**
 * The positive part of the vector. Ie. any negative values will be 0.
 *
 * @arg vector: The vector.
 *
 * @returns: The positive part of the vector.
 */
inline float2 positivePart(const float2 &vector)
{
    return max(vector, float2(0));
}


/**
 * The positive part of the vector. Ie. any negative values will be 0.
 *
 * @arg vector: The vector.
 *
 * @returns: The positive part of the vector.
 */
inline float positivePart(const float value)
{
    return max(value, 0.0f);
}


/**
 * The negative part of the vector. Ie. any positive values will be 0,
 * and the negative values will be positive.
 *
 * @arg vector: The vector.
 *
 * @returns: The negative part of the vector.
 */
inline float3 negativePart(const float3 &vector)
{
    return -min(vector, float3(0));
}


/**
 * The negative part of the vector. Ie. any positive values will be 0,
 * and the negative values will be positive.
 *
 * @arg vector: The vector.
 *
 * @returns: The negative part of the vector.
 */
inline float2 negativePart(const float2 &vector)
{
    return -min(vector, float2(0));
}


/**
 * The negative part of the vector. Ie. any positive values will be 0,
 * and the negative values will be positive.
 *
 * @arg vector: The vector.
 *
 * @returns: The negative part of the vector.
 */
inline float negativePart(const float value)
{
    return -min(value, 0.0f);
}


/**
 * Dot product of a vector with itself.
 *
 * @args vector: The vector to take the dot product of.
 *
 * @returns: The dot product.
 */
inline float dot2(const float2 &vector)
{
    return dot(vector, vector);
}


/**
 * Dot product of a vector with itself.
 *
 * @args vector: The vector to take the dot product of.
 *
 * @returns: The dot product.
 */
inline float dot2(const float3 &vector)
{
    return dot(vector, vector);
}


/**
 * Dot product of a vector with itself.
 *
 * @args vector: The vector to take the dot product of.
 *
 * @returns: The dot product.
 */
inline float dot2(const float4 &vector)
{
    return dot(vector, vector);
}


/**
 * Get a rotation matrix from radian angle values.
 *
 * @arg angles: The rotation angles in radians.
 * @arg out: The location to store the rotation matrix.
 */
inline void rotationMatrix(const float3 &angles, float3x3 &out)
{
    const float3 cosAngles = cos(angles);
    const float3 sinAngles = sin(angles);

    // Why tf can I not init a float3x3 normally??
    out[0][0] = cosAngles.y * cosAngles.z;
    out[0][1] = sinAngles.x * sinAngles.y * cosAngles.z - cosAngles.x * sinAngles.z;
    out[0][2] = cosAngles.x * sinAngles.y * cosAngles.z + sinAngles.x * sinAngles.z;
    out[1][0] = cosAngles.y * sinAngles.z;
    out[1][1] = sinAngles.x * sinAngles.y * sinAngles.z + cosAngles.x * cosAngles.z;
    out[1][2] = cosAngles.x * sinAngles.y * sinAngles.z - sinAngles.x * cosAngles.z;
    out[2][0] = -sinAngles.y;
    out[2][1] = sinAngles.x * cosAngles.y;
    out[2][2] = cosAngles.x * cosAngles.y;
}


/**
 * Get a rotation matrix from an axis and an angle about that axis.
 *
 * @arg angles: The rotation angles in radians.
 * @arg out: The location to store the rotation matrix.
 */
inline void axisAngleRotationMatrix(const float3 &axis, const float angle, float3x3 &out)
{
    const float cosAngle = cos(angle);
    const float oneMinusCosAngle = 1.0f - cosAngle;
    const float sinAngle = sin(angle);

    const float3 axisSquared = axis * axis;

    const float axisXY = axis.x * axis.y * oneMinusCosAngle;
    const float axisXZ = axis.x * axis.z * oneMinusCosAngle;
    const float axisYZ = axis.y * axis.z * oneMinusCosAngle;

    const float3 axisSinAngle = axis * sinAngle;

    out[0][0] = cosAngle + axisSquared.x * oneMinusCosAngle;
    out[0][1] = axisXY - axisSinAngle.z;
    out[0][2] = axisXZ + axisSinAngle.y;
    out[1][0] = axisXY + axisSinAngle.z;
    out[1][1] = cosAngle + axisSquared.y * oneMinusCosAngle;
    out[1][2] = axisYZ - axisSinAngle.x;
    out[2][0] = axisXZ - axisSinAngle.y;
    out[2][1] = axisYZ + axisSinAngle.x;
    out[2][2] = cosAngle + axisSquared.z * oneMinusCosAngle;
}


/**
 * Multiply a 4d vector by a 4x4 matrix.
 *
 * @arg m: The matrix that will transform the vector.
 * @arg v: The vector to transform.
 * @arg out: The location to store the resulting vector.
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
 * @arg m: The matrix that will transform the vector.
 * @arg v: The vector to transform.
 * @arg out: The location to store the resulting vector.
 */
inline void matmul(const float3x3 &m, const float3 &v, float3 &out)
{
    out = float3(
        m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
        m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
        m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
    );
}


/**
 * Multiply a 4d vector by a 4x4 matrix.
 *
 * @arg m: The matrix that will transform the vector.
 * @arg v: The vector to transform.
 * @arg out: The location to store the resulting vector.
 */
inline float4 matmul(const float4x4 &m, const float4 &v)
{
    float4 out;
    for (int i=0; i < 4; i++)
    {
        out[i] = 0;

        for (int j=0; j < 4; j++)
        {
            out[i] += m[i][j] * v[j];
        }
    }

    return out;
}


/**
 * Multiply a 3d vector by a 3x3 matrix.
 *
 * @arg m: The matrix that will transform the vector.
 * @arg v: The vector to transform.
 * @arg out: The location to store the resulting vector.
 */
inline float3 matmul(const float3x3 &m, const float3 &v)
{
    return float3(
        m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z,
        m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z,
        m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z
    );
}


/**
 * Offset a point in a direction.
 *
 * @arg point: The point to offset.
 * @arg direction: The direction to offset in.
 * @arg offset: The distance to offset the point.
 *
 * @returns: The offset point.
 */
inline float3 offsetPoint(
        const float3 &point,
        const float3 &direction,
        const float offset)
{
    return offset * direction + point;
}


/**
 * The distance from a point to the y-axis.
 *
 * @arg position: The position to get the distance from.
 *
 * @returns: The distance to the y-axis.
 */
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
    return sqrt(min(dot2(vector0), dot2(vector1)));
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
    return sqrt(min(dot2(vector0), dot2(vector1)));
}


/**
 * Convert a cartesion vector to cylindrical, without worrying about
 * the angle.
 *
 * @returns: Cylindrical coordinates without angle, (r, h)
 */
inline float2 cartesianToCylindrical(const float3 &coordinates)
{
    return float2(distanceToYAxis(coordinates), coordinates.y);
}


/**
 * Convert a spherical unit vector (unit radius) to cartesion.
 *
 * @arg angles: The spherical angles in radians.
 *
 * @returns: The equivalent cartesion vector.
 */
inline float3 sphericalUnitVectorToCartesion(const float2 &angles)
{
    const float sinPhi = sin(angles.y);
    return float3(
        cos(angles.x) * sinPhi,
        cos(angles.y),
        sin(angles.x) * sinPhi
    );
}


/**
 * Get the equivalent theta and phi values that lie between [0, 2 * PI),
 * and [0, PI) respectively.
 *
 * @arg angles: The spherical angles in radians.
 *
 * @returns: The equivalent theta and phi.
 */
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


inline float3 anglesBetweenVectors(const float3 &vector0, const float3 &vector1)
{
    return float3(
        acos(dot(float2(vector0.y, vector0.z), float2(vector1.y, vector1.z))),
        acos(dot(float2(vector0.x, vector0.z), float2(vector1.x, vector1.z))),
        acos(dot(float2(vector0.x, vector0.y), float2(vector1.x, vector1.y)))
    );
}


/**
 * Get the angle and axis to use to rotate a vector onto another.
 *
 * @arg axis: The rotation angles in radians.
 * @arg out: The location to store the axis.
 *
 * @returns: The angle.
 */
inline float getAngleAndAxisBetweenVectors(
        const float3 &vector0,
        const float3 &vector1,
        float3 &axis)
{
    axis = normalize(cross(vector0, vector1));
    return acos(dot(vector0, vector1));
}


inline float3 alignWithDirection(
        const float3 &unalignedAxis,
        const float3 &alignDirection,
        const float3 &vectorToAlign)
{
    float3 rotationAxis;
    const float angle = getAngleAndAxisBetweenVectors(
        unalignedAxis,
        alignDirection,
        rotationAxis
    );

    if (angle == 0.0f)
    {
        return unalignedAxis;
    }

    float3x3 rotationMatrix;
    axisAngleRotationMatrix(rotationAxis, angle, rotationMatrix);

    return matmul(rotationMatrix, vectorToAlign);
}


/**
 * Convert a cartesion unit vector to spherical.
 *
 * @arg angles: The cartesion unit vector.
 * @arg thetaOffset: Offset the theta angle by this amount.
 *
 * @returns: The spherical angles in radians.
 */
inline float2 cartesionUnitVectorToSpherical(
        const float3 &rayDirection,
        const float thetaOffset)
{
    return normalizeAngles(float2(
        atan2(rayDirection.z, rayDirection.x) + thetaOffset,
        acos(rayDirection.y)
    ));
}


/**
 * Convert a cartesion unit vector to spherical.
 *
 * @arg angles: The cartesion unit vector.
 *
 * @returns: The spherical angles in radians.
 */
inline float2 cartesionUnitVectorToSpherical(const float3 &rayDirection)
{
    return normalizeAngles(float2(
        atan2(rayDirection.z, rayDirection.x),
        acos(rayDirection.y)
    ));
}


/**
 * Spherical unit vector dot product.
 *
 * @arg vector0: The first unit vector.
 * @arg vector1: The second unit vector.
 *
 * @returns: The spherical dot product.
 */
inline float sphericalUnitDot(const float2 &vector0, const float2 &vector1)
{
    return (
        cos(vector0.x) * cos(vector1.x)
        + cos(vector0.y - vector1.y) * sin(vector0.x) * sin(vector1.x)
    );
}


/**
 * Convert the uv position in a latlong image to angles.
 *
 * @arg uvPosition: The UV position.
 *
 * @returns: The equivalent angles in radians.
 */
inline float2 uvPositionToAngles(const float2 &uvPosition)
{
    return float2(
        (uvPosition.x + 1.0f) * PI,
        (1.0f - uvPosition.y) * PI / 2.0f
    );
}


/**
 * Convert location of a pixel in an image into UV.
 *
 * @arg pixelLocation: The x, and y positions of the pixel.
 * @arg format: The image width, and height.
 *
 * @returns: The UV position.
 */
inline float2 pixelsToUV(const float2 &pixelLocation, const float2 &format)
{
    return float2(
        2.0f * pixelLocation.x / format.x - 1.0f,
        2.0f * pixelLocation.y / format.y - 1.0f
    );
}


/**
 * Blend linearly between two values.
 *
 * @arg value0: The first value.
 * @arg value1: The second value.
 * @arg weight: The blend weight, 1 will return value0, and 0 will
 *     return value1.
 *
 * @returns: The blended value.
 */
inline float blend(const float value0, const float value1, const float weight)
{
    return value1 + weight * (value0 - value1);
}


/**
 * Blend linearly between two values.
 *
 * @arg value0: The first value.
 * @arg value1: The second value.
 * @arg weight: The blend weight, 1 will return value0, and 0 will
 *     return value1.
 *
 * @returns: The blended value.
 */
inline float3 blend(const float3 &value0, const float3 &value1, const float weight)
{
    return value1 + weight * (value0 - value1);
}


/**
 * Blend linearly between two values.
 *
 * @arg value0: The first value.
 * @arg value1: The second value.
 * @arg weight: The blend weight, 1 will return value0, and 0 will
 *     return value1.
 *
 * @returns: The blended value.
 */
inline float4 blend(const float4 &value0, const float4 &value1, const float weight)
{
    return value1 + weight * (value0 - value1);
}


/**
 * Blend linearly between two values.
 *
 * @arg value0: The first value.
 * @arg value1: The second value.
 * @arg weight: The blend weight, 1 will return value0, and 0 will
 *     return value1.
 *
 * @returns: The blended value.
 */
inline float mix(const float value0, const float value1, const float weight)
{
    return (1.0f - weight) * value0 + weight * value1;
}


/**
 * Blend linearly between two values.
 *
 * @arg value0: The first value.
 * @arg value1: The second value.
 * @arg weight: The blend weight, 1 will return value0, and 0 will
 *     return value1.
 *
 * @returns: The blended value.
 */
inline float3 mix(const float3 &value0, const float3 &value1, const float weight)
{
    return (1.0f - weight) * value0 + weight * value1;
}


/**
 * Blend linearly between two values.
 *
 * @arg value0: The first value.
 * @arg value1: The second value.
 * @arg weight: The blend weight, 1 will return value0, and 0 will
 *     return value1.
 *
 * @returns: The blended value.
 */
inline float4 mix(const float4 &value0, const float4 &value1, const float weight)
{
    return (1.0f - weight) * value0 + weight * value1;
}



/**
 * Get the position component of a world matrix.
 *
 * @arg worldMatrix: The world matrix.
 * @arg position: The location to store the position.
 */
inline void positionFromWorldMatrix(const float4x4 &worldMatrix, float3 &position)
{
    position = float3(
        worldMatrix[0][3],
        worldMatrix[1][3],
        worldMatrix[2][3]
    );
}


/**
 * Get the rotation component of a world matrix.
 *
 * @arg worldMatrix: The world matrix.
 * @arg rotation: The location to store the rotation.
 */
inline void rotationFromWorldMatrix(const float4x4 &worldMatrix, float3x3 &rotationMatrix)
{
    rotationMatrix[0][0] = worldMatrix[0][0];
    rotationMatrix[0][1] = worldMatrix[0][1];
    rotationMatrix[0][2] = worldMatrix[0][2];
    rotationMatrix[1][0] = worldMatrix[1][0];
    rotationMatrix[1][1] = worldMatrix[1][1];
    rotationMatrix[1][2] = worldMatrix[1][2];
    rotationMatrix[2][0] = worldMatrix[2][0];
    rotationMatrix[2][1] = worldMatrix[2][1];
    rotationMatrix[2][2] = worldMatrix[2][2];
}


/**
 *
 */
inline float adaptiveSamples(
        const float minPaths,
        const float maxPaths,
        const float3 &variance)
{
    return clamp(
        round(maxPaths * length(variance)),
        minPaths,
        maxPaths
    );
}


/**
 *
 */
inline float balanceHeuristic(const float pdf0, const float pdf1)
{
    return pdf0 / (pdf0 + pdf1);
}

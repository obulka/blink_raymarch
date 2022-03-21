//
// Matrix and vector math operations
//


inline float min(const float value0, const float value1, const float value2)
{
    return min(value0, min(value1, value2));
}


inline float max(const float value0, const float value1, const float value2)
{
    return max(value0, max(value1, value2));
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


inline float3 offsetPoint(
        const float3 &point,
        const float3 &direction,
        const float offset)
{
    return offset * direction + point;
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
float sdfLength(const float2 &vector)
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
float sdfLength(const float3 &vector)
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


/**
 * Get the value of sky the ray would hit at infinite distance
 */
float2 cartesionUnitVectorToSpherical(const float3 &rayDirection, const float thetaOffset)
{
    float rayAnglePhi = fmod(acos(rayDirection.y), PI);
    rayAnglePhi += PI * (rayAnglePhi < 0);

    float rayAngleTheta = fmod(
        atan2(rayDirection.z, rayDirection.x) + thetaOffset,
        2 * PI
    );
    rayAngleTheta += 2 * PI * (rayAngleTheta < 0);

    return float2(rayAngleTheta, rayAnglePhi);
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


inline float mix(float x, float y, float a)
{
    return x + (y - x) * a;
}


inline float2 smoothMinN(float colour0, float colour1, float blendSize, float exponent)
{
    float m = 0.5f * pow(
        positivePart(blendSize - fabs(colour0 - colour1)) / blendSize,
        exponent
    );

    float s = m * blendSize / exponent;

    return (colour0 < colour1) ? float2(colour0 - s, m) : float2(colour1 - s, m - 1.0f);
}


inline float4 blend(const float4 &colour0, const float4 &colour1, const float weight)
{
    return weight * colour0 + (1 - weight) * colour1;
}


inline float4 blend(
    const float4 &colour0,
    const float weight0,
    const float4 &colour1,
    const float weight1,
    const float4 &colour2,
    const float weight2,
    const float4 &colour3)
{
    return blend(
        colour0,
        blend(
            colour1,
            blend(
                colour2,
                colour3,
                weight2
            ),
            weight1
        ),
        weight0
    );
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


inline float3 reflectRayOffSurface(
    const float3 &incidentRayDirection,
    const float3 &surfaceNormalDirection)
{
    return (
        incidentRayDirection
        - 2 * dot(incidentRayDirection, surfaceNormalDirection) * surfaceNormalDirection
    );
}


inline void rotationMatrix(const float3 &angles, float3x3 &out)
{
    // Why tf cant you init a float3x3 normally??
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
 * Invert a 4x4 matrix
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

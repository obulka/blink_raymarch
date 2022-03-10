//
// Matrix and vector math operations
//


float3 offsetPoint(
        const float3 &point,
        const float3 &direction,
        const float offset)
{
    return offset * direction + point;
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
float saturate(float value)
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
float2 saturate(const float2 &value)
{
    return float2(saturate(value.x), saturate(value.y));
}


/**
 * Get the position component of a world matrix.
 *
 * @arg worldMatrix: The world matrix
 * @arg position: The location to store the position
 */
void positionFromWorldMatrix(const float4x4 &worldMatrix, float3 &position)
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
void matmul(const float4x4 &m, const float4 &v, float4 &out)
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
        m[1][1]  * m[2][2] * m[3][3] - 
        m[1][1]  * m[2][3] * m[3][2] - 
        m[2][1]  * m[1][2]  * m[3][3] + 
        m[2][1]  * m[1][3]  * m[3][2] +
        m[3][1] * m[1][2]  * m[2][3] - 
        m[3][1] * m[1][3]  * m[2][2];

    inv[1][0] =
        -m[1][0]  * m[2][2] * m[3][3] + 
        m[1][0]  * m[2][3] * m[3][2] + 
        m[2][0]  * m[1][2]  * m[3][3] - 
        m[2][0]  * m[1][3]  * m[3][2] - 
        m[3][0] * m[1][2]  * m[2][3] + 
        m[3][0] * m[1][3]  * m[2][2];

    inv[2][0] =
        m[1][0]  * m[2][1] * m[3][3] - 
        m[1][0]  * m[2][3] * m[3][1] - 
        m[2][0]  * m[1][1] * m[3][3] + 
        m[2][0]  * m[1][3] * m[3][1] + 
        m[3][0] * m[1][1] * m[2][3] - 
        m[3][0] * m[1][3] * m[2][1];

    inv[3][0] =
        -m[1][0]  * m[2][1] * m[3][2] + 
        m[1][0]  * m[2][2] * m[3][1] +
        m[2][0]  * m[1][1] * m[3][2] - 
        m[2][0]  * m[1][2] * m[3][1] - 
        m[3][0] * m[1][1] * m[2][2] + 
        m[3][0] * m[1][2] * m[2][1];

    inv[0][1] =
        -m[0][1]  * m[2][2] * m[3][3] + 
        m[0][1]  * m[2][3] * m[3][2] + 
        m[2][1]  * m[0][2] * m[3][3] - 
        m[2][1]  * m[0][3] * m[3][2] - 
        m[3][1] * m[0][2] * m[2][3] + 
        m[3][1] * m[0][3] * m[2][2];

    inv[1][1] =
        m[0][0]  * m[2][2] * m[3][3] - 
        m[0][0]  * m[2][3] * m[3][2] - 
        m[2][0]  * m[0][2] * m[3][3] + 
        m[2][0]  * m[0][3] * m[3][2] + 
        m[3][0] * m[0][2] * m[2][3] - 
        m[3][0] * m[0][3] * m[2][2];

    inv[2][1] = 
        m[0][0]  * m[2][1] * m[3][3] + 
        m[0][0]  * m[2][3] * m[3][1] + 
        m[2][0]  * m[0][1] * m[3][3] - 
        m[2][0]  * m[0][3] * m[3][1] - 
        m[3][0] * m[0][1] * m[2][3] + 
        m[3][0] * m[0][3] * m[2][1];

    inv[3][1] =
        m[0][0]  * m[2][1] * m[3][2] - 
        m[0][0]  * m[2][2] * m[3][1] - 
        m[2][0]  * m[0][1] * m[3][2] + 
        m[2][0]  * m[0][2] * m[3][1] + 
        m[3][0] * m[0][1] * m[2][2] - 
        m[3][0] * m[0][2] * m[2][1];

    inv[0][2] =
        m[0][1]  * m[1][2] * m[3][3] - 
        m[0][1]  * m[1][3] * m[3][2] - 
        m[1][1]  * m[0][2] * m[3][3] + 
        m[1][1]  * m[0][3] * m[3][2] + 
        m[3][1] * m[0][2] * m[1][3] - 
        m[3][1] * m[0][3] * m[1][2];

    inv[1][2] =
        -m[0][0]  * m[1][2] * m[3][3] + 
        m[0][0]  * m[1][3] * m[3][2] + 
        m[1][0]  * m[0][2] * m[3][3] - 
        m[1][0]  * m[0][3] * m[3][2] - 
        m[3][0] * m[0][2] * m[1][3] + 
        m[3][0] * m[0][3] * m[1][2];

    inv[2][2] =
        m[0][0]  * m[1][1] * m[3][3] - 
        m[0][0]  * m[1][3] * m[3][1] - 
        m[1][0]  * m[0][1] * m[3][3] + 
        m[1][0]  * m[0][3] * m[3][1] + 
        m[3][0] * m[0][1] * m[1][3] - 
        m[3][0] * m[0][3] * m[1][1];

    inv[3][2] =
        -m[0][0]  * m[1][1] * m[3][2] + 
        m[0][0]  * m[1][2] * m[3][1] + 
        m[1][0]  * m[0][1] * m[3][2] - 
        m[1][0]  * m[0][2] * m[3][1] - 
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

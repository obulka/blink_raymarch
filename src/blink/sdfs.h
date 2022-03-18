//
// Signed Distance Functions
//


/**
 * Compute the signed distance from a point to a sphere
 *
 * @arg position: The point to get the distance to from the object
 * @arg centre: The object centre
 * @arg radius: The radius of the sphere
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToSphere(const float3 &position, const float radius)
{
    return length(position) - radius;
}

/**
 * Compute the signed distance from a point to a rectangular prism
 *
 * @arg position: The point to get the distance to from the object
 * @arg centre: The object centre
 * @arg size: The length, width, and depth of the prism
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToRectangularPrism(const float3 &position, const float3 &size)
{
    const float3 boundingSphereVector = fabs(position) - size;
    float ud = length(max(boundingSphereVector, float3(0, 0, 0)));
    float n = max(
        max(min(boundingSphereVector.x, 0.0f), min(boundingSphereVector.y, 0.0f)),
        min(boundingSphereVector.z, 0.0f)
    );

    return ud + n;
}

/**
 * Compute the signed distance from a point to a torus
 *
 * @arg position: The point to get the distance to from the object
 * @arg centre: The object centre
 * @arg radii: The inner and outer radii of the torus
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToTorus(const float3 &position, const float2 &radii)
{
    float2 positionXZ = float2(position.x, position.z);
    float2 inner = float2(
        length(positionXZ) - radii.y,
        position.y
    );

    return length(inner) - radii.x;
}

/**
 * Compute the signed distance from a point to a triangular prism
 *
 * @arg position: The point to get the distance to from the object
 * @arg centre: The object centre
 * @arg size: The height and depth of the prism
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToTriangularPrism(const float3 &position, const float2 &size)
{
    const float3 centreVector = fabs(position);

    return max(
        centreVector.z - size.y,
        max(
            centreVector.x * 0.866025f + position.y * 0.5f,
            -position.y
        ) - size.x * 0.5f
    );
}

/**
 * Compute the signed distance from a point to a cylinder
 *
 * @arg position: The point to get the distance to from the object
 * @arg centre: The object centre
 * @arg size: The height and radius of the cylinder
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToCylinder(const float3 &position, const float2 &size)
{
    const float3 centreVector = fabs(position);

    float2 distanceToXZ = float2(centreVector.x, centreVector.z);

    float2 d = fabs(float2(length(distanceToXZ), centreVector.y)) - size;

    return length(max(d, float2(0, 0))) + min(max(d.x, d.y), 0.0f);
}


/**
 * Compute the signed distance from a point to a mandelbulb
 *
 * @arg position: The point to get the distance to from the object
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToMandelbulb(const float3 &position, const float power)
{
    float3 currentPosition = position;
    float dradius = 1;
    float radius = 0;

    for (int i = 0; i < 13; i++) {
        radius = length(currentPosition);

        if (radius > 2) {
            break;
        }

        float theta = acos(currentPosition.z / radius);
        float phi = atan2(currentPosition.y, currentPosition.x);
        dradius = pow(radius, power - 1.0f) * power * dradius + 1.0f;

        theta = theta * power;
        phi = phi * power;

        currentPosition = pow(radius, power) * float3(
            sin(theta) * cos(phi),
            sin(phi) * sin(theta),
            cos(theta)
        );
        currentPosition += position;
    }

    return 0.5f * log(radius) * radius / dradius;
}


float distanceToObject(const float3 &position, const int shapeType, const float3 &scale)
{
    if (shapeType == 0)
    {
        return distanceToSphere(position, scale.x);
    }
    if (shapeType == 1)
    {
        return distanceToRectangularPrism(position, scale);
    }

    float2 size = float2(scale.x, scale.y);
    if (shapeType == 2)
    {
        return distanceToCylinder(position, size);
    }
    if (shapeType == 3)
    {
        return distanceToTriangularPrism(position, size);
    }
    if (shapeType == 4)
    {
        return distanceToTorus(position, size);
    }
    return distanceToMandelbulb(position, size.x);
}

//
// Signed Distance Functions
//


/**
 * Compute the signed distance from a point to a sphere
 *
 * @arg position: The point to get the distance to from the object
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
 * @arg radii: The inner and outer radii of the torus
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToTorus(const float3 &position, const float2 &radii)
{
    return length(
        float2(
            length(float2(position.x, position.z)) - radii.y,
            position.y
        )
    ) - radii.x;
}

/**
 * Compute the signed distance from a point to a triangular prism
 *
 * @arg position: The point to get the distance to from the object
 * @arg size: The height and depth of the prism
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToTriangularPrism(const float3 &position, const float2 &size)
{
    const float3 absPosition = fabs(position);

    return max(
        absPosition.z - size.y,
        max(
            absPosition.x * 0.866025f + position.y * 0.5f,
            -position.y
        ) - size.x * 0.5f
    );
}

/**
 * Compute the signed distance from a point to a cylinder
 *
 * @arg position: The point to get the distance to from the object
 * @arg size: The height and radius of the cylinder
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToCylinder(const float3 &position, const float2 &size)
{
    const float3 absPosition = fabs(position);

    float2 absPositionXZ = float2(absPosition.x, absPosition.z);

    float2 d = fabs(
        float2(
            length(float2(absPosition.x, absPosition.z)),
            absPosition.y
        )
    ) - size;

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


/**
 * Compute the signed distance from a point to a rectangular prism
 *
 * @arg position: The point to get the distance to from the object
 * @arg dims: The length, width, depth, and thickness of the prism
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToRectangularPrismFrame(const float3 &position, const float4 &dims)
{
    float3 external = fabs(position) - float3(dims.x, dims.y, dims.z);
    float3 internal = fabs(external + dims.w) - dims.w;

    return min(
        min(
            length(max(float3(external.x, internal.y, internal.z), float3(0)))
                + min(max(external.x, max(internal.y, internal.z)), 0.0f),
            length(max(float3(internal.x, external.y, internal.z), float3(0)))
                + min(max(internal.x, max(external.y, internal.z)), 0.0f)
        ),
        length(max(float3(internal.x, internal.y, external.z), float3(0)))
            + min(max(internal.x, max(internal.y, external.z)), 0.0f)
    );
}


float distanceToObject(const float3 &position, const int shapeType, const float4 &dimensions)
{
    float dim1 = dimensions.x;
    if (shapeType == 0)
    {
        return distanceToSphere(position, dim1);
    }

    float3 dim3 = float3(dimensions.x, dimensions.y, dimensions.z);
    if (shapeType == 1)
    {
        return distanceToRectangularPrism(position, dim3);
    }

    float2 dim2 = float2(dimensions.x, dimensions.y);
    if (shapeType == 2)
    {
        return distanceToCylinder(position, dim2);
    }
    if (shapeType == 3)
    {
        return distanceToTriangularPrism(position, dim2);
    }
    if (shapeType == 4)
    {
        return distanceToTorus(position, dim2);
    }
    if (shapeType == 5)
    {
        return distanceToMandelbulb(position, dim1);
    }
    if (shapeType == 6)
    {
        return distanceToRectangularPrismFrame(position, dimensions);
    }
    /*
    if (shapeType == 5)
    {
        return distanceToMandelbulb(position, dim1);
    }
    if (shapeType == 5)
    {
        return distanceToMandelbulb(position, dim1);
    }
    if (shapeType == 5)
    {
        return distanceToMandelbulb(position, dim1);
    }
    if (shapeType == 5)
    {
        return distanceToMandelbulb(position, dim1);
    }
    */

    return 0;
}

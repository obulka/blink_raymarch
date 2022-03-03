//
// Signed Distance Functions
//


/**
 * Compute the signed distance from a point to a sphere
 *
 * @arg distanceTo: The point to get the distance to from the object
 * @arg centre: The object centre
 * @arg radius: The radius of the sphere
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToSphere(float3 distanceTo, float3 centre, float radius) {
    return length(centre - distanceTo) - radius;
}

/**
 * Compute the signed distance from a point to a rectangular prism
 *
 * @arg distanceTo: The point to get the distance to from the object
 * @arg centre: The object centre
 * @arg size: The length, width, and depth of the prism
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToRectangularPrism(float3 distanceTo, float3 centre, float3 size) {
    float3 boundingSphereVector = fabs(distanceTo - centre) - size;
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
 * @arg distanceTo: The point to get the distance to from the object
 * @arg centre: The object centre
 * @arg radii: The inner and outer radius of the torus
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToTorus(float3 distanceTo, float3 centre, float2 radii)
{
    float3 centreVector = distanceTo - centre;
    float2 centreXZ = float2(centreVector.x, centreVector.z);
    float2 inner = float2(
        length(centreXZ) - radii.y,
        distanceTo.y - centre.y
    );

    return length(inner) - radii.x;
}

/**
 * Compute the signed distance from a point to a triangular prism
 *
 * @arg distanceTo: The point to get the distance to from the object
 * @arg centre: The object centre
 * @arg size: The height and depth of the prism
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToTriangularPrism(float3 distanceTo, float3 centre, float2 size) {
    float3 centreVector = fabs(distanceTo - centre);

    return max(
        centreVector.z - size.y,
        max(
            centreVector.x * 0.866025f + distanceTo.y * 0.5f,
            -distanceTo.y
        ) - size.x * 0.5f
    );
}

/**
 * Compute the signed distance from a point to a cylinder
 *
 * @arg distanceTo: The point to get the distance to from the object
 * @arg centre: The object centre
 * @arg size: The height and radius of the cylinder
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToCylinder(float3 distanceTo, float3 centre, float2 size) {
    float3 centreVector = fabs(distanceTo - centre);

    float2 distanceToXZ = float2(centreVector.x, centreVector.z);

    float2 d = fabs(float2(length(distanceToXZ), centreVector.y)) - size;

    return length(max(d, float2(0, 0))) + min(max(d.x, d.y), 0.0f);
}

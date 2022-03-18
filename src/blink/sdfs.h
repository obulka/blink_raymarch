//
// Signed Distance Functions
//


/**
 * Compute the signed distance from a point to a sphere
 *
 * @arg position: The point to get the distance to, from the object
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
 * @arg position: The point to get the distance to, from the object
 * @arg width: The width (x) of the prism
 * @arg height: The height (y) of the prism
 * @arg depth: The depth (z) of the prism
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToRectangularPrism(
        const float3 &position,
        const float width,
        const float height,
        const float depth)
{
    const float3 boundingSphereVector = fabs(position) - float3(width, height, depth);
    float ud = length(max(boundingSphereVector, float3(0, 0, 0)));
    float n = max(
        max(min(width, 0.0f), min(height, 0.0f)),
        min(depth, 0.0f)
    );

    return ud + n;
}


/**
 * Compute the signed distance from a point to a triangular prism
 *
 * @arg position: The point to get the distance to, from the object
 * @arg triangleEdgeLength: The triangular edge length (xy-plane)
 * @arg prismDepth: The depth (z-axis) of the prism
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToTriangularPrism(
        const float3 &position,
        const float triangleEdgeLength,
        const float prismDepth)
{
    const float3 absPosition = fabs(position);

    return max(
        absPosition.z - prismDepth,
        max(
            absPosition.x * 0.866025f + position.y * 0.5f,
            -position.y
        ) - triangleEdgeLength * 0.5f
    );
}


/**
 * Compute the signed distance from a point to a cylinder
 *
 * @arg position: The point to get the distance to, from the object
 * @arg radius: The radius (xz-plane) of the cylinder
 * @arg height: The height (y-axis) of the cylinder
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToCylinder(
        const float3 &position,
        const float radius,
        const float height)
{
    const float3 absPosition = fabs(position);

    float2 d = fabs(
        float2(
            length(float2(absPosition.x, absPosition.z)),
            absPosition.y
        )
    ) - float2(radius, height);

    return length(max(d, float2(0, 0))) + min(max(radius, height), 0.0f);
}


/**
 * Compute the signed distance from a point to a mandelbulb
 *
 * @arg position: The point to get the distance to, from the object
 * @arg power: One greater than the axes of symmetry in the xy-plane
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
 * Compute the signed distance from a point to the frame of a
 * rectangular prism
 *
 * @arg position: The point to get the distance to, from the object
 * @arg width:  The width (x) of the frame
 * @arg height:  The height (y) of the frame
 * @arg depth:  The depth (z) of the frame
 * @arg thickness:  The thickness of the frame
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToRectangularPrismFrame(
        const float3 &position,
        const float width,
        const float height,
        const float depth,
        const float thickness)
{
    float3 external = fabs(position) - float3(width, height, depth);
    float3 internal = fabs(external + thickness) - thickness;

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


/**
 * Compute the signed distance from a point to a torus
 *
 * @arg position: The point to get the distance to, from the object
 * @arg ringRadius: The radius (xy-plane) of the ring of the torus
 * @arg tubeRadius: The radius of the tube of the torus
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToTorus(
        const float3 &position,
        const float ringRadius,
        const float tubeRadius)
{
    return length(
        float2(
            length(float2(position.x, position.y)) - ringRadius,
            position.z
        )
    ) - tubeRadius;
}


/**
 * Compute the signed distance from a point to a capped torus
 *
 * @arg position: The point to get the distance to, from the object
 * @arg ringRadius: The radius (xy-plane) of the ring of the torus
 * @arg tubeRadius: The radius of the tube of the torus
 * @arg capAngle: The angle (xy-plane, symmetric about y-axis) to cap
 *     at, in the range (0-PI)
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToCappedTorus(
        const float3 &position,
        const float ringRadius,
        const float tubeRadius,
        const float capAngle)
{
    float2 capDirection = float2(sin(capAngle), cos(capAngle));
    float3 absXPosition = float3(fabs(position.x), position.y, position.z);
    float2 posXY = float2(absXPosition.x, absXPosition.y);

    float capFactor;
    if (capDirection.y * absXPosition.x > capDirection.x * absXPosition.y)
    {
        // project position on xy-plane onto the direction we are capping at
        capFactor = dot(posXY, float2(capDirection.x, capDirection.y));
    }
    else
    {
        // distance to z-axis from position
        capFactor = length(posXY);
    }
    return sqrt(
        dot(absXPosition, absXPosition)
        + ringRadius * ringRadius
        - 2.0f * ringRadius * capFactor
    ) - tubeRadius;
}


/**
 * Compute the signed distance from a point to a capped torus
 *
 * @arg position: The point to get the distance to, from the object
 * @arg dims: The radius of the 'tube' of the torus, the radius of
 *     the 'ring', and the angle to cap at in range (0-PI)
 *
 * @returns: The minimum distance from the point to the shape
float distanceToLink(const float3 &position, const float3 &dims)
{
    float2 cap = float2(sin(dims.z), cos(dims.z));
    float3 pos = float3(fabs(position.x), position.y, position.z);
    float2 posXY = float2(pos.x, pos.y);

    float k;
    if (cap.y * pos.x > cap.x * pos.y)
    {
        k = dot(posXY, float2(cap.x, cap.y));
    }
    else
    {
        k = length(posXY);
    }
    return sqrt(dot(pos, pos) + dims.x * dims.x - 2.0f * dims.x * k) - dims.y;
}
 */



/**
 * Compute the signed distance from a point to a capped torus
 *
 * @arg position: The point to get the distance to, from the object
 * @arg shapeType: The selection of shape to get the distance to, options:
 *     0: sphere
 * 
 * @arg dimensions: The radius of the 'tube' of the torus, the radius of
 *     the 'ring', and the angle to cap at in range (0-PI)
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToObject(const float3 &position, const int shapeType, const float4 &dimensions)
{
    if (shapeType == 0)
    {
        return distanceToSphere(position, dimensions.x);
    }
    if (shapeType == 1)
    {
        return distanceToRectangularPrism(
            position,
            dimensions.x,
            dimensions.y,
            dimensions.z
        );
    }
    if (shapeType == 2)
    {
        return distanceToCylinder(position, dimensions.x, dimensions.y);
    }
    if (shapeType == 3)
    {
        return distanceToTriangularPrism(position, dimensions.x, dimensions.y);
    }
    if (shapeType == 4)
    {
        return distanceToTorus(position, dimensions.x, dimensions.y);
    }
    if (shapeType == 5)
    {
        return distanceToMandelbulb(position, dimensions.x);
    }
    if (shapeType == 6)
    {
        return distanceToRectangularPrismFrame(
            position,
            dimensions.x,
            dimensions.y,
            dimensions.z,
            dimensions.w
        );
    }
    if (shapeType == 7)
    {
        return distanceToCappedTorus(
            position,
            dimensions.x,
            dimensions.y,
            dimensions.z
        );
    }
    /*
    if (shapeType == 8)
    {
        return distanceToLink(
            position,
            dimensions.x,
            dimensions.y,
            dimensions.z
        );
    }
    if (shapeType == 5)
    {
        return distanceToMandelbulb(position, dimX);
    }
    if (shapeType == 5)
    {
        return distanceToMandelbulb(position, dimX);
    }
    */

    return 0;
}

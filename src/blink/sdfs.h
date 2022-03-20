//
// Signed Distance Functions
//


float distanceToYAxis(const float3 &position)
{
    return length(float2(position.x, position.z));
}


/**
 * Compute the min distance from a point to a circle
 *
 * @arg position: The point to get the distance to, from the object
 * @arg radius: The radius (xy-plane) of the ring of the torus
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToCircle(const float2 &position, const float radius)
{
    return length(position) - radius;
}


/**
 * Compute the min distance from a point to a sphere
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
 * Compute the min distance from a point to a rectangular prism
 * Centered at the origin
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
    // Only look at positive quadrant, using symmetry
    const float3 prismToPosition = fabs(position) - float3(width, height, depth) / 2.0f;
    // Clamp the components that are inside the prism to the surface
    // before getting the distance
    return length(positivePart(prismToPosition)) - negativePart(maxComponent(prismToPosition));
}


/**
 * Compute the min distance from a point to a triangular prism
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
 * Compute the min distance from a point to a cylinder
 * Symmetric about the xz-plane
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
    // Cylindrical coordinates (r, h), ignoring the angle due to symmetry
    float2 cylindricalPosition = float2(distanceToYAxis(position), fabs(position.y));
    float2 cylinderToPosition = cylindricalPosition - float2(radius, height / 2);

    return length(positivePart(cylinderToPosition));
}


/**
 * Compute the min distance from a point to a mandelbulb
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
 * Compute the min distance from a point to the frame of a
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
    float3 prismToPosition = fabs(position) - float3(width, height, depth);
    float3 innerReflected = fabs(prismToPosition + thickness) - thickness;

    return min(
        min(
            length(positivePart(float3(prismToPosition.x, innerReflected.y, innerReflected.z)))
                - negativePart(max(prismToPosition.x, max(innerReflected.y, innerReflected.z))),
            length(positivePart(float3(innerReflected.x, prismToPosition.y, innerReflected.z)))
                - negativePart(max(innerReflected.x, max(prismToPosition.y, innerReflected.z)))
        ),
        length(positivePart(float3(innerReflected.x, innerReflected.y, prismToPosition.z)))
            - negativePart(max(innerReflected.x, max(innerReflected.y, prismToPosition.z)))
    );
}


/**
 * Compute the min distance from a point to a torus
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
    float distanceToRingInXYPlane = distanceToCircle(
        float2(position.x, position.y),
        ringRadius
    );
    return distanceToCircle(
        float2(distanceToRingInXYPlane, position.z),
        tubeRadius
    );
}


/**
 * Compute the min distance from a point to a capped torus
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
 * Compute the min distance from a point to a chain link
 *
 * @arg position: The point to get the distance to, from the object
 * @arg width: The width (x-axis) of the link,
 *     measured from origin to center of the 'tube'
 * @arg height: The height (y-axis) of the link
 *     measured from origin to the base of the cap (ie. 0 is a torus)
 * @arg tubeRadius: The radius of the tube that makes the link
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToLink(
        const float3 &position,
        const float width,
        const float height,
        const float tubeRadius)
{
    float heightDifference = fabs(position.y) - height;

    float distanceInXYPlane = distanceToCircle(
        float2(position.x, positivePart(heightDifference)),
        width
    );
    return distanceToCircle(
        float2(distanceInXYPlane, position.z),
        tubeRadius
    );
}


/**
 * Compute the min distance from a point to an infinite cylinder
 * (y-axis aligned)
 *
 * @arg position: The point to get the distance to, from the object
 * @arg radius: The radius (xz-plane) of the cylinder
 *
 * @returns: The minimum distance from the point to the shape
 */
float distanceToInfiniteCylinder(const float3 &position, const float radius)
{
    return distanceToCircle(float2(position.x, position.z), radius);
}


/**
 * Compute the min distance from a point to a cone
 * (y-axis aligned) The tip of the cone is at the origin, and it opens
 * up the y-axis
 *
 * @arg position: The point to get the distance to, from the object
 * @arg angle: The angle between the tip and base of the cone [0-PI/2)
 *     measured between the y-axis and wall of the cone
 * @arg height: The height of the cone, cannot be 0
 * 
 * @returns: The minimum distance from the point to the shape
 */
float distanceToCone(const float3 &position, const float angle, const float height)
{
    // Cylindrical coordinates (r, h), ignoring the angle due to symmetry
    float2 cylindricalPosition = cartesianToCylindrical(position);

    // The cylindrical coordinates of the edge of the cone base
    float2 cylindricalBound = float2(fabs(height * tan(angle)), height);

    float2 coneEdgeToPosition = (
        cylindricalPosition - cylindricalBound * saturate(
            dot(cylindricalPosition, cylindricalBound)
            / dot(cylindricalBound, cylindricalBound)
        )
    );
    float2 coneRadialToPosition = cylindricalPosition - cylindricalBound * float2(
        saturate(cylindricalPosition.x / cylindricalBound.x),
        1
    );

    float heightSign = sign(height);
    float inside = sign(max(
        heightSign * (cylindricalPosition.x * height - cylindricalPosition.y * cylindricalBound.x),
        heightSign * (cylindricalPosition.y - height)
    ));
    return inside * sqrt(min(
        dot(coneEdgeToPosition, coneEdgeToPosition),
        dot(coneRadialToPosition, coneRadialToPosition)
    ));
}


/**
 * Compute the min distance from a point to an infinite cone
 * (y-axis aligned) The tip of the cone is at the origin, and it opens
 * up the y-axis
 *
 * @arg position: The point to get the distance to, from the object
 * @arg angle: The angle between the tip and base of the cone [0-PI/2)
 *     measured between the y-axis and wall of the cone
 * 
 * @returns: The minimum distance from the point to the shape
 */
float distanceToInfiniteCone(const float3 &position, const float angle)
{
    // The normalized cylindrical coordinates of the edge of the cone base
    float2 cylindricalBound = float2(sin(angle), cos(angle));

    // Cylindrical coordinates (r, h), ignoring the angle due to symmetry
    float2 cylindricalPosition = cartesianToCylindrical(position);

    float inside = sign(
        cylindricalPosition.x * cylindricalBound.y
        - cylindricalPosition.y * cylindricalBound.x
    );

    return inside * length(
        cylindricalPosition
        - cylindricalBound * positivePart(dot(cylindricalPosition, cylindricalBound))
    );
}


/**
 * Compute the min distance from a point to a plane
 *
 * @arg position: The point to get the distance to, from the object
 * @arg normal: The normal direction of the plane
 * 
 * @returns: The minimum distance from the point to the shape
 */
float distanceToPlane(const float3 &position, const float3 &normal)
{
    return dot(position, normal);
}


/**
 * Compute the min distance from a point to a capped torus
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
    if (shapeType == 8)
    {
        return distanceToLink(
            position,
            dimensions.x,
            dimensions.y,
            dimensions.z
        );
    }
    if (shapeType == 9)
    {
        return distanceToInfiniteCylinder(position, dimensions.x);
    }
    if (shapeType == 10)
    {
        return distanceToCone(position, dimensions.x, dimensions.y);
    }
    if (shapeType == 11)
    {
        return distanceToInfiniteCone(position, dimensions.x);
    }
    if (shapeType == 12)
    {
        return distanceToPlane(
            position,
            normalize(float3(dimensions.x, dimensions.y, dimensions.z))
        );
    }



    /*
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

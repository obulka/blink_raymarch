// Copyright 2022 by Owen Bulka.
// All rights reserved.
// This file is released under the "MIT License Agreement".
// Please see the LICENSE.md file that should have been included as part
// of this package.

//
// Signed Distance Functions
//
// Many of the below sdfs are based on the work of Inigo Quilez
// https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
//


/**
 * Compute the min distance from a point to a point.
 *
 * @arg position: The point to get the distance to, from the object.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToPoint(const float3 &position)
{
    return length(position);
}


/**
 * Compute the min distance from a point to a circle.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg radius: The radius (xy-plane) of the ring of the torus.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToCircle(const float2 &position, const float radius)
{
    return length(position) - radius;
}


/**
 * Compute the min distance from a point to a sphere.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg radius: The radius of the sphere.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToSphere(const float3 &position, const float radius)
{
    return length(position) - radius;
}


/**
 * Compute the inexact min distance from a point to an ellipsoid.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg xRadius: The radius along the x-axis of the ellipsoid.
 * @arg yRadius: The radius along the y-axis of the ellipsoid.
 * @arg zRadius: The radius along the z-axis of the ellipsoid.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToEllipsoid(
        const float3 &position,
        const float xRadius,
        const float yRadius,
        const float zRadius)
{
    const float3 radii = float3(xRadius, yRadius, zRadius);

    // Components of this vector that are < 1 are inside the ellipse
    // when projected onto the plane the respective axis is normal to
    const float3 scaledPosition = position / radii;

    // If this length is < 1 we are inside the ellipsoid
    const float scaledLength = length(scaledPosition);

    return scaledLength * (scaledLength - 1.0f) / length(scaledPosition / radii);
}


/**
 * Compute the min distance from a point to a cut sphere.
 * The cut surface faces up the y-axis.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg radius: The radius of the sphere.
 * @arg height: The height (y-axis) below which the sphere is culled.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToCutSphere(
        const float3 &position,
        const float radius,
        const float height)
{
    const float2 cylindricalPosition = cartesianToCylindrical(position);

    // The radius of the circle made by slicing the sphere
    const float cutRadiusSquared = radius * radius - height * height;
    const float cutRadius = sqrt(cutRadiusSquared);

    // When the height is positive, if we are outside an infinite
    // cone with its tip at the origin, opening through the edge of
    // the cut surface, then the nearest point will be on the
    // spherical surface. If the height is negative, we must be
    // below the portion of the cone that is below the y-axis, but we
    // must also be below a curved boundary separating the regions where
    // the flat and spherical surfaces are closest
    const float nearestIsSpherical = max(
        cutRadiusSquared * (radius - height + 2.0f * cylindricalPosition.y)
            - (radius + height) * cylindricalPosition.x * cylindricalPosition.x,
        cutRadius * cylindricalPosition.y - height * cylindricalPosition.x
    );

    if (nearestIsSpherical < 0.0f)
    {
        // Closest point is on the surface of the sphere
        return length(cylindricalPosition) - radius;
    }
    else if (cylindricalPosition.x < cutRadius)
    {
        // Closest point is within the cut surface
        return -height + cylindricalPosition.y;
    }
    else
    {
        // Closest point is on the edge of the cut surface
        return length(cylindricalPosition - float2(cutRadius, height));
    }
}


/**
 * Compute the min distance from a point to a hollow sphere.
 * The hollowed opening points up the y-axis.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg radius: The radius of the sphere.
 * @arg height: The height (y-axis) at which an opening is created.
 * @arg thickness: The thickness of the walls of the hollow sphere.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToHollowSphere(
        const float3 &position,
        const float radius,
        const float height,
        const float thickness)
{
    const float halfThickness = thickness / 2.0f;

    const float2 cylindricalPosition = cartesianToCylindrical(position);

    const float cutRadius = sqrt(radius * radius - height * height);

    if (height * cylindricalPosition.x < cutRadius * cylindricalPosition.y)
    {
        // Closest point is on the rim
        return length(cylindricalPosition - float2(cutRadius, height)) - halfThickness;
    }
    // Closest point is on the spherical surface
    return fabs(length(cylindricalPosition) - radius) - halfThickness;
}


/**
 * Compute the min distance from a point to a death star.
 * The hollowed opening points up the y-axis.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg solidRadius: The radius of the sphere that remains solid.
 * @arg hollowRadius: The radius of the sphere that is cut from the
 *     solid.
 * @arg hollowHeight: The height (y-axis) of the center of the sphere
 *     that is cut from the solid, above solidRadius + hollowRadius, the
 *     result will be a standard sphere of radius solidRadius.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToDeathStar(
        const float3 &position,
        const float solidRadius,
        const float hollowRadius,
        const float hollowHeight)
{
    const float2 cylindricalPosition = cartesianToCylindrical(position);

    const float solidRadiusSquared = solidRadius * solidRadius;

    const float cutHeight = (
        solidRadiusSquared
        - (hollowRadius * hollowRadius - hollowHeight * hollowHeight)
    ) / (2.0f * hollowHeight);

    const float cutRadius = sqrt(solidRadiusSquared - cutHeight * cutHeight);

    if (hollowHeight * positivePart(cutRadius - cylindricalPosition.x)
            < cylindricalPosition.y * cutRadius - cylindricalPosition.x * cutHeight)
    {
        // Closest point is on the rim
        return length(cylindricalPosition - float2(cutRadius, cutHeight));
    }
    return max(
        // Closest point to the solid sphere
        length(cylindricalPosition) - solidRadius,
        // Closest point to the hollowed portion
        hollowRadius - length(cylindricalPosition - float2(0, hollowHeight))
    );
}


/**
 * Compute the min distance from a point to a solid angle.
 * The conical shape has its tip at the origin and opens up the y-axis.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg radius: The radius of the sphere to cut the angle out of.
 * @arg angle: The angle between the edge of the solid angle and the
 *     y-axis on [0-PI] measured between the y-axis and wall of the
 *     solid angle.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToSolidAngle(
        const float3 &position,
        const float radius,
        const float angle)
{
    const float2 cylindricalPosition = cartesianToCylindrical(position);

    // The direction from the tip of the conical portion to where it
    // meets the sphere
    const float2 coneEdgeDirection = float2(sin(angle), cos(angle));

    // Distance to the sphere we cut the cone out of
    const float distanceToSphere = length(cylindricalPosition) - radius;
    const float distanceToCone = length(
        cylindricalPosition - coneEdgeDirection * clamp(
            dot(cylindricalPosition, coneEdgeDirection),
            0.0f,
            radius
        )
    );
    const float inside = sign(
        coneEdgeDirection.y * cylindricalPosition.x
        - coneEdgeDirection.x * cylindricalPosition.y
    );

    return max(distanceToSphere, inside * distanceToCone);
}


/**
 * Compute the min distance from a point to a rectangular prism.
 * Centered at the origin.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg width: The width (x) of the prism.
 * @arg height: The height (y) of the prism.
 * @arg depth: The depth (z) of the prism.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToRectangularPrism(
        const float3 &position,
        const float width,
        const float height,
        const float depth)
{
    // Only look at positive quadrant, using symmetry
    const float3 prismToPosition = fabs(position) - float3(width, height, depth) / 2.0f;
    // Clamp the components that are inside the prism to the surface
    // before getting the distance
    return sdfLength(prismToPosition);
}


/**
 * Compute the min distance from a point to the frame of a
 * rectangular prism.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg width:  The width (x) of the frame.
 * @arg height:  The height (y) of the frame.
 * @arg depth:  The depth (z) of the frame.
 * @arg thickness:  The thickness of the frame.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToRectangularPrismFrame(
        const float3 &position,
        const float width,
        const float height,
        const float depth,
        const float thickness)
{
    const float3 prismToPosition = fabs(position) - float3(width, height, depth) / 2.0f;
    const float3 innerReflected = fabs(prismToPosition + thickness) - thickness;

    return min(
        sdfLength(float3(prismToPosition.x, innerReflected.y, innerReflected.z)),
        sdfLength(float3(innerReflected.x, prismToPosition.y, innerReflected.z)),
        sdfLength(float3(innerReflected.x, innerReflected.y, prismToPosition.z))
    );
}


/**
 * Compute the min distance from a point to a rhombus.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg width:  The width (x) of the rhombus.
 * @arg height:  The height (y) of the rhombus.
 * @arg depth:  The depth (z) of the rhombus, this the extruded
 *     dimension, or thickness.
 * @arg cornerRadius:  The radius of the corners of the rhombus'
 *     xy-plane parallel face.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToRhombus(
        const float3 &position,
        const float width,
        const float height,
        const float depth,
        const float cornerRadius)
{
    const float3 absPosition = fabs(position);
    const float2 absPositionXY = float2(absPosition.x, absPosition.y);

    const float halfWidth = width / 2.0f;
    const float halfHeight = height / 2.0f;
    const float2 halfWidthHeight = float2(halfWidth, halfHeight);

    const float2 s = halfWidthHeight - 2.0f * absPositionXY;

    const float f = clamp(
        (
            (halfWidth * s.x - halfHeight * s.y)
            / dot2(halfWidthHeight)
        ),
        -1.0f,
        1.0f
    );

    const float inside = sign(
        absPosition.x * halfHeight + absPosition.y * halfWidth - halfWidth * halfHeight
    );

    const float2 rhombusToPosition = float2(
        inside * length(
            absPositionXY - 0.5f * halfWidthHeight * float2(1.0f - f, 1.0f + f)
        ) - cornerRadius,
        // Closest point along z-axis only depends on the thickness of
        // the extrusion
        absPosition.z - depth / 2.0f
    );

    return sdfLength(rhombusToPosition);
}


/**
 * Compute the min distance from a point to a triangular prism.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg base: The equalateral triangles edge length (xy-plane).
 * @arg depth: The depth (z-axis) of the prism.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToTriangularPrism(
        const float3 &position,
        const float base,
        const float depth)
{
    // 0.28867513459f = tan(PI / 6.0f) / 2.0f, converts base length
    // to the min distance from centroid to edge of triangle

    // 0.86602540378f = cos(PI / 6.0f) = base / height
    // 0.5f = sin(PI / 6.0f) = base / (2 * base)

    return max(
        fabs(position.z) - depth,
        max(
            fabs(position.x) * 0.86602540378f + position.y * 0.5f,
            -position.y
        ) - 0.28867513459f * base
    );
}


/**
 * Compute the min distance from a point to a cylinder
 * Symmetric about the xz-plane.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg radius: The radius (xz-plane) of the cylinder.
 * @arg height: The height (y-axis) of the cylinder.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToCylinder(
        const float3 &position,
        const float radius,
        const float height)
{
    // Cylindrical coordinates (r, h), ignoring the angle due to symmetry
    const float2 cylindricalPosition = fabs(cartesianToCylindrical(position));
    const float2 cylinderToPosition = cylindricalPosition - float2(radius, height / 2);

    return sdfLength(cylinderToPosition);
}


/**
 * Compute the min distance from a point to an infinite cylinder
 * (y-axis aligned).
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg radius: The radius (xz-plane) of the cylinder.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToInfiniteCylinder(const float3 &position, const float radius)
{
    return distanceToCircle(float2(position.x, position.z), radius);
}


/**
 * Compute the min distance from a point to a plane.
 * Anything underneath the plane, as defined by the normal direction
 * pointing above, will be considered inside.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg normal: The normal direction of the plane.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToPlane(const float3 &position, const float3 &normal)
{
    return dot(position, normal);
}


/**
 * Compute the min distance from a point to a capsule.
 * Oriented along the y-axis.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg radius: The radius of the capsule.
 * @arg negativeHeight: The distance along the negative y-axis before
 *     entering the dome.
 * @arg positiveHeight: The distance along the positive y-axis before
 *     entering the dome.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToCapsule(
        const float3 &position,
        const float radius,
        const float negativeHeight,
        const float positiveHeight)
{
    return length(float3(
        position.x,
        position.y - clamp(position.y, -negativeHeight, positiveHeight),
        position.z
    )) - radius;
}



/**
 * Compute the min distance from a point to a cone
 * (y-axis aligned). The tip of the cone is at the origin, and it opens
 * up the y-axis.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg angle: The angle between the tip and base of the cone [0-PI/2)
 *     measured between the y-axis and wall of the cone.
 * @arg height: The height (y-axis) of the cone. Cannot be 0.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToCone(
        const float3 &position,
        const float angle,
        const float height)
{
    // Cylindrical coordinates (r, h), ignoring the angle due to symmetry
    const float2 cylindricalPosition = cartesianToCylindrical(position);

    // The cylindrical coordinates of the edge of the cone base
    const float2 cylindricalBound = float2(fabs(height * tan(angle)), height);

    // Vector from the top surface of the cone to the position given
    const float2 coneTopToPosition = cylindricalPosition - cylindricalBound * float2(
        saturate(cylindricalPosition.x / cylindricalBound.x),
        1
    );
    // Vector from the edge of the cone to the position given
    const float2 coneEdgeToPosition = (
        cylindricalPosition - cylindricalBound * saturate(
            dot(cylindricalPosition, cylindricalBound)
            / dot2(cylindricalBound)
        )
    );

    const float heightSign = sign(height);

    // -1 if the position is inside the cone, +1 if it is outside
    const float inside = sign(max(
        heightSign * (cylindricalPosition.x * height - cylindricalPosition.y * cylindricalBound.x),
        heightSign * (cylindricalPosition.y - height)
    ));
    // The distance is the minimum between the distance to the edge and
    // the distance to the base
    return inside * minLength(coneEdgeToPosition, coneTopToPosition);
}


/**
 * Compute the min distance from a point to an infinite cone
 * (y-axis aligned). The tip of the cone is at the origin, and it opens
 * up the y-axis.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg angle: The angle between the tip and base of the cone [0-PI/2)
 *     measured between the y-axis and wall of the cone.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToInfiniteCone(const float3 &position, const float angle)
{
    // The normalized cylindrical coordinates of the edge of the cone base
    const float2 coneEdgeDirection = float2(sin(angle), cos(angle));

    // Cylindrical coordinates (r, h), ignoring the angle due to symmetry
    const float2 cylindricalPosition = cartesianToCylindrical(position);

    // -1 if the position is inside the cone, +1 if it is outside
    const float inside = sign(
        cylindricalPosition.x * coneEdgeDirection.y
        - cylindricalPosition.y * coneEdgeDirection.x
    );

    // The shortest path is always to the cones edge, or tip if we are
    // below it. The dot product projects the position onto the cone
    // edge, and taking the positive part clamps the cone above the
    // xz-plane
    return inside * length(
        cylindricalPosition
        - coneEdgeDirection * positivePart(dot(cylindricalPosition, coneEdgeDirection))
    );
}


/**
 * Compute the min distance from a point to a capped cone.
 * Oriented along the y-axis.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg height: The height (y-axis) of the cone, centered at the origin
 *     Cannot be 0.
 * @arg lowerRadius: The radius of the cone at y = -height/2.
 * @arg upperRadius: The radius of the cone at y = height/2.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToCappedCone(
        const float3 &position,
        const float height,
        const float lowerRadius,
        const float upperRadius)
{
    const float halfHeight = height / 2.0f;
    const float2 cylindricalPosition = cartesianToCylindrical(position);

    // The 'corners' are the apparent corners when the shape is
    // projected onto the xy-plane
    const float2 upperCorner = float2(upperRadius, halfHeight);
    const float2 lowerToUpperCorner = float2(upperRadius - lowerRadius, height);

    const float2 coneTopOrBottomToPosition = float2(
        cylindricalPosition.x - min(
            cylindricalPosition.x,
            (cylindricalPosition.y < 0.0f) ? lowerRadius : upperRadius
        ),
        fabs(cylindricalPosition.y) - halfHeight
    );
    const float2 coneEdgeToPosition = (
        cylindricalPosition
        - upperCorner
        + lowerToUpperCorner * saturate(
            dot(upperCorner - cylindricalPosition, lowerToUpperCorner)
            / dot2(lowerToUpperCorner)
        )
    );

    float inside;
    if (coneEdgeToPosition.x < 0.0f && coneTopOrBottomToPosition.y < 0.0f)
    {
        inside = -1.0f;
    }
    else
    {
        inside = 1.0f;
    }

    return inside * minLength(coneTopOrBottomToPosition, coneEdgeToPosition);
}


/**
 * Compute the min distance from a point to a rounded cone.
 * Oriented along the y-axis.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg height: The height (y-axis) of the rounded cone.
 * @arg lowerRadius: The radius of the cone at y = 0.
 * @arg upperRadius: The radius of the cone at y = height.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToRoundedCone(
        const float3 &position,
        const float height,
        const float lowerRadius,
        const float upperRadius)
{
    const float2 cylindricalPosition = cartesianToCylindrical(position);

    // Get the unit vector that is normal to the conical surface in 2D
    const float parallelX = (upperRadius - lowerRadius) / height;
    const float parallelY = sqrt(1.0f - parallelX * parallelX);
    const float2 parallel = float2(parallelX, parallelY);

    const float positionProjectedOnCone = dot(cylindricalPosition, parallel);

    if (positionProjectedOnCone < 0.0f)
    {
        // Closest point is on the lower sphere
        return length(cylindricalPosition) - lowerRadius;
    }
    else if (positionProjectedOnCone > parallelY * height)
    {
        // Closest point is on the upper sphere
        return length(cylindricalPosition - float2(0.0f, height)) - upperRadius;
    }

    // Closest point is on the conical surface, so project the position
    // onto the cone's normal direction, then offset it by the lower radius
    return dot(cylindricalPosition, float2(parallelY, -parallelX)) - lowerRadius;
}


/**
 * Compute the min distance from a point to a torus.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg ringRadius: The radius (xy-plane) of the ring of the torus.
 * @arg tubeRadius: The radius of the tube of the torus.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToTorus(
        const float3 &position,
        const float ringRadius,
        const float tubeRadius)
{
    const float distanceToRingInXYPlane = distanceToCircle(
        float2(position.x, position.y),
        ringRadius
    );
    return distanceToCircle(
        float2(distanceToRingInXYPlane, position.z),
        tubeRadius
    );
}


/**
 * Compute the min distance from a point to a capped torus.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg ringRadius: The radius (xy-plane) of the ring of the torus.
 * @arg tubeRadius: The radius of the tube of the torus.
 * @arg capAngle: The angle (xy-plane, symmetric about y-axis) to cap
 *     at, in the range (0-PI).
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToCappedTorus(
        const float3 &position,
        const float ringRadius,
        const float tubeRadius,
        const float capAngle)
{
    const float2 capDirection = float2(sin(capAngle), cos(capAngle));
    const float3 absXPosition = float3(fabs(position.x), position.y, position.z);
    const float2 posXY = float2(absXPosition.x, absXPosition.y);

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
        dot2(absXPosition)
        + ringRadius * ringRadius
        - 2.0f * ringRadius * capFactor
    ) - tubeRadius;
}


/**
 * Compute the min distance from a point to a chain link.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg ringRadius: The radius (xy-plane) of the ring of the torus that
 *     will be stretched to create the link.
 * @arg tubeRadius: The radius of the tube that makes the link.
 * @arg height: The height (y-axis) to elongate the torus.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToLink(
        const float3 &position,
        const float ringRadius,
        const float tubeRadius,
        const float height)
{
    const float heightDifference = fabs(position.y) - height / 2.0f;

    const float distanceInXYPlane = distanceToCircle(
        float2(position.x, positivePart(heightDifference)),
        ringRadius
    );
    return distanceToCircle(
        float2(distanceInXYPlane, position.z),
        tubeRadius
    );
}


/**
 * Compute the min distance from a point to a hexagonal prism.
 * The hexagonal face is parallel to the xy-plane, centered at the
 * origin.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg height: The height (y) of the prism.
 * @arg depth: The depth (z) of the prism.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToHexagonalPrism(
        const float3 &position,
        const float height,
        const float depth)
{
    // -cos(-PI / 6.0f), -sin(-PI / 6.0f), -tan(-PI / 6.0f)
    const float2 kCosSin = float2(-0.86602540378f, 0.5f);
    const float kTan = 0.57735026919f;
    const float halfHeight = height / 2.0f;

    const float3 absPosition = fabs(position);
    float2 absPositionXY = float2(absPosition.x, absPosition.y);

    absPositionXY += 2.0f * negativePart(dot(kCosSin, absPositionXY)) * kCosSin;

    // Radial distance in xy-plane, and the distance along the z-axis
    const float2 radialAndZDistance = float2(
        sign(absPositionXY.y - halfHeight) * length(
            absPositionXY - float2(
                clamp(absPositionXY.x, -kTan * halfHeight, kTan * halfHeight),
                halfHeight
            )
        ),
        absPosition.z - depth / 2.0f
    );

    // Return the positive distance if we are outside, negative if we are inside
    return sdfLength(radialAndZDistance);
}


/**
 * Compute the min distance from a point to a octahedron.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg radialExtent: The maximum distance along the x, y, and z axes.
 *     ie. The vertices are at +/-radialExtent on the x, y, and z axes.
 *
 * @returns: The minimum distance from the point to the shape.
 */
inline float distanceToOctahedron(
        const float3 &position,
        const float radialExtent)
{
    const float3 absPosition = fabs(position);

    const float positionSumToExtent = dot(absPosition, float3(1)) - radialExtent;

    const float3 threePosition = 3.0f * absPosition;
    float3 changeOfAxes;
    if (threePosition.x < positionSumToExtent)
    {
        changeOfAxes = absPosition;
    }
    else if (threePosition.y < positionSumToExtent)
    {
        changeOfAxes = float3(
            absPosition.y,
            absPosition.z,
            absPosition.x
        );
    }
    else if (threePosition.z < positionSumToExtent)
    {
        changeOfAxes = float3(
            absPosition.z,
            absPosition.x,
            absPosition.y
        );
    }
    else
    {
        return positionSumToExtent * 0.57735027f;
    }

    const float surface = clamp(
        0.5f * (changeOfAxes.z - changeOfAxes.y + radialExtent),
        0.0f,
        radialExtent
    );

    return length(float3(
        changeOfAxes.x,
        changeOfAxes.y - radialExtent + surface,
        changeOfAxes.z - surface
    ));
}


/**
 * Compute the min distance from a point to a mandelbulb.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg power: One greater than the axes of symmetry in the xy-plane.
 * @arg iterations: The number of iterations to compute, the higher this
 *     is the slower it will be to compute, but the deeper the fractal
 *     will have detail.
 * @arg maxSquareRadius: When the square radius has reached this length,
 *     stop iterating.
 *
 * @returns: The minimum distance from the point to the shape.
 */
float distanceToMandelbulb(
        const float3 &position,
        const float power,
        const int iterations,
        const float maxSquareRadius,
        float4 &trapColour)
{
    float3 currentPosition = position;
    float radiusSquared = dot2(currentPosition);

    float3 absPosition = fabs(currentPosition);
    trapColour = float4(absPosition.x, absPosition.y, absPosition.z, radiusSquared);

    float dradius = 1.0f;
    for (int i=0; i < iterations; i++)
    {
        dradius = power * pow(radiusSquared, (power - 1) / 2) * dradius + 1.0f;

        float currentRadius = length(currentPosition);
        float theta = power * acos(currentPosition.z / currentRadius);
        float phi = power * atan2(currentPosition.y, currentPosition.x);

        currentPosition = position + pow(currentRadius, power) * float3(
            sin(theta) * cos(phi),
            sin(theta) * sin(phi),
            cos(theta)
        );

        absPosition = fabs(currentPosition);
        trapColour = min(
            trapColour,
            float4(absPosition.x, absPosition.y, absPosition.z, radiusSquared)
        );

        radiusSquared = dot2(currentPosition);
        if(radiusSquared > maxSquareRadius)
        {
            break;
        }
    }

    trapColour = saturate(trapColour);

    return 0.25f * log(radiusSquared) * sqrt(radiusSquared) / dradius;
}


/**
 * Compute the min distance from a point to a mandelbulb.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg power: One greater than the axes of symmetry in the xy-plane.
 * @arg iterations: The number of iterations to compute, the higher this
 *     is the slower it will be to compute, but the deeper the fractal
 *     will have detail.
 * @arg maxSquareRadius: When the square radius has reached this length,
 *     stop iterating.
 *
 * @returns: The minimum distance from the point to the shape.
 */
float distanceToMandelbulb(
        const float3 &position,
        const float power,
        const int iterations,
        const float maxSquareRadius)
{
    float3 currentPosition = position;
    float radiusSquared = dot2(currentPosition);

    float3 absPosition = fabs(currentPosition);

    float dradius = 1.0f;
    for (int i=0; i < iterations; i++)
    {
        dradius = power * pow(radiusSquared, (power - 1) / 2) * dradius + 1.0f;

        float currentRadius = length(currentPosition);
        float theta = power * acos(currentPosition.z / currentRadius);
        float phi = power * atan2(currentPosition.y, currentPosition.x);

        currentPosition = position + pow(currentRadius, power) * float3(
            sin(theta) * cos(phi),
            sin(theta) * sin(phi),
            cos(theta)
        );

        absPosition = fabs(currentPosition);

        radiusSquared = dot2(currentPosition);
        if(radiusSquared > maxSquareRadius)
        {
            break;
        }
    }

    return 0.25f * log(radiusSquared) * sqrt(radiusSquared) / dradius;
}


inline float3 boxFold(const float3 &position, const float3 &foldingLimit)
{
    return clamp(position, -foldingLimit, foldingLimit) * 2.0f - position;
}


inline float4 sphereFold(
        const float4 &position,
        const float radiusSquared,
        const float minSquareRadius)
{
    return position * saturate(max(minSquareRadius / radiusSquared, minSquareRadius));
}


/**
 * Compute the min distance from a point to a mandelbox.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg scale:
 * @arg iterations: The number of iterations to compute, the higher this
 *     is the slower it will be to compute, but the deeper the fractal
 *     will have detail.
 *
 * @returns: The minimum distance from the point to the shape.
 */
float distanceToMandelbox(
        const float3 &position,
        const float scale,
        const int iterations,
        const float minSquareRadius,
        const float foldingLimit,
        float4 &trapColour)
{
    const float4 scaleVector = float4(
        scale,
        scale,
        scale,
        fabs(scale)
    ) / minSquareRadius;

    const float4 initialPosition = float4(
        position.x,
        position.y,
        position.z,
        1.0f
    );
    float4 currentPosition = initialPosition;

    const float3 foldingLimit3 = float3(foldingLimit);

    for (int i=0; i < iterations; i++)
    {
        float3 currentPosition3 = float3(
            currentPosition.x,
            currentPosition.y,
            currentPosition.z
        );
        currentPosition3 = boxFold(currentPosition3, foldingLimit3);

        const float radiusSquared = dot2(currentPosition3);
        currentPosition = sphereFold(
            float4(
                currentPosition3.x,
                currentPosition3.y,
                currentPosition3.z,
                currentPosition.w
            ),
            radiusSquared,
            minSquareRadius
        );

        currentPosition = scaleVector * currentPosition + initialPosition;
        trapColour = min(
            trapColour,
            fabs(currentPosition)
        );
    }

    trapColour = saturate(trapColour);

    return (
        length(
            float3(
                currentPosition.x,
                currentPosition.y,
                currentPosition.z
            ) - fabs(scale - 1.0f)
        ) / currentPosition.w - pow(fabs(scale), (float) (1 - iterations))
    );
}


/**
 * Compute the min distance from a point to a mandelbox.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg scale:
 * @arg iterations: The number of iterations to compute, the higher this
 *     is the slower it will be to compute, but the deeper the fractal
 *     will have detail.
 *
 * @returns: The minimum distance from the point to the shape.
 */
float distanceToMandelbox(
        const float3 &position,
        const float scale,
        const int iterations,
        const float minSquareRadius,
        const float foldingLimit)
{
    const float4 scaleVector = float4(
        scale,
        scale,
        scale,
        fabs(scale)
    ) / minSquareRadius;

    const float4 initialPosition = float4(
        position.x,
        position.y,
        position.z,
        1.0f
    );
    float4 currentPosition = initialPosition;

    const float3 foldingLimit3 = float3(foldingLimit);

    for (int i=0; i < iterations; i++)
    {
        float3 currentPosition3 = float3(
            currentPosition.x,
            currentPosition.y,
            currentPosition.z
        );
        currentPosition3 = boxFold(currentPosition3, foldingLimit3);

        const float radiusSquared = dot2(currentPosition3);
        currentPosition = sphereFold(
            float4(
                currentPosition3.x,
                currentPosition3.y,
                currentPosition3.z,
                currentPosition.w
            ),
            radiusSquared,
            minSquareRadius
        );

        currentPosition = scaleVector * currentPosition + initialPosition;
    }

    return (
        length(
            float3(
                currentPosition.x,
                currentPosition.y,
                currentPosition.z
            ) - fabs(scale - 1.0f)
        ) / currentPosition.w - pow(fabs(scale), (float) (1 - iterations))
    );
}


/**
 * Compute the min distance from a point to a geometric object.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg shapeType: The selection of shape to get the distance to, options:
 *     0: sphere
 *     1: ellipsoid
 *     2: cut sphere
 *     3: hollow sphere
 *     4: death star
 *     5: solid angle
 *     6: rectangular prism
 *     7: rectangular prism frame
 *     8: rhombus
 *     9: triangular prism
 *     10: cylinder
 *     11: infinite cylinder
 *     12: plane
 *     13: capsule
 *     14: cone
 *     15: infinite cone
 *     16: capped cone
 *     17: rounded cone
 *     18: torus
 *     19: capped torus
 *     20: link
 *     21: hexagonal prism
 *     22: octahedron
 * @arg dimensions: The dimensions of the object.
 *
 * @returns: The minimum distance from the point to the shape.
 */
float distanceToColourlessObject(
        const float3 &position,
        const int shapeType,
        const float4 &dimensions)
{
    if (shapeType == 0)
    {
        return distanceToSphere(position, dimensions.x);
    }
    if (shapeType == 1)
    {
        return distanceToEllipsoid(
            position,
            dimensions.x,
            dimensions.y,
            dimensions.z
        );
    }
    if (shapeType == 2)
    {
        return distanceToCutSphere(position, dimensions.x, dimensions.y);
    }
    if (shapeType == 3)
    {
        return distanceToHollowSphere(
            position,
            dimensions.x,
            dimensions.y,
            dimensions.z
        );
    }
    if (shapeType == 4)
    {
        return distanceToDeathStar(
            position,
            dimensions.x,
            dimensions.y,
            dimensions.z
        );
    }
    if (shapeType == 5)
    {
        return distanceToSolidAngle(
            position,
            dimensions.x,
            degreesToRadians(dimensions.y)
        );
    }
    if (shapeType == 6)
    {
        return distanceToRectangularPrism(
            position,
            dimensions.x,
            dimensions.y,
            dimensions.z
        );
    }
    if (shapeType == 7)
    {
        return distanceToRectangularPrismFrame(
            position,
            dimensions.x,
            dimensions.y,
            dimensions.z,
            dimensions.w
        );
    }
    if (shapeType == 8)
    {
        return distanceToRhombus(
            position,
            dimensions.x,
            dimensions.y,
            dimensions.z,
            dimensions.w
        );
    }
    if (shapeType == 9)
    {
        return distanceToTriangularPrism(position, dimensions.x, dimensions.y);
    }
    if (shapeType == 10)
    {
        return distanceToCylinder(position, dimensions.x, dimensions.y);
    }
    if (shapeType == 11)
    {
        return distanceToInfiniteCylinder(position, dimensions.x);
    }
    if (shapeType == 12)
    {
        return distanceToPlane(
            position,
            normalize(float3(dimensions.x, dimensions.y, dimensions.z))
        );
    }
    if (shapeType == 13)
    {
        return distanceToCapsule(
            position,
            dimensions.x,
            dimensions.y,
            dimensions.z
        );
    }
    if (shapeType == 14)
    {
        return distanceToCone(
            position,
            degreesToRadians(dimensions.x),
            dimensions.y
        );
    }
    if (shapeType == 15)
    {
        return distanceToInfiniteCone(position, degreesToRadians(dimensions.x));
    }
    if (shapeType == 16)
    {
        return distanceToCappedCone(
            position,
            dimensions.x,
            dimensions.y,
            dimensions.z
        );
    }
    if (shapeType == 17)
    {
        return distanceToRoundedCone(
            position,
            dimensions.x,
            dimensions.y,
            dimensions.z
        );
    }
    if (shapeType == 18)
    {
        return distanceToTorus(position, dimensions.x, dimensions.y);
    }
    if (shapeType == 19)
    {
        return distanceToCappedTorus(
            position,
            dimensions.x,
            dimensions.y,
            degreesToRadians(dimensions.z)
        );
    }
    if (shapeType == 20)
    {
        return distanceToLink(
            position,
            dimensions.x,
            dimensions.y,
            dimensions.z
        );
    }
    if (shapeType == 21)
    {
        return distanceToHexagonalPrism(position, dimensions.x, dimensions.y);
    }
    if (shapeType == 22)
    {
        return distanceToOctahedron(position, dimensions.x);
    }

    return 0;
}


/**
 * Compute the min distance from a point to a geometric object.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg shapeType: The selection of shape to get the distance to, options:
 *     0: sphere
 *     1: ellipsoid
 *     2: cut sphere
 *     3: hollow sphere
 *     4: death star
 *     5: solid angle
 *     6: rectangular prism
 *     7: rectangular prism frame
 *     8: rhombus
 *     9: triangular prism
 *     10: cylinder
 *     11: infinite cylinder
 *     12: plane
 *     13: capsule
 *     14: cone
 *     15: infinite cone
 *     16: capped cone
 *     17: rounded cone
 *     18: torus
 *     19: capped torus
 *     20: link
 *     21: hexagonal prism
 *     22: octahedron
 *     23: mandelbulb
 *     24: mandelbox
 * @arg dimensions: The dimensions of the object.
 * @arg modifications: The modifications to perform.
 *     Each bit will enable a modification:
 *         bit 13: enable diffuse trap colour
 *         bit 14: enable specular trap colour
 *         bit 15: enable absorption trap colour
 *         bit 16: enable emission trap colour
 * @arg diffuseColour: The diffuse colour of the surface will be stored
 *     here. This will only be modified by the fractals (23-25).
 * @arg specularColour: The specular colour of the surface will be
 *     stored here. This will only be modified by the fractals (23-25).
 * @arg absorptionColour: The absorption colour of the surface will be
 *     stored here. This will only be modified by the fractals (23-25).
 * @arg emissionColour: The emission colour of the surface will be
 *     stored here. This will only be modified by the fractals (23-25).
 *
 * @returns: The minimum distance from the point to the shape.
 */
float distanceToObject(
        const float3 &position,
        const int shapeType,
        const float4 &dimensions,
        const int modifications,
        float4 &diffuseColour,
        float4 &specularColour,
        float4 &absorptionColour,
        float4 &emissionColour)
{
    if (shapeType >= 23)
    {
        float4 colour = float4(1);
        float distance = FLT_MAX;
        if (shapeType == 23)
        {
            distance = distanceToMandelbulb(
                position,
                dimensions.x,
                (int) dimensions.y,
                dimensions.z,
                colour
            );
        }
        if (shapeType == 24)
        {
            distance = distanceToMandelbox(
                position,
                dimensions.x,
                (int) dimensions.y,
                dimensions.z,
                dimensions.w,
                colour
            );
        }
        diffuseColour = modifications & 8192 ? diffuseColour * colour : diffuseColour;
        specularColour = modifications & 16384 ? specularColour * colour : specularColour;
        absorptionColour = modifications & 32768 ? absorptionColour * colour : absorptionColour;
        emissionColour = modifications & 65536 ? emissionColour * colour : emissionColour;

        return distance;
    }

    return distanceToColourlessObject(position, shapeType, dimensions);
}


/**
 * Compute the min distance from a point to a geometric object.
 *
 * @arg position: The point to get the distance to, from the object.
 * @arg shapeType: The selection of shape to get the distance to, options:
 *     0: sphere
 *     1: ellipsoid
 *     2: cut sphere
 *     3: hollow sphere
 *     4: death star
 *     5: solid angle
 *     6: rectangular prism
 *     7: rectangular prism frame
 *     8: rhombus
 *     9: triangular prism
 *     10: cylinder
 *     11: infinite cylinder
 *     12: plane
 *     13: capsule
 *     14: cone
 *     15: infinite cone
 *     16: capped cone
 *     17: rounded cone
 *     18: torus
 *     19: capped torus
 *     20: link
 *     21: hexagonal prism
 *     22: octahedron
 *     23: mandelbulb
 *     24: mandelbox
 * @arg dimensions: The dimensions of the object.
 *
 * @returns: The minimum distance from the point to the shape.
 */
float distanceToObject(
        const float3 &position,
        const int shapeType,
        const float4 &dimensions)
{
    if (shapeType == 23)
    {
        return distanceToMandelbulb(
            position,
            dimensions.x,
            (int) dimensions.y,
            dimensions.z
        );
    }
    if (shapeType == 24)
    {
        return distanceToMandelbox(
            position,
            dimensions.x,
            (int) dimensions.y,
            dimensions.z,
            dimensions.w
        );
    }

    return distanceToColourlessObject(position, shapeType, dimensions);
}


/**
 * Compute the modified distance to an object.
 *
 * @arg rayOrigin: The location the ray originates from.
 * @arg shape: The shape of the object:
 *     0: sphere
 *     1: ellipsoid
 *     2: cut sphere
 *     3: hollow sphere
 *     4: death star
 *     5: solid angle
 *     6: rectangular prism
 *     7: rectangular prism frame
 *     8: rhombus
 *     9: triangular prism
 *     10: cylinder
 *     11: infinite cylinder
 *     12: plane
 *     13: capsule
 *     14: cone
 *     15: infinite cone
 *     16: capped cone
 *     17: rounded cone
 *     18: torus
 *     19: capped torus
 *     20: link
 *     21: hexagonal prism
 *     22: octahedron
 *     23: mandelbulb
 *     24: mandelbox
 *     25: mandelbox (no trap colour)
 * @arg dimensions: The dimensions of the object.
 * @arg uniformScale: The factor to scale the object by.
 * @arg modifications: The modifications to perform.
 *     Each bit will enable a modification:
 *         bit 6: hollowing
 *         bit 13: enable diffuse trap colour
 *         bit 14: enable specular trap colour
 *         bit 15: enable absorption trap colour
 *         bit 16: enable emission trap colour
 * @arg edgeRadius: The radius to round the edges by.
 * @arg wallThickness: The thickness of the walls if hollowing the
 *     object.
 * @arg diffuseColour: The diffuse colour of the surface will be stored
 *     here. This will only be modified by the fractals (23-25).
 * @arg specularColour: The specular colour of the surface will be
 *     stored here. This will only be modified by the fractals (23-25).
 * @arg absorptionColour: The absorption colour of the surface will be
 *     stored here. This will only be modified by the fractals (23-25).
 * @arg emissionColour: The emission colour of the surface will be
 *     stored here. This will only be modified by the fractals (23-25).
 *
 * @returns: The distance to the modified object.
 */
inline float getModifiedDistance(
    const float3 &rayOrigin,
    const int shape,
    const float4 &dimensions,
    const float uniformScale,
    const int modifications,
    const float edgeRadius,
    const float wallThickness,
    float4 &diffuseColour,
    float4 &specularColour,
    float4 &absorptionColour,
    float4 &emissionColour)
{
    float nextDistance = distanceToObject(
        rayOrigin / uniformScale,
        shape,
        dimensions,
        modifications,
        diffuseColour,
        specularColour,
        absorptionColour,
        emissionColour
    ) * uniformScale;

    return performDistanceModification(
        modifications,
        edgeRadius,
        wallThickness,
        nextDistance
    );
}


/**
 * Compute the modified distance to an object.
 *
 * @arg rayOrigin: The location the ray originates from.
 * @arg shape: The shape of the object:
 *     0: sphere
 *     1: ellipsoid
 *     2: cut sphere
 *     3: hollow sphere
 *     4: death star
 *     5: solid angle
 *     6: rectangular prism
 *     7: rectangular prism frame
 *     8: rhombus
 *     9: triangular prism
 *     10: cylinder
 *     11: infinite cylinder
 *     12: plane
 *     13: capsule
 *     14: cone
 *     15: infinite cone
 *     16: capped cone
 *     17: rounded cone
 *     18: torus
 *     19: capped torus
 *     20: link
 *     21: hexagonal prism
 *     22: octahedron
 *     23: mandelbulb
 *     24: mandelbox
 *     25: mandelbox (no trap colour)
 * @arg dimensions: The dimensions of the object.
 * @arg uniformScale: The factor to scale the object by.
 * @arg modifications: The modifications to perform.
 *     Each bit will enable a modification:
 *         bit 6: hollowing
 * @arg edgeRadius: The radius to round the edges by.
 * @arg wallThickness: The thickness of the walls if hollowing the
 *     object.
 *
 * @returns: The distance to the modified object.
 */
inline float getModifiedDistance(
    const float3 &rayOrigin,
    const int shape,
    const float4 &dimensions,
    const float uniformScale,
    const int modifications,
    const float edgeRadius,
    const float wallThickness)
{
    float nextDistance = distanceToObject(
        rayOrigin / uniformScale,
        shape,
        dimensions
    ) * uniformScale;

    return performDistanceModification(
        modifications,
        edgeRadius,
        wallThickness,
        nextDistance
    );
}

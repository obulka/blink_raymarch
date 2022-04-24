# BlinkScript Ray Marcher for Nuke

This project contains three gizmos that allow you to ray march a wide variety of shapes on the GPU.

## Setup

Simply clone/download this repo and add the top level directory to your Nuke path. The gizmos will appear in the nodes menu, and all callbacks and the `FN_BLINK_INCLUDE_PATHS` will be set up automatically.

## Gizmos

### sdf_primitive

This gizmo lets you choose the shape, dimensions, location, and surface properties of an object. It takes other sdf_primitives as inputs, and all nodes in the 'children' input will be positioned relative to it. The children will also interact with the shape according to your selection, allowing you to intersect, subtract, and blend the objects. You can also use any shape as a bounding volume of its children in order to improve performance.

The surface properties include reflection, transmission, diffuse colour, roughness, and emission. You can select the refractive index of the object as well as the medium the camera, and therefore the ray, starts in.

You can elongate, mirror, repeat (both finitely and infinitely) with no extra memory cost, and hollow the objects as well.

The current available shapes are:
- sphere
- ellipsoid
- cut sphere
- hollow sphere
- death star
- solid angle
- rectangular prism
- rectangular prism frame
- rhombus
- triangular prism
- cylinder
- infinite cylinder
- plane
- capsule
- cone
- infinite cone
- capped cone
- rounded cone
- torus
- capped torus
- link
- hexagonal prism
- octahedron
- mandelbulb
- mandelbox

### sdf_light

This gizmo allows you to light the scene with point, directional, or simple ambient lighting. You can choose the colour, intensity, and falloff of the light. You can also soften the shadows with a slider.

### ray_march

This gizmo renders the scene using a ray marching algorithm, with support for hdri image based lighting. You can inptut a standard nuke camera and the perspective projection, axes, and world space coordinates, will match that of Nuke's native scanline renderer, and general 3D system.

## References
- https://iquilezles.org/articles/distfunctions/
- http://blog.hvidtfeldts.net/index.php/2011/09/distance-estimated-3d-fractals-v-the-mandelbulb-different-de-approximations/

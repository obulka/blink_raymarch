# BlinkScript Ray Marcher for Nuke

This project contains three gizmos that allow you to ray march a wide variety of shapes, including fractals, utilizing the GPU. Included are two blink kernels, one that is used to precompute the irradiance of a latlong, hdr image, and the other is the main ray marching engine.

![mandelbox_trap_test1_reduced](https://user-images.githubusercontent.com/21975584/164989275-4eb4791c-df89-4332-981d-aac79b607762.png)

![test_trap_bulb_done_reduced](https://user-images.githubusercontent.com/21975584/164989470-d13cabe2-8eb1-42de-becb-bd2672b14538.png)

<p align="center">
  <img src="https://user-images.githubusercontent.com/21975584/165710806-19b473e3-8d11-4cd8-8b49-55bf3d1a4544.gif" alt="animated" />
</p>

## Setup

Simply clone/download this repo and add the following line to your `init.py`: `nuke.pluginAddPath("/path/to/blink_raymarch/src/python")`, replacing "`/path/to`" with the actual path to the repository. The gizmos will appear in the nodes menu under the "SDF" group, and all callbacks and the `FN_BLINK_INCLUDE_PATHS` will be set up automatically. The project is also available at: http://www.nukepedia.com/blink/3d/blink-ray-marcher

## Requirements

This project has been tested in Nuke 12.0v8, 12.1v5, and 13.0v2. I recommend using 12.0 if you do not have much GPU memory, because it still uses OpenCL for blinkscript, rather than cuda. The cuda version will continue to increase memory consumption until crashing, so the entire framerange needs to fit in memory. You will want to have a GPU, the above examples were rendered using an NVIDIA GeForce GTX 980 Ti.

## Gizmos

### sdf_primitive

This gizmo lets you choose the shape, dimensions, location, and surface properties of an object. It takes other sdf_primitives as inputs, and all nodes in the 'children' input will be positioned relative to it. The children will also interact with the shape according to your selection, allowing you to intersect, subtract, and blend the objects. You can also use any shape as a bounding volume of its children in order to improve performance.

The surface properties include:
- reflection
- transmission
- diffuse colour
- roughness
- emission (placeholder for when 'path marched' global illumination is added)

You can select the refractive index of the object as well as the medium the camera, and therefore the ray, starts in.

There are also controls for:
- elongating about the x, y, and z axes
- mirroring the object across the x, y, and z axes
- repeating the object on a grid both finitely and infinitely with no extra memory cost
- hollowing out the objects, which can be used in combination with intersections

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

This gizmo allows you to light the scene with a few different light types, namely:
- point
- directional
- ambient
- ambient occlusion

You can choose the colour, intensity, and falloff of the light. You can also soften the shadows with a slider.

### ray_march

This gizmo renders the scene using a ray marching algorithm, with support for hdri image based lighting. You can input a standard nuke camera and the perspective projection, axes, and world space coordinates, will match that of Nuke's native scanline renderer, and general 3D system.

The AOV options are:
- scene
- normal
- position
- depth

## References
- https://iquilezles.org/articles/distfunctions/
- http://blog.hvidtfeldts.net/index.php/2011/09/distance-estimated-3d-fractals-v-the-mandelbulb-different-de-approximations/

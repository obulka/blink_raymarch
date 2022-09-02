# BlinkScript Ray/Path Marcher for Nuke

This project contains five gizmos that allow you to ray march, and path march, a wide variety of shapes, including fractals, utilizing the GPU.

![nested_dielectrics_final](https://user-images.githubusercontent.com/21975584/185783312-db9a7f74-cf07-4655-b566-57b265afcf58.png)

![blended](https://user-images.githubusercontent.com/21975584/179328892-de242ee0-f3ec-4f9f-bcd4-eba3d76b6ea6.png)

![room_test](https://user-images.githubusercontent.com/21975584/186559860-cf376ee5-8875-4865-8a08-3c2e57a5587c.png)

![mandelbox_trap_test1_reduced](https://user-images.githubusercontent.com/21975584/164989275-4eb4791c-df89-4332-981d-aac79b607762.png)

![test_trap_bulb_done_reduced](https://user-images.githubusercontent.com/21975584/164989470-d13cabe2-8eb1-42de-becb-bd2672b14538.png)

<p align="center">
  <img src="https://user-images.githubusercontent.com/21975584/165710806-19b473e3-8d11-4cd8-8b49-55bf3d1a4544.gif" alt="animated" />
</p>

## Setup

Simply clone/download this repo and add the following line to your `init.py`: `nuke.pluginAddPath("/path/to/blink_raymarch/src/python")`, replacing "`/path/to`" with the actual path to the repository. The gizmos will appear in the nodes menu under the "SDF" group, and all callbacks and the `FN_BLINK_INCLUDE_PATHS` will be set up automatically. The project is also available at: http://www.nukepedia.com/blink/3d/blink-ray-marcher

## Requirements

This project has been tested in Nuke 12.0v8, 12.1v5, and 13.0v2. Note that after Nuke 12.0 the VRAM cache limits are determined by the same settings as the CPU cache limits in Edit->Preferences->Performance->Caching, so if the limits exceed what your GPU can handle, then the VRAM will continue to fill up until Nuke either crashes or falls back to the CPU, which is very slow for these nodes. For that reason you will want to have a GPU. The above examples were rendered using an NVIDIA GeForce GTX 980 Ti.

## Gizmos

### ray_march

This gizmo renders the scene using a ray marching algorithm, with support for:
- global illumination
- multiple importance sampled direct illumination
    - set the 'max light sampling bounces' to be > 0 to enable this
- equi-angular sampling for participating media
    - includes volumetric caustics if you lower the 'light sampling bias' and increase the 'max light sampling bounces' knobs
    - increase the 'equi-angular samples' knob for clearer results when using an 'sdf_noise' node with 'scattering' enabled
- adaptive sampling using a normalized variance AOV
    - plug a 'ray_march' node's output, or a previous render with different seeds, into the 'previous' input of another 'ray_march' node
    - set the minimum and maximum paths to trace, and the node will adaptively interpolate between the values
    - the first node in the chain will always trace the maximum paths
    - be sure to change the seed on each chained node
- nested dielectrics
- depth of field based on the camera input, simply check the 'enable dof' knob
- hdr image based lighting

The alpha channel contains unique ids for each object that is hit on the first bounce.

You can input a standard nuke camera and the perspective projection, axes, and world space coordinates, will match that of Nuke's native scanline renderer, and general 3D system.

The AOV options are:
- beauty
- normal
- world position
- local position
- depth
- stats

The stats AOV gives you the average number of iterations, the average number of bounces, and the total number of paths traced per pixel in the r, g, and b channels, respectively.

### sdf_material

This gizmo lets you set the surface properties of an object, and can be passed into an 'sdf_primitive' node in order to apply the surface material to the primitive.

The material properties include:
- diffuse colour
- specular
- specular roughness
- specular colour
- transmission
- transmission roughness
- absorption colour
- refractive index
- scattering coefficient
- emission
- emission colour

### sdf_noise

This gizmo allows you to vary the material properties of an 'sdf_material' node, or blend between them. It allows for a position seeded, turbulence or fBm noise, with all the properties of Nuke's built-in noise node, plus the additional ability to modify the black and white points, and lift. You can also invert the noise, and select which material properties will be affected by it. This node can be passed into a material, and it will affect that material and that material only. You can also pass it directly into the 'path_march' node's 'noise' input for use with the global scattering coefficient.

### sdf_primitive

This gizmo lets you choose the shape, dimensions, and location of an object. It takes other sdf_primitives as inputs, and all nodes in the 'children' input will be positioned relative to it. The children will also interact with the shape according to your selection, allowing you to intersect, subtract, and blend the objects. You can also use any shape as a bounding volume of its children in order to improve performance.

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
- ambient
- ambient occlusion
- point
- directional

You can choose the colour, intensity, and falloff of the light. You can also soften the shadows with a slider.

## References
- https://iquilezles.org/articles/distfunctions/
- http://blog.hvidtfeldts.net/index.php/2011/09/distance-estimated-3d-fractals-v-the-mandelbulb-different-de-approximations/
- https://blog.demofox.org/2020/05/25/casual-shadertoy-path-tracing-1-basic-camera-diffuse-emissive/
- https://www.researchgate.net/publication/354065092_Multiple_Importance_Sampling_101
- https://www.arnoldrenderer.com/research/egsr2012_volume.pdf

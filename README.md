# BlinkScript Ray Marcher for Nuke

This project contains three gizmos that allow you to ray march a wide variety of shapes on the GPU.

## Gizmos

### sdf_primitive

This gizmo lets you choose the shape, dimensions, location, and surface properties of an object. It takes other sdf_primitives as inputs, and all nodes in the 'children' input will be positioned relative to it. The children will also interact with the shape according to your selection, allowing you to intersect, subtract, and blend the objects. You can also use any shape as a bounding volume of its children in order to improve performance.

The surface properties include reflection, transmission, diffuse colour, roughness, and emission. You can select the refractive index of the object as well as the medium the camera, and therefore the ray, starts in.

You can elongate, mirror, repeat, and hollow the object.


### sdf_light

This gizmo allows you to light the scene with point, directional, or simple ambient lighting. You can choose the colour, intensity, and falloff of the light. You can also soften the shadows with a slider.

### ray_march

This gizmo renders the scene using a ray marching algorithm, with support for hdri image based lighting.

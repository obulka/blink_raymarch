from collections import OrderedDict

import nuke


class SDFPrimitive():

    dimensional_knob_prefix = "dimension_"
    dimensional_knob_defaults = {
        "sphere": OrderedDict({
            "radius": {
                "default": .5,
                "tooltip": "The radius of the sphere.",
            },
        }),
        "ellipsoid": OrderedDict({
            "x radius": {
                "default": .5,
                "tooltip": "The radius along the x-axis of the ellipsoid.",
            },
            "y radius": {
                "default": .25,
                "tooltip": "The radius along the y-axis of the ellipsoid.",
            },
            "z radius": {
                "default": .25,
                "tooltip": "The radius along the z-axis of the ellipsoid.",
            },
        }),
        "cut sphere": OrderedDict({
            "radius": {
                "default": .5,
                "tooltip": "The radius of the sphere.",
            },
            "height": {
                "default": .25,
                "tooltip": "The height (y-axis) below which the sphere is culled.",
            },
        }),
        "hollow sphere": OrderedDict({
            "radius": {
                "default": .5,
                "tooltip": "The radius of the sphere.",
            },
            "height": {
                "default": .25,
                "tooltip": "The height (y-axis) at which an opening is created.",
            },
            "thickness": {
                "default": .05,
                "tooltip": "The thickness of the walls of the hollow sphere.",
            },
        }),
        "death star": OrderedDict({
            "solid radius": {
                "default": .5,
                "tooltip": "The radius of the sphere that remains solid.",
            },
            "hollow radius": {
                "default": .5,
                "tooltip": "The radius of the sphere that is cut from the solid.",
            },
            "hollow height": {
                "default": .75,
                "tooltip": (
                    "The height (y-axis) of the center of the sphere that is cut "
                    "from the solid, above solidRadius + hollowRadius, the result "
                    "will be a standard sphere of radius solidRadius."
                ),
            },
        }),
        "solid angle": OrderedDict({
            "radius": {
                "default": .5,
                "tooltip": "The radius of the sphere to cut the angle out of.",
            },
            "angle": {
                "default": 1.,
                "tooltip": (
                    "The angle between the edge of the solid angle and the "
                    "y-axis on [0-PI] measured between the y-axis and wall of the "
                    "solid angle."
                ),
            },
        }),
        "rectangular prism": OrderedDict({
            "width": {
                "default": .5,
                "tooltip": "The width (x) of the prism.",
            },
            "height": {
                "default": .75,
                "tooltip": "The height (y) of the prism.",
            },
            "depth": {
                "default": .25,
                "tooltip": "The depth (z) of the prism.",
            },
        }),
        "rectangular prism frame": OrderedDict({
            "width": {
                "default": .5,
                "tooltip": "The width (x) of the frame.",
            },
            "height": {
                "default": .75,
                "tooltip": "The height (y) of the frame.",
            },
            "depth": {
                "default": .25,
                "tooltip": "The depth (z) of the frame.",
            },
            "thickness": {
                "default": .05,
                "tooltip": "The thickness of the frame.",
            },
        }),
        "rhombus": OrderedDict({
            "width": {
                "default": .5,
                "tooltip": "The width (x) of the rhombus.",
            },
            "height": {
                "default": .75,
                "tooltip": "The height (y) of the rhombus.",
            },
            "depth": {
                "default": .25,
                "tooltip": (
                    "The depth (z) of the rhombus, this the extruded dimension, "
                    "or thickness."
                ),
            },
            "corner radius": {
                "default": .05,
                "tooltip": (
                    "The radius of the corners of the rhombus' xy-plane parallel "
                    " face."
                ),
            },
        }),
        "triangular prism": OrderedDict({
            "base": {
                "default": .5,
                "tooltip": "The equalateral triangles edge length (xy-plane).",
            },
            "depth": {
                "default": .75,
                "tooltip": "The depth (z-axis) of the prism.",
            },
        }),
        "cylinder": OrderedDict({
            "radius": {
                "default": .5,
                "tooltip": "The radius (xz-plane) of the cylinder.",
            },
            "height": {
                "default": .75,
                "tooltip": "The height (y-axis) of the cylinder.",
            },
        }),
        "infinite cylinder": OrderedDict({
            "radius": {
                "default": .5,
                "tooltip": "The radius (xz-plane) of the cylinder.",
            },
        }),
        "plane": OrderedDict({
            "normal x": {
                "default": 0.,
                "tooltip": "The x component of the normal direction of the plane.",
            },
            "normal y": {
                "default": 0.,
                "tooltip": "The y component of the normal direction of the plane.",
            },
            "normal z": {
                "default": 1.,
                "tooltip": "The z component of the normal direction of the plane.",
            },
        }),
        "capsule": OrderedDict({
            "radius": {
                "default": .25,
                "tooltip": "The radius of the capsule.",
            },
            "negative height": {
                "default": .25,
                "tooltip": "The distance along the negative y-axis before entering the dome.",
            },
            "positive height": {
                "default": .25,
                "tooltip": "The distance along the positive y-axis before entering the dome.",
            },
        }),
        "cone": OrderedDict({
            "angle": {
                "default": .5,
                "tooltip": (
                    "The angle between the tip and base of the cone [0-PI/2) "
                    "measured between the y-axis and wall of the cone."
                ),
            },
            "height": {
                "default": .33,
                "tooltip": "The height (y-axis) of the cone. Cannot be 0.",
            },
        }),
        "infinite cone": OrderedDict({
            "angle": {
                "default": .5,
                "tooltip": (
                    "The angle between the tip and base of the cone [0-PI/2) "
                    "measured between the y-axis and wall of the cone."
                ),
            },
        }),
        "capped cone": OrderedDict({
            "height": {
                "default": .5,
                "tooltip": (
                    "The height (y-axis) of the cone, centered at the origin. "
                    "Cannot be 0."
                ),
            },
            "lower radius": {
                "default": .5,
                "tooltip": "The radius of the cone at y = -height/2.",
            },
            "upper radius": {
                "default": .25,
                "tooltip": "The radius of the cone at y = height/2.",
            },
        }),
        "rounded cone": OrderedDict({
            "height": {
                "default": .3,
                "tooltip": "The height (y-axis) of the rounded cone.",
            },
            "lower radius": {
                "default": .2,
                "tooltip": "The radius of the cone at y = 0.",
            },
            "upper radius": {
                "default": .1,
                "tooltip": "The radius of the cone at y = height.",
            },
        }),
        "torus": OrderedDict({
            "ring radius": {
                "default": .3,
                "tooltip": "The radius (xy-plane) of the ring of the torus.",
            },
            "tube radius": {
                "default": .2,
                "tooltip": "The radius of the tube of the torus.",
            },
        }),
        "capped torus": OrderedDict({
            "ring radius": {
                "default": .3,
                "tooltip": "The radius (xy-plane) of the ring of the torus.",
            },
            "tube radius": {
                "default": .2,
                "tooltip": "The radius of the tube of the torus.",
            },
            "cap angle": {
                "default": 1.,
                "tooltip": (
                    "The angle (xy-plane, symmetric about y-axis) to cap at, in "
                    "the range (0-PI)."
                ),
            },
        }),
        "link": OrderedDict({
            "ring radius": {
                "default": .3,
                "tooltip": (
                    "The radius (xy-plane) of the ring of the torus that will be "
                    "stretched to create the link."
                ),
            },
            "tube radius": {
                "default": .2,
                "tooltip": "The radius of the tube that makes the link.",
            },
            "height": {
                "default": .1,
                "tooltip": "The height (y-axis) to elongate the torus.",
            },
        }),
        "hexagonal prism": OrderedDict({
            "height": {
                "default": .5,
                "tooltip": "The height (y) of the prism.",
            },
            "depth": {
                "default": .5,
                "tooltip": "The depth (z) of the prism.",
            },
        }),
        "octahedron": OrderedDict({
            "radial extent": {
                "default": .5,
                "tooltip": (
                    "The maximum distance along the x, y, and z axes. "
                    "ie. The vertices are at +/-radial_extent on the x, y, and z axes."
                ),
            },
        }),
        "mandelbulb": OrderedDict({
            "power": {
                "default": 8.,
                "tooltip": "One greater than the axes of symmetry in the xy-plane.",
            },
            "iterations": {
                "default": 10.,
                "tooltip": (
                    "The number of iterations to compute, the higher this "
                    "is the slower it will be to compute, but the deeper the "
                    "fractal will have detail."
                ),
            },
            "max square radius": {
                "default": 4.,
                "tooltip": (
                    "When the square radius has reached this length, stop "
                    "iterating."
                ),
            },
        }),
    }

    @staticmethod
    def handle_knob_changed():
        changed_knob = nuke.thisKnob()
        if changed_knob.name() == "shape":
            SDFPrimitive._handle_shape_changed(nuke.thisNode(), changed_knob)

    @staticmethod
    def _handle_shape_changed(node, shape_knob):
        default_values = SDFPrimitive.dimensional_knob_defaults[
            shape_knob.enumName(int(shape_knob.getValue()))
        ]

        dimensional_knobs = [
            node.knob(SDFPrimitive.dimensional_knob_prefix + axis)
            for axis in ("x", "y", "z", "w")
        ]

        for (knob_name, knob_values), dimensional_knob in zip(
            default_values.items(),
            dimensional_knobs
        ):
            dimensional_knob.setVisible(True)
            dimensional_knob.setLabel(knob_name)
            dimensional_knob.setValue(knob_values["default"])
            dimensional_knob.setTooltip(knob_values["tooltip"])

        for dimensional_knob in dimensional_knobs[len(default_values):]:
            dimensional_knob.setVisible(False)


SDFPrimitive.handle_knob_changed()

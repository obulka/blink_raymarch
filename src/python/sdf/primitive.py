"""Knob management for sdf primitives.

# Add on knob changed callback to sdf_primitive group:
nuke.toNode("sdf_primitive").knob("knobChanged").setValue(
    "__import__('sdf.primitive', fromlist='SDFPrimitive').SDFPrimitive().handle_knob_changed()"
)

# Add on node create callback to sdf_primitive group:
nuke.toNode("sdf_primitive").knob("onCreate").setValue(
    "__import__('sdf.primitive', fromlist='SDFPrimitive').SDFPrimitive().handle_node_created()"
)
"""
from collections import OrderedDict
from math import pi

from .knob_manager import KnobChangedCallbacks, SDFKnobManager


class SDFPrimitive(SDFKnobManager):
    """Knob manager for primitive shapes in signed distance fields."""

    shape_knob_name = "shape"
    hollow_knob_name = "hollow"
    wall_thickness_knob_name = "wall_thickness"
    elongate_knob_name = "elongate"
    elongation_knob_name = "elongation"
    is_bound_knob_name = "is_bound"
    blend_strength_knob_name = "blend_strength"
    blend_type_knob_name = "blend_type"

    _dimensional_axes = SDFKnobManager._dimensional_axes + ("w",)

    _knob_changed_callbacks = KnobChangedCallbacks(SDFKnobManager._knob_changed_callbacks)

    dimensional_knob_defaults = {
        "sphere": OrderedDict([
            (
                "radius",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": "The radius of the sphere.",
                },
            ),
        ]),
        "ellipsoid": OrderedDict([
            (
                "x radius",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": "The radius along the x-axis of the ellipsoid.",
                },
            ),
            (
                "y radius",
                {
                    "default": .25,
                    "range": (0., 10.),
                    "tooltip": "The radius along the y-axis of the ellipsoid.",
                },
            ),
            (
                "z radius",
                {
                    "default": .25,
                    "range": (0., 10.),
                    "tooltip": "The radius along the z-axis of the ellipsoid.",
                },
            ),
        ]),
        "cut sphere": OrderedDict([
            (
                "radius",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": "The radius of the sphere.",
                },
            ),
            (
                "height",
                {
                    "default": .25,
                    "range": (0., 10.),
                    "tooltip": "The height (y-axis) below which the sphere is culled.",
                },
            ),
        ]),
        "hollow sphere": OrderedDict([
            (
                "radius",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": "The radius of the sphere.",
                },
            ),
            (
                "height",
                {
                    "default": .25,
                    "range": (0., 10.),
                    "tooltip": "The height (y-axis) at which an opening is created.",
                },
            ),
            (
                "thickness",
                {
                    "default": .05,
                    "range": (0., 1.),
                    "tooltip": "The thickness of the walls of the hollow sphere.",
                },
            ),
        ]),
        "death star": OrderedDict([
            (
                "solid radius",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": "The radius of the sphere that remains solid.",
                },
            ),
            (
                "hollow radius",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": "The radius of the sphere that is cut from the solid.",
                },
            ),
            (
                "hollow height",
                {
                    "default": .75,
                    "range": (0., 10.),
                    "tooltip": (
                        "The height (y-axis) of the center of the sphere that is cut "
                        "from the solid, above solidRadius + hollowRadius, the result "
                        "will be a standard sphere of radius solidRadius."
                    ),
                },
            ),
        ]),
        "solid angle": OrderedDict([
            (
                "radius",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": "The radius of the sphere to cut the angle out of.",
                },
            ),
            (
                "angle",
                {
                    "default": 1.,
                    "range": (0., pi),
                    "tooltip": (
                        "The angle between the edge of the solid angle and the "
                        "y-axis on [0-PI] measured between the y-axis and wall of the "
                        "solid angle."
                    ),
                },
            ),
        ]),
        "rectangular prism": OrderedDict([
            (
                "width",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": "The width (x) of the prism.",
                },
            ),
            (
                "height",
                {
                    "default": .75,
                    "range": (0., 10.),
                    "tooltip": "The height (y) of the prism.",
                },
            ),
            (
                "depth",
                {
                    "default": .25,
                    "range": (0., 10.),
                    "tooltip": "The depth (z) of the prism.",
                },
            ),
        ]),
        "rectangular prism frame": OrderedDict([
            (
                "width",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": "The width (x) of the frame.",
                },
            ),
            (
                "height",
                {
                    "default": .75,
                    "range": (0., 10.),
                    "tooltip": "The height (y) of the frame.",
                },
            ),
            (
                "depth",
                {
                    "default": .25,
                    "range": (0., 10.),
                    "tooltip": "The depth (z) of the frame.",
                },
            ),
            (
                "thickness",
                {
                    "default": .05,
                    "range": (0., 1.),
                    "tooltip": "The thickness of the frame.",
                },
            ),
        ]),
        "rhombus": OrderedDict([
            (
                "width",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": "The width (x) of the rhombus.",
                },
            ),
            (
                "height",
                {
                    "default": .75,
                    "range": (0., 10.),
                    "tooltip": "The height (y) of the rhombus.",
                },
            ),
            (
                "depth",
                {
                    "default": .25,
                    "range": (0., 10.),
                    "tooltip": (
                        "The depth (z) of the rhombus, this the extruded dimension, "
                        "or thickness."
                    ),
                },
            ),
            (
                "corner radius",
                {
                    "default": .05,
                    "range": (0., 1.),
                    "tooltip": (
                        "The radius of the corners of the rhombus' xy-plane parallel "
                        " face."
                    ),
                },
            ),
        ]),
        "triangular prism": OrderedDict([
            (
                "base",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": "The equalateral triangles edge length (xy-plane).",
                },
            ),
            (
                "depth",
                {
                    "default": .75,
                    "range": (0., 10.),
                    "tooltip": "The depth (z-axis) of the prism.",
                },
            ),
        ]),
        "cylinder": OrderedDict([
            (
                "radius",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": "The radius (xz-plane) of the cylinder.",
                },
            ),
            (
                "height",
                {
                    "default": .75,
                    "range": (0., 10.),
                    "tooltip": "The height (y-axis) of the cylinder.",
                },
            ),
        ]),
        "infinite cylinder": OrderedDict([
            (
                "radius",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": "The radius (xz-plane) of the cylinder.",
                },
            ),
        ]),
        "plane": OrderedDict([
            (
                "normal x",
                {
                    "default": 0.,
                    "range": (0., 1.),
                    "tooltip": "The x component of the normal direction of the plane.",
                },
            ),
            (
                "normal y",
                {
                    "default": 0.,
                    "range": (0., 1.),
                    "tooltip": "The y component of the normal direction of the plane.",
                },
            ),
            (
                "normal z",
                {
                    "default": 1.,
                    "range": (0., 1.),
                    "tooltip": "The z component of the normal direction of the plane.",
                },
            ),
        ]),
        "capsule": OrderedDict([
            (
                "radius",
                {
                    "default": .25,
                    "range": (0., 10.),
                    "tooltip": "The radius of the capsule.",
                },
            ),
            (
                "negative height",
                {
                    "default": .25,
                    "range": (0., 10.),
                    "tooltip": "The distance along the negative y-axis before entering the dome.",
                },
            ),
            (
                "positive height",
                {
                    "default": .25,
                    "range": (0., 10.),
                    "tooltip": "The distance along the positive y-axis before entering the dome.",
                },
            ),
        ]),
        "cone": OrderedDict([
            (
                "angle",
                {
                    "default": .5,
                    "range": (0., pi / 2.),
                    "tooltip": (
                        "The angle between the tip and base of the cone [0-PI/2) "
                        "measured between the y-axis and wall of the cone."
                    ),
                },
            ),
            (
                "height",
                {
                    "default": .33,
                    "range": (0., 10.),
                    "tooltip": "The height (y-axis) of the cone. Cannot be 0.",
                },
            ),
        ]),
        "infinite cone": OrderedDict([
            (
                "angle",
                {
                    "default": .5,
                    "range": (0., pi / 2.),
                    "tooltip": (
                        "The angle between the tip and base of the cone [0-PI/2) "
                        "measured between the y-axis and wall of the cone."
                    ),
                },
            ),
        ]),
        "capped cone": OrderedDict([
            (
                "height",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": (
                        "The height (y-axis) of the cone, centered at the origin. "
                        "Cannot be 0."
                    ),
                },
            ),
            (
                "lower radius",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": "The radius of the cone at y = -height/2.",
                },
            ),
            (
                "upper radius",
                {
                    "default": .25,
                    "range": (0., 10.),
                    "tooltip": "The radius of the cone at y = height/2.",
                },
            ),
        ]),
        "rounded cone": OrderedDict([
            (
                "height",
                {
                    "default": .3,
                    "range": (0., 10.),
                    "tooltip": "The height (y-axis) of the rounded cone.",
                },
            ),
            (
                "lower radius",
                {
                    "default": .2,
                    "range": (0., 10.),
                    "tooltip": "The radius of the cone at y = 0.",
                },
            ),
            (
                "upper radius",
                {
                    "default": .1,
                    "range": (0., 10.),
                    "tooltip": "The radius of the cone at y = height.",
                },
            ),
        ]),
        "torus": OrderedDict([
            (
                "ring radius",
                {
                    "default": .3,
                    "range": (0., 10.),
                    "tooltip": "The radius (xy-plane) of the ring of the torus.",
                },
            ),
            (
                "tube radius",
                {
                    "default": .2,
                    "range": (0., 5.),
                    "tooltip": "The radius of the tube of the torus.",
                },
            ),
        ]),
        "capped torus": OrderedDict([
            (
                "ring radius",
                {
                    "default": .3,
                    "range": (0., 10.),
                    "tooltip": "The radius (xy-plane) of the ring of the torus.",
                },
            ),
            (
                "tube radius",
                {
                    "default": .2,
                    "range": (0., 5.),
                    "tooltip": "The radius of the tube of the torus.",
                },
            ),
            (
                "cap angle",
                {
                    "default": 1.,
                    "range": (0., pi),
                    "tooltip": (
                        "The angle (xy-plane, symmetric about y-axis) to cap at, in "
                        "the range (0-PI)."
                    ),
                },
            ),
        ]),
        "link": OrderedDict([
            (
                "ring radius",
                {
                    "default": .3,
                    "range": (0., 10.),
                    "tooltip": (
                        "The radius (xy-plane) of the ring of the torus that will be "
                        "stretched to create the link."
                    ),
                },
            ),
            (
                "tube radius",
                {
                    "default": .2,
                    "range": (0., 5.),
                    "tooltip": "The radius of the tube that makes the link.",
                },
            ),
            (
                "height",
                {
                    "default": .1,
                    "range": (0., 10.),
                    "tooltip": "The height (y-axis) to elongate the torus.",
                },
            ),
        ]),
        "hexagonal prism": OrderedDict([
            (
                "height",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": "The height (y) of the prism.",
                },
            ),
            (
                "depth",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": "The depth (z) of the prism.",
                },
            ),
        ]),
        "octahedron": OrderedDict([
            (
                "radial extent",
                {
                    "default": .5,
                    "range": (0., 10.),
                    "tooltip": (
                        "The maximum distance along the x, y, and z axes. "
                        "ie. The vertices are at +/-radial_extent on the x, y, and z axes."
                    ),
                },
            ),
        ]),
        "mandelbulb": OrderedDict([
            (
                "power",
                {
                    "default": 8.,
                    "range": (2., 30.),
                    "tooltip": "One greater than the axes of symmetry in the xy-plane.",
                },
            ),
            (
                "iterations",
                {
                    "default": 10.,
                    "range": (1., 30.),
                    "tooltip": (
                        "The number of iterations to compute, the higher this "
                        "is the slower it will be to compute, but the deeper the "
                        "fractal will have detail."
                    ),
                },
            ),
            (
                "max square radius",
                {
                    "default": 4.,
                    "range": (1., 9.),
                    "tooltip": (
                        "When the square radius has reached this length, stop "
                        "iterating."
                    ),
                },
            ),
            (
                "orbital trap blend strength",
                {
                    "default": 1.,
                    "range": (0., 1.),
                    "tooltip": (
                        "Transition between orbital trap based colouring and "
                        "the colour provided by the 'colour' knob."
                    ),
                },
            ),
        ]),
        "mandelbox": OrderedDict([
            (
                "scale",
                {
                    "default": -1.75,
                    "range": (-5., 5.),
                    "tooltip": (
                        "The amount to scale the position between folds. "
                        "Can be negative or positive."
                    ),
                },
            ),
            (
                "iterations",
                {
                    "default": 12.,
                    "range": (1., 30.),
                    "tooltip": (
                        "The number of iterations to compute, the higher this "
                        "is the slower it will be to compute, but the more detail "
                        "the fractal will have."
                    ),
                },
            ),
            (
                "min square radius",
                {
                    "default": 0.001,
                    "range": (0.00001, 0.1),
                    "tooltip": "The minimum square radius to use when spherically folding.",
                },
            ),
            (
                "folding limit",
                {
                    "default": 0.8,
                    "range": (0.01, 2.),
                    "tooltip": (
                        "Clamp the position between +/- this value when performing "
                        "the box fold."
                    ),
                },
            ),
        ]),
    }

    def __init__(self):
        """Initialize the manager"""
        super(SDFPrimitive, self).__init__()

        self._knob_names_only_enabled_if_parent = {
            SDFPrimitive.is_bound_knob_name,
            SDFPrimitive.blend_strength_knob_name,
            SDFPrimitive.blend_type_knob_name,
        }

    @_knob_changed_callbacks.register(shape_knob_name)
    def _shape_changed(self):
        """Dynamically enable/disable and change the labels/tooltips/values
        of the dimensional knobs when the selected shape has changed.
        """
        self._dropdown_context_changed(
            self.dimensional_knob_defaults,
            self.dimensional_context_knobs,
            set_node_label=True,
        )

    @_knob_changed_callbacks.register(hollow_knob_name)
    def _hollow_changed(self):
        """Dynamically enable/disable the wall thickness knob depending
        on whether or not hollowing has been enabled.
        """
        self._node.knob(self.wall_thickness_knob_name).setEnabled(self._knob.value())

    @_knob_changed_callbacks.register(elongate_knob_name)
    def _elongate_changed(self):
        """Dynamically enable/disable the elongation knob depending
        on whether or not elongate has been enabled.
        """
        self._node.knob(self.elongation_knob_name).setEnabled(self._knob.value())

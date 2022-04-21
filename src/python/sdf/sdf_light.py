"""Knob management for sdf lights.


# Add on knob changed callback to sdf_light group:
nuke.toNode("sdf_light").knob("knobChanged").setValue(
    "__import__('sdf.sdf_light', fromlist='SDFLight').SDFLight().handle_knob_changed()"
)

# Add on node create callback to sdf_light group:
nuke.toNode("sdf_light").knob("onCreate").setValue(
    "__import__('sdf.sdf_light', fromlist='SDFLight').SDFLight().handle_node_created()"
)
"""
from collections import OrderedDict

from .knob_manager import SDFKnobManager
from .utils import KnobChangedCallbacks


class SDFLight(SDFKnobManager):
    """Knob manager for light shapes in signed distance fields."""

    type_knob_name = "type"
    soften_shadows_knob_name = "soften_shadows"
    shadow_hardness_knob_name = "shadow_hardness"
    falloff_knob_name = "falloff"

    _knob_changed_callbacks = KnobChangedCallbacks(SDFKnobManager._knob_changed_callbacks)

    dimensional_knob_prefix = "dimension_"
    dimensional_knob_defaults = {
        "ambient": OrderedDict([]),
        "directional": OrderedDict([
            (
                "x direction",
                {
                    "default": 0.,
                    "tooltip": "The x direction of the light.",
                },
            ),
            (
                "y direction",
                {
                    "default": -1.,
                    "tooltip": "The y direction of the light.",
                },
            ),
            (
                "z direction",
                {
                    "default": 0.,
                    "tooltip": "The z direction of the light.",
                },
            ),
        ]),
        "point": OrderedDict([
            (
                "x position",
                {
                    "default": 0.,
                    "tooltip": "The x position of the light.",
                },
            ),
            (
                "y position",
                {
                    "default": 1.,
                    "tooltip": "The y position of the light.",
                },
            ),
            (
                "z position",
                {
                    "default": 0.,
                    "tooltip": "The z position of the light.",
                },
            ),
        ]),
        "ambient occlusion": OrderedDict([
            (
                "iterations",
                {
                    "default": 5,
                    "tooltip": "The number of iterations to compute.",
                },
            ),
        ]),
    }

    @_knob_changed_callbacks.register(type_knob_name)
    def _type_changed(self):
        """Dynamically enable/disable and change the labels/tooltips/values
        of the dimensional knobs when the selected type has changed.
        """
        self._dropdown_context_changed()

        self._node.knob(self.falloff_knob_name).setEnabled("ambient" not in self._knob.name())

    @_knob_changed_callbacks.register(soften_shadows_knob_name)
    def _soften_shadows_changed(self):
        """Dynamically enable/disable the wall thickness knob depending
        on whether or not hollowing has been enabled.
        """
        self._node.knob(self.shadow_hardness_knob_name).setEnabled(self._knob.value())

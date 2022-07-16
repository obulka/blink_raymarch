# Copyright 2022 by Owen Bulka.
# All rights reserved.
# This file is released under the "MIT License Agreement".
# Please see the LICENSE.md file that should have been included as part
# of this package.
"""Knob management for sdf lights.

# Add on knob changed callback to sdf_light group:
nuke.toNode("sdf_light").knob("knobChanged").setValue(
    "__import__('sdf.light', fromlist='SDFLight').SDFLight().handle_knob_changed()"
)

# Add on node create callback to sdf_light group:
nuke.toNode("sdf_light").knob("onCreate").setValue(
    "__import__('sdf.light', fromlist='SDFLight').SDFLight().handle_node_created()"
)
"""
from collections import OrderedDict

from .knob_manager import KnobChangedCallbacks, SDFGeoKnobManager
from .utils import rgb_to_hex


class SDFLight(SDFGeoKnobManager):
    """Knob manager for light shapes in signed distance fields."""

    type_knob_name = "type"
    soften_shadows_knob_name = "soften_shadows"
    shadow_hardness_knob_name = "shadow_hardness"
    falloff_knob_name = "falloff"
    colour_knob_name = "colour"

    _knob_changed_callbacks = KnobChangedCallbacks(SDFGeoKnobManager._knob_changed_callbacks)

    _dimensional_axes = ("x", "y")

    dimensional_knob_defaults = {
        "directional": OrderedDict([
            (
                "direction",
                {
                    "default": [0., -1., 0.],
                    "tooltip": "The direction of the light.",
                },
            ),
        ]),
        "point": OrderedDict([
            (
                "position",
                {
                    "default": [0., 1., 0.],
                    "tooltip": "The position of the light.",
                },
            ),
        ]),
        "ambient": OrderedDict(),
        "ambient occlusion": OrderedDict([
            (
                "iterations",
                {
                    "default": 5.,
                    "range": (1., 10.),
                    "tooltip": "The number of iterations to compute.",
                },
            ),
        ]),
    }

    def _update_ambient_dependent_knobs(self):
        """Enable/disable the knobs that are not used by ambient lights"""
        not_ambient = "ambient" not in self._knob.value()

        self._node.knob(self.falloff_knob_name).setEnabled(not_ambient)

        soften_shadows_knob = self._node.knob(self.soften_shadows_knob_name)
        soften_shadows_knob.setEnabled(not_ambient)
        self._node.knob(self.shadow_hardness_knob_name).setEnabled(
            not_ambient and soften_shadows_knob.value(),
        )

    @_knob_changed_callbacks.register(colour_knob_name)
    def _colour_changed(self):
        """Change the node colour to match the object for easier ID."""
        self._node.knob("tile_color").setValue(rgb_to_hex(self._knob.value()))

    @_knob_changed_callbacks.register(type_knob_name)
    def _type_changed(self):
        """Dynamically enable/disable and change the labels/tooltips/values
        of the context knobs when the selected type has changed.
        """
        self._dropdown_context_changed(
            self.dimensional_knob_defaults,
            self.dimensional_context_knobs,
            set_node_label=True,
        )
        self._update_ambient_dependent_knobs()

    @_knob_changed_callbacks.register(soften_shadows_knob_name)
    def _soften_shadows_changed(self):
        """Dynamically enable/disable the shadow hardness knob depending
        on whether or not shadow softening has been enabled.
        """
        self._node.knob(self.shadow_hardness_knob_name).setEnabled(self._knob.value())

    def _dropdown_context_changed(self, default_dict, context_knobs, set_node_label=False):
        """Dynamically enable/disable and change the labels/tooltips/values
        of the context knobs dependent on dropdown knobs.

        Args:
            default_dict (dict(str, OrderedDict(str, dict()))): The
                default values for the context sensitive knobs.

            context_knobs (list(nuke.Knob)): The knobs that change with
                the context.

            set_node_label (bool): Set the node's label to the context
                value if True.
        """
        if "ambient" in self._knob.value():
            context_knobs = context_knobs[::-1]

        super(SDFLight, self)._dropdown_context_changed(
            default_dict,
            context_knobs,
            set_node_label=set_node_label,
        )

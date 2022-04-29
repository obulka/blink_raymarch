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

from .knob_manager import KnobChangedCallbacks, SDFKnobManager


class SDFLight(SDFKnobManager):
    """Knob manager for light shapes in signed distance fields."""

    type_knob_name = "type"
    soften_shadows_knob_name = "soften_shadows"
    shadow_hardness_knob_name = "shadow_hardness"
    falloff_knob_name = "falloff"

    _knob_changed_callbacks = KnobChangedCallbacks(SDFKnobManager._knob_changed_callbacks)

    dimensional_knob_defaults = {
        "directional": OrderedDict([
            (
                "x direction",
                {
                    "default": 0.,
                    "range": (-1., 1.),
                    "tooltip": "The x direction of the light.",
                },
            ),
            (
                "y direction",
                {
                    "default": -1.,
                    "range": (-1., 1.),
                    "tooltip": "The y direction of the light.",
                },
            ),
            (
                "z direction",
                {
                    "default": 0.,
                    "range": (-1., 1.),
                    "tooltip": "The z direction of the light.",
                },
            ),
        ]),
        "point": OrderedDict([
            (
                "x position",
                {
                    "default": 0.,
                    "range": (-10., 10.),
                    "tooltip": "The x position of the light.",
                },
            ),
            (
                "y position",
                {
                    "default": 1.,
                    "range": (-10., 10.),
                    "tooltip": "The y position of the light.",
                },
            ),
            (
                "z position",
                {
                    "default": 0.,
                    "range": (-10., 10.),
                    "tooltip": "The z position of the light.",
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

# Copyright 2022 by Owen Bulka.
# All rights reserved.
# This file is released under the "MIT License Agreement".
# Please see the LICENSE.md file that should have been included as part
# of this package.
"""Knob management for sdf primitives.

# Add on knob changed callback to sdf_primitive group:
nuke.toNode("sdf_material").knob("knobChanged").setValue(
    "__import__('sdf.material', fromlist='SDFMaterial').SDFMaterial().handle_knob_changed()"
)
"""
from collections import OrderedDict

from .knob_manager import KnobChangedCallbacks, KnobManager
from .utils import rgb_to_hex


class SDFMaterial(KnobManager):
    """Knob manager for primitive shapes in signed distance fields."""

    colour_knob_names = (
        "colour",
        "specular_colour",
        "extinction_colour",
        "scattering_colour",
        "emission_colour",
    )

    _knob_changed_callbacks = KnobChangedCallbacks(KnobManager._knob_changed_callbacks)


    @_knob_changed_callbacks.register_multiple(colour_knob_names)
    def _colour_changed(self):
        """Change the node colour to match the object for easier ID."""
        self._node.knob("tile_color").setValue(rgb_to_hex(self._knob.value()))

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

from .knob_manager import KnobChangedCallbacks, SDFKnobManager
from .utils import rgb_to_hex


class SDFMaterial(SDFKnobManager):
    """Knob manager for primitive shapes in signed distance fields."""

    specular_knob_name = "specular"
    transmission_knob_name = "transmission"
    colour_knob_name = "colour"

    _knob_changed_callbacks = KnobChangedCallbacks(SDFKnobManager._knob_changed_callbacks)


    @_knob_changed_callbacks.register(colour_knob_name)
    def _colour_changed(self):
        """Change the node colour to match the object for easier ID."""
        self._node.knob("tile_color").setValue(rgb_to_hex(self._knob.value()))

    @_knob_changed_callbacks.register(specular_knob_name)
    def _specular_changed(self):
        """Ensure that specular + transmission <= 1"""
        specular = self._knob.value()
        transmission_knob = self._node.knob(self.transmission_knob_name)

        if specular + transmission_knob.value() > 1.:
            transmission_knob.setValue(1. - specular)

    @_knob_changed_callbacks.register(transmission_knob_name)
    def _transmission_changed(self):
        """Ensure that specular + transmission <= 1"""
        transmission = self._knob.value()
        specular_knob = self._node.knob(self.specular_knob_name)

        if transmission + specular_knob.value() > 1.:
            specular_knob.setValue(1. - transmission)

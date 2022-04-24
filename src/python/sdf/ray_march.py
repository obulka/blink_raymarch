# Copyright 2022 by Owen Bulka.
# All rights reserved.
# This file is released under the "MIT License Agreement".
# Please see the LICENSE.md file that should have been included as part
# of this package.
"""Knob management for sdf lights.

# Add on knob changed callback to sdf_light group:
nuke.toNode("ray_march").knob("knobChanged").setValue(
    "__import__('sdf.ray_march', fromlist='RayMarch').RayMarch().handle_knob_changed()"
)
"""
from .knob_manager import KnobChangedCallbacks, KnobManager


class RayMarch(KnobManager):
    """Knob manager for the ray marcher."""

    soften_shadows_knob_name = "soften_shadows"
    shadow_hardness_knob_name = "hdri_shadow_hardness"

    _knob_changed_callbacks = KnobChangedCallbacks(KnobManager._knob_changed_callbacks)


    @_knob_changed_callbacks.register(soften_shadows_knob_name)
    def _soften_shadows_changed(self):
        """Dynamically enable/disable the shadow hardness knob depending
        on whether or not shadow softening has been enabled.
        """
        self._node.knob(self.shadow_hardness_knob_name).setEnabled(self._knob.value())

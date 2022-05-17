# Copyright 2022 by Owen Bulka.
# All rights reserved.
# This file is released under the "MIT License Agreement".
# Please see the LICENSE.md file that should have been included as part
# of this package.
"""Knob management for sdf lights.

# Add on knob changed callback to sdf_light group:
nuke.toNode("path_march").knob("knobChanged").setValue(
    "__import__('sdf.path_march', fromlist='PathMarch').PathMarch().handle_knob_changed()"
)
"""
from .knob_manager import KnobChangedCallbacks, KnobManager


class PathMarch(KnobManager):
    """Knob manager for the ray marcher."""

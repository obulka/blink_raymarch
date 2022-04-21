"""Knob management for primitive shapes in signed distance fields."""
import nuke

from .utils import KnobChangedCallbacks, KnobManager, rgb_to_hex


class SDFKnobManager(KnobManager):
    """Knob manager for primitive shapes in signed distance fields."""

    sibling_input_index = 0
    children_input_index = 1

    colour_knob_name = "colour"

    _knob_changed_callbacks = KnobChangedCallbacks(KnobManager._knob_changed_callbacks)

    def __init__(self):
        """Initialize the manager"""
        self._knob_names_only_enabled_if_parent = set()
        super(SDFKnobManager, self).__init__()

    @_knob_changed_callbacks.register(colour_knob_name)
    def _colour_changed(self):
        """Change the node colour to match the object for easier ID."""
        self._node.knob("tile_color").setValue(rgb_to_hex(self._knob.value()))

    @_knob_changed_callbacks.register("inputChange")
    def _input_changed(self):
        """Dynamically enable/disable the is_bound and blend knobs
        depending on whether or not the primitive has children.
        """
        has_child_input = self._node.input(self.children_input_index) is not None
        for knob_name in self._knob_names_only_enabled_if_parent:
            self._node.knob(knob_name).setEnabled(has_child_input)

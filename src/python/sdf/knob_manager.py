"""Knob management for primitive shapes in signed distance fields."""
import nuke

from .utils import KnobChangedCallbacks, KnobManager, rgb_to_hex


class SDFKnobManager(KnobManager):
    """Knob manager for primitive shapes in signed distance fields."""

    sibling_input_index = 0
    children_input_index = 1

    colour_knob_name = "colour"

    _knob_changed_callbacks = KnobChangedCallbacks(KnobManager._knob_changed_callbacks)

    _dimensional_axes = ("x", "y", "z")
    _dimensional_knob_prefix = "dimension_"

    def __init__(self):
        """Initialize the manager"""
        super(SDFKnobManager, self).__init__()

        self._knob_names_only_enabled_if_parent = set()

    @property
    def dimensional_context_knob_names(self):
        """"""
        return [
            self._node.knob(self._dimensional_knob_prefix + axis)
            for axis in self._dimensional_axes
        ]

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

    def _dropdown_context_changed(self, default_dict, context_knobs, set_node_label=False):
        """Dynamically enable/disable and change the labels/tooltips/values
        of the dimensional knobs when the dropdown has changed.

        Args:
            default_dict (dict(str, OrderedDict(str, dict()))): The
                default values for the context sensitive knobs.
        """
        new_value = self._knob.value()

        if set_node_label:
            self._node.knob("label").setValue(new_value)

        default_values = default_dict.get(new_value, {})

        for (knob_name, knob_values), context_knob in zip(
            default_values.items(),
            context_knobs
        ):
            context_knob.setVisible(True)
            context_knob.setLabel(knob_name)
            context_knob.setValue(knob_values["default"])
            context_knob.setTooltip(knob_values["tooltip"])

        for context_knob in context_knobs[len(default_values):]:
            context_knob.setVisible(False)
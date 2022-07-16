# Copyright 2022 by Owen Bulka.
# All rights reserved.
# This file is released under the "MIT License Agreement".
# Please see the LICENSE.md file that should have been included as part
# of this package.
"""Knob management for primitive shapes in signed distance fields."""
import logging

import nuke


_LOGGER = logging.getLogger(__file__)


class KnobChangedCallbacks(dict):
    """Class to register knob callbacks using decorators."""

    def register(self, knob_name):
        """Register the decorated function as a callback of the knob
        given by knob_name.

        Args:
            knob_name (str): The name of the knob this is a callback
                for.
        """
        def decorated(method):
            self[knob_name] = method
            return method
        return decorated

    def register_multiple(self, knob_names):
        """Register the decorated function as a callback of the knobs
        given by knob_names.

        Args:
            knob_name (list(str)): The name of the knobs this is a
                callback for.
        """
        def decorated(method):
            for knob_name in knob_names:
                self[knob_name] = method
            return method
        return decorated


class KnobManager(object):
    """Knob manager for primitive shapes in signed distance fields."""

    _knob_changed_callbacks = KnobChangedCallbacks()

    def __init__(self):
        """Initialize the manager"""
        self._node = nuke.thisNode()
        self._knob = nuke.thisKnob()

    def handle_node_created(self):
        """Setup a newly created node"""
        self._input_changed()

    def handle_knob_changed(self):
        """Handle a knob changed event"""
        knob_name = self._knob.name()
        try:
            type(self)._knob_changed_callbacks.get(knob_name)(self)
        except TypeError:
            _LOGGER.debug("No callbacks for knob: %s", knob_name)

    @_knob_changed_callbacks.register("inputChange")
    def _input_changed(self):
        """Called when the input has changed"""


class SDFGeoKnobManager(KnobManager):
    """Knob manager for primitive shapes in signed distance fields."""

    sibling_input_index = 0
    children_input_index = 1

    _knob_changed_callbacks = KnobChangedCallbacks(KnobManager._knob_changed_callbacks)

    _dimensional_axes = ("x", "y", "z")
    _dimensional_knob_prefix = "dimension_"

    def __init__(self):
        """Initialize the manager"""
        super(SDFGeoKnobManager, self).__init__()

        self._knob_names_only_enabled_if_parent = set()

    @property
    def dimensional_context_knobs(self):
        """list(nuke.Knob): The context knobs for an sdf node's
                dimensional parameters.
        """
        return [
            self._node.knob(self._dimensional_knob_prefix + axis)
            for axis in self._dimensional_axes
        ]

    @property
    def has_children(self):
        """bool: True if a node is connected to the 'children' input."""
        return self._node.input(self.children_input_index) is not None

    @_knob_changed_callbacks.register("inputChange")
    def _input_changed(self):
        """Enable/disable the knobs that only apply if this object has
        children.
        """
        has_child_input = self.has_children
        for knob_name in self._knob_names_only_enabled_if_parent:
            self._node.knob(knob_name).setEnabled(has_child_input)

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
        new_value = self._knob.value()

        if set_node_label:
            self._node.knob("label").setValue(new_value)

        default_values = default_dict.get(new_value, {})

        for (knob_name, knob_values), context_knob in zip(
            default_values.items(),
            context_knobs,
        ):
            print("Visible", context_knob)
            context_knob.setVisible(True)
            context_knob.setLabel(knob_name)
            context_knob.setValue(knob_values["default"])
            context_knob.setTooltip(knob_values["tooltip"])
            if "range" in knob_values:
                context_knob.setRange(*knob_values["range"])

        for context_knob in context_knobs[len(default_values):]:
            print("Not Visible", context_knob)
            context_knob.setVisible(False)

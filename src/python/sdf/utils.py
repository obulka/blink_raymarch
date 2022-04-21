"""General purpose utility functions"""
import logging

import nuke


_LOGGER = logging.getLogger(__file__)


def float_to_8bit_colour(colour_value):
    """Convert a floating point value to 8bit

    Args:
        colour_value (float): The value to convert.

    Returns:
        int: The 8bit colour value.
    """
    return int(max(0, min(round(colour_value * 255.), 255)))


def rgb_to_hex(rgb_value):
    """Convert a floating point rgb value to hex.

    Args:
        rgb_value (list(float)): The value to convert.

    Returns:
        int: The hex colour value.
    """
    return int(
        "0x{0:02x}{1:02x}{2:02x}{3:02x}".format(
            float_to_8bit_colour(rgb_value[0]),
            float_to_8bit_colour(rgb_value[1]),
            float_to_8bit_colour(rgb_value[2]),
            255,
        ),
        0,
    )


class KnobChangedCallbacks(dict):
    def register(self, knob_name):
        def decorated(method):
            self[knob_name] = method
            return method
        return decorated


class KnobManager():
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
        """Dynamically enable/disable the is_bound and blend knobs
        depending on whether or not the primitive has children.
        """
        pass

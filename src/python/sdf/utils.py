"""General purpose utility functions"""


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

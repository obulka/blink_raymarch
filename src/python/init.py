import os


_file_dir = os.path.dirname(__file__)
_parent_dir = os.path.dirname(_file_dir)

# This will overwrite your FN_BLINK_INCLUDE_PATHS if you have one set up
# but I cannot find a delimiter to get nuke reading multiple paths and
# there is only one reference to this in all of google :(
# Either move the blink files to your path location or let me know
# how to fix this.
os.environ["FN_BLINK_INCLUDE_PATHS"] = os.path.join(_parent_dir, "blink", "include")

nuke.pluginAddPath(_file_dir)
nuke.pluginAddPath(os.path.join(_parent_dir, "gizmos"))

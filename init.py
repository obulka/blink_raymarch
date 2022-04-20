import os


_file_dir = os.path.dirname(__file__)

os.environ["FN_BLINK_INCLUDE_PATHS"] = os.path.join(_file_dir, "src", "blink")

nuke.pluginAddPath(os.path.join(_file_dir, "src", "python"))
nuke.pluginAddPath(os.path.join(_file_dir, "gizmos"))
nuke.pluginAddPath(os.path.join(_file_dir, "plugins"))

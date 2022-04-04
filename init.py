import os


os.environ["FN_BLINK_INCLUDE_PATHS"] = os.path.join(
    os.path.dirname(__file__),
    "src/blink",
)

nuke.pluginAddPath("./gizmos")
nuke.pluginAddPath("./python")
nuke.pluginAddPath("./plugins")

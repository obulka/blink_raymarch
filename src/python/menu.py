""""""


_toolbar = nuke.toolbar("Nodes")

_sdf_menu = _toolbar.addMenu("SDF")
_sdf_menu.addCommand("sdf_primitive", "nuke.createNode('sdf_primitive')")
_sdf_menu.addCommand("sdf_light", "nuke.createNode('sdf_light')")
_sdf_menu.addCommand("sdf_material", "nuke.createNode('sdf_material')")
_sdf_menu.addCommand("sdf_noise", "nuke.createNode('sdf_noise')")
_sdf_menu.addCommand("path_march", "nuke.createNode('path_march')")
_sdf_menu.addCommand("ray_march", "nuke.createNode('ray_march')")

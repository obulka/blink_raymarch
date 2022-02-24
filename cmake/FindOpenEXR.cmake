find_path(OpenEXR_INCLUDE_DIR OpenEXR/OpenEXRConfig.h)

# We expect these to be found in the Nuke directory as well
find_library(OpenEXR_HALF NAMES Half)
find_library(OpenEXR_IEX NAMES Iex)
find_library(OpenEXR_IEXMATH NAMES IexMath)


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenEXR
  FOUND_VAR OpenEXR_FOUND
  REQUIRED_VARS
    OpenEXR_INCLUDE_DIR
    OpenEXR_HALF
    OpenEXR_IEX
    OpenEXR_IEXMATH
)

add_library(OpenEXR::OpenEXR INTERFACE IMPORTED)
set_target_properties(OpenEXR::OpenEXR
    PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${OpenEXR_INCLUDE_DIR};${OpenEXR_INCLUDE_DIR}/OpenEXR"
        INTERFACE_LINK_LIBRARIES "${OpenEXR_HALF};${OpenEXR_IEX};${OpenEXR_IEXMATH}"
)

mark_as_advanced(
    OpenEXR_INCLUDE_DIR
    OpenEXR_HALF
    OpenEXR_IEX
    OpenEXR_IEXMATH
)
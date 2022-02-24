cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(raymarch)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
set(OpenGL_GL_PREFERENCE GLVND)

find_package(Nuke REQUIRED)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)


function(add_nuke_plugin PLUGIN_NAME)
    add_library(${PLUGIN_NAME} MODULE ${ARGN})
    add_library(NukePlugins::${PLUGIN_NAME} ALIAS ${PLUGIN_NAME})
    target_link_libraries(${PLUGIN_NAME} PRIVATE Nuke::NDK)
    target_compile_definitions(${PLUGIN_NAME} PRIVATE FN_EXAMPLE_PLUGIN)
    set_target_properties(${PLUGIN_NAME} PROPERTIES PREFIX "")
    if (APPLE)
        set_target_properties(${PLUGIN_NAME} PROPERTIES SUFFIX ".dylib")
    endif()
    if (DEFINED ENV{NUKE_PLUGIN_PATH})
        install(TARGETS ${PLUGIN_NAME} DESTINATION $ENV{NUKE_PLUGIN_PATH})
    endif()
endfunction()


add_nuke_plugin(GPUNode src/cpp/gpu_node/GPUNode.cpp)



find_package(OpenGL)
if (OpenGL_FOUND)
#     add_nuke_plugin(Handle Handle.cpp)
#     target_link_libraries(Handle PRIVATE OpenGL::GL)

#     add_nuke_plugin(Draw2D Draw2D.cpp)
#     target_link_libraries(Draw2D PRIVATE OpenGL::GL)

#     add_nuke_plugin(Draw3D Draw3D.cpp)
#     target_link_libraries(Draw3D PRIVATE OpenGL::GL)

#     add_nuke_plugin(SimpleAxis SimpleAxis.cpp)
#     target_link_libraries(SimpleAxis PRIVATE OpenGL::GL)
#     if (APPLE)
#         target_compile_definitions(SimpleAxis PRIVATE GL_SILENCE_DEPRECATION)
#     endif()

#     add_nuke_plugin(Phong Phong.cpp)
#     target_link_libraries(Phong PRIVATE OpenGL::GL)
else()
    message(STATUS "Couldn't find OpenGL dev libraries, skipping Handle, Draw2D, Draw3D, SimpleAxis, and Phong plugin examples")
endif()


find_package(FFmpeg COMPONENTS avcodec avformat swresample swscale)
if (FFMPEG_FOUND)
#     add_nuke_plugin(movReader
#         mov/codecWhitelist.cpp
#         mov/movReader.cpp
#     )
#     target_link_libraries(movReader PRIVATE FFmpeg::avutil FFmpeg::avcodec FFmpeg::avformat FFmpeg::swresample FFmpeg::swscale)

#     add_nuke_plugin(movWriter
#         mov/codecWhitelist.cpp
#         mov/movWriter.cpp
#     )
#     target_link_libraries(movWriter PRIVATE FFmpeg::avutil FFmpeg::avcodec FFmpeg::avformat FFmpeg::swresample FFmpeg::swscale)
else()
    message(STATUS "FFmpeg libraries weren't found, skipping mov64Reader and mov64 Writer plugins")
endif()


find_package(OpenEXR MODULE)
if (OpenEXR_FOUND)
#     add_library(ExrCommon OBJECT ExrChannelNameToNuke.cpp)
#     set_target_properties(ExrCommon PROPERTIES POSITION_INDEPENDENT_CODE ON)
#     target_link_libraries(ExrCommon
#         PRIVATE Nuke::NDK
#         PUBLIC OpenEXR::IlmImf
#     )

#     add_nuke_plugin(exrReader exrReader.cpp)
#     target_link_libraries(exrReader PRIVATE ExrCommon)

#     add_nuke_plugin(exrReaderDeep exrReaderDeep.cpp)
#     target_link_libraries(exrReaderDeep PRIVATE ExrCommon)

#     add_nuke_plugin(exrWriter exrWriter.cpp)
#     target_link_libraries(exrWriter PRIVATE ExrCommon)

#     add_nuke_plugin(exrWriterDeep exrWriterDeep.cpp)
#     target_link_libraries(exrWriterDeep PRIVATE ExrCommon)
else()
    message(STATUS "OpenEXR libraries were not found, skipping exrReader/Writer plugins")
endif()


find_package(Qt5 COMPONENTS Core Gui Widgets)
if (Qt5_FOUND)
#     add_nuke_plugin(AddCustomQt AddCustomQt.cpp)
#     qt_wrap_cpp(AddCustomQt MOCSOURCES AddCustomQt.moc.h)
#     target_sources(AddCustomQt PRIVATE ${MOCSOURCES})
#     target_link_libraries(AddCustomQt PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)

#     add_nuke_plugin(Cryptomatte
#         CryptomatteManifest.cpp
#         CryptomatteMatteExpression.cpp
#         CryptomatteMatteList.cpp
#         CryptomattePlugin.cpp
#         CryptomatteUtils.cpp)
#     target_link_libraries(Cryptomatte PRIVATE Qt5::Core)
else()
    message(STATUS "Qt5 libraries were not found, skipping the Qt examples")
endif()

cmake_minimum_required(VERSION 3.14)
project(ModernCppAppTemplate VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build settings
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")

# --- Dependencies ---
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_library(COCOA_LIB Cocoa)
find_library(IOKIT_LIB IOKit)
find_library(COREVIDEO_LIB CoreVideo)

# --- ImGui Library ---
set(IMGUI_DIR "external/imgui")
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
)

add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC 
    ${IMGUI_DIR} 
    ${IMGUI_DIR}/backends
    ${IMGUI_DIR}/misc/cpp
)
target_link_libraries(imgui PUBLIC glfw)

# --- Framework Library (CRITICAL - enforces separation) ---
add_library(modern_framework STATIC
    # Core
    framework/core/IClientApp.h
    
    # GUI
    framework/gui/Theme.cpp
    framework/gui/WindowManager.cpp
    framework/gui/GuiUtils.cpp
    
    # I/O
    framework/io/MemoryMappedFile.cpp
    framework/io/FileWriter.cpp
    
    # Utils
    framework/utils/ConfigManager.cpp
    framework/utils/ResourceManager.cpp
    
    # Main template
    framework/main_template.cpp
)

target_include_directories(modern_framework PUBLIC 
    ${CMAKE_SOURCE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(modern_framework PUBLIC 
    imgui 
    glfw 
    OpenGL::GL
    ${COCOA_LIB} 
    ${IOKIT_LIB} 
    ${COREVIDEO_LIB}
)

# --- Framework Examples ---

# 1. Simple Viewer (Proof of Concept)
add_executable(simple_viewer MACOSX_BUNDLE
    examples/simple_viewer/SimpleViewerApp.cpp
    examples/simple_viewer/main.cpp
)

target_link_libraries(simple_viewer PRIVATE modern_framework)

# Proper Resource Bundling for macOS
set(SIMPLE_VIEWER_RESOURCES
    resources/AppIcon.icns
    resources/background_calm.png
    resources/fa-solid-900.ttf
)

set_target_properties(simple_viewer PROPERTIES
    MACOSX_BUNDLE_ICON_FILE "AppIcon.icns"
    RESOURCE "${SIMPLE_VIEWER_RESOURCES}"
)

# Copy resources to build directory for non-bundle development
foreach(RES_FILE ${SIMPLE_VIEWER_RESOURCES})
    get_filename_component(RES_NAME ${RES_FILE} NAME)
    add_custom_command(TARGET simple_viewer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/${RES_FILE}
        $<TARGET_FILE_DIR:simple_viewer>/${RES_NAME}
        COMMENT "Copying ${RES_NAME} to output directory"
    )
endforeach()

# --- Framework Tests ---
add_executable(framework_tests
    tests/framework/test_theme.cpp
    tests/framework/test_resource_manager.cpp
    external/catch2/catch_amalgamated.cpp
)
target_include_directories(framework_tests PRIVATE external/catch2)
target_link_libraries(framework_tests PRIVATE modern_framework)

# --- Note on Legacy Code ---
# De originele Log Analyzer code is verplaatst naar voor archivering naar:
# examples/log_analyzer/
# Deze is momenteel niet geconfigureerd in deze root CMakeLists.txt om 
# de template neutraal en overzichtelijk te houden.

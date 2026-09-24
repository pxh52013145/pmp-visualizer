cmake_minimum_required(VERSION 3.25)

project(pmp-visualizer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(PMP_RENDER_BACKEND "auto" CACHE STRING "Renderer backend: auto, native-d3d12, native-vulkan, or bgfx")
set_property(CACHE PMP_RENDER_BACKEND PROPERTY STRINGS auto native-d3d12 native-vulkan bgfx)
option(PMP_ENABLE_WARNINGS "Enable strict compiler warnings" ON)

find_package(Qt6 6.6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()

qt_add_executable(pmp-visualizer
    src/main.cpp
    src/app/MainWindow.cpp
    src/app/MainWindow.hpp
    src/core/VisualizerModel.cpp
    src/core/VisualizerModel.hpp
    src/ipc/SpectrumProtocol.cpp
    src/ipc/SpectrumProtocol.hpp
    src/render/Renderer.hpp
    src/render/Renderer.cpp
    src/ui/StudioTheme.hpp
    src/ui/ViewportWidget.cpp
    src/ui/ViewportWidget.hpp
    src/ui/OutlinerWidget.cpp
    src/ui/OutlinerWidget.hpp
    src/ui/InspectorWidget.cpp
    src/ui/InspectorWidget.hpp
    src/ui/TimelineWidget.cpp
    src/ui/TimelineWidget.hpp
)

target_include_directories(pmp-visualizer PRIVATE src)
target_compile_definitions(pmp-visualizer PRIVATE
    PMP_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    PMP_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    PMP_VERSION_PATCH=${PROJECT_VERSION_PATCH}
    PMP_RENDER_BACKEND_NAME="${PMP_RENDER_BACKEND}"
)
target_link_libraries(pmp-visualizer PRIVATE Qt6::Widgets)

if(PMP_ENABLE_WARNINGS)
    if(MSVC)
        target_compile_options(pmp-visualizer PRIVATE /W4 /permissive-)
    else()
        target_compile_options(pmp-visualizer PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

if(WIN32)
    target_link_libraries(pmp-visualizer PRIVATE d3d12 dxgi dxguid)
endif()

if(PMP_RENDER_BACKEND STREQUAL "bgfx")
    message(FATAL_ERROR "PMP_RENDER_BACKEND=bgfx is reserved until dependency packaging is finalized.")
endif()

if(PMP_RENDER_BACKEND STREQUAL "native-vulkan")
    find_package(Vulkan REQUIRED)
    target_link_libraries(pmp-visualizer PRIVATE Vulkan::Vulkan)
endif()

set_target_properties(pmp-visualizer PROPERTIES WIN32_EXECUTABLE TRUE OUTPUT_NAME "pmp-visualizer")

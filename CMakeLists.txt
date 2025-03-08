cmake_minimum_required(VERSION 3.30)
project(TrinVK)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    set(PLATFORM_WINDOWS TRUE)
elseif(APPLE)
    set(PLATFORM_MACOS TRUE)
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_LINUX TRUE)
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

if(PLATFORM_WINDOWS)
    add_definitions(-DPLATFORM_WINDOWS)
    add_definitions(-DNOMINMAX -D_USE_MATH_DEFINES)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
elseif(PLATFORM_MACOS)
    add_definitions(-DPLATFORM_MACOS)
elseif(PLATFORM_LINUX)
    add_definitions(-DPLATFORM_LINUX)
endif()

find_package(Vulkan REQUIRED)
if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Vulkan not found")
endif()

if(PLATFORM_MACOS)
    find_library(MOLTEN_VK_LIBRARY MoltenVK)
    if(NOT MOLTEN_VK_LIBRARY)
        message(WARNING "MoltenVK library not found. Vulkan support on macOS requires MoltenVK.")
    endif()

    # macOS frameworks
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(METAL_LIBRARY Metal REQUIRED)
    find_library(METALKIT_LIBRARY MetalKit REQUIRED)
    find_library(QUARTZ_CORE_LIBRARY QuartzCore REQUIRED)

    set(MACOS_LIBRARIES
            ${COCOA_LIBRARY}
            ${METAL_LIBRARY}
            ${METALKIT_LIBRARY}
            ${QUARTZ_CORE_LIBRARY}
            ${MOLTEN_VK_LIBRARY}
    )

    # Autocompletion
    execute_process(
            COMMAND xcrun --show-sdk-path
            OUTPUT_VARIABLE MACOS_SDK_PATH
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_OSX_SYSROOT ${MACOS_SDK_PATH})
elseif (PLATFORM_LINUX)
    find_package(X11 REQUIRED)
    find_package(Threads REQUIRED)
    find_package(PkgConfig REQUIRED)

    # Check for Wayland support
    pkg_check_modules(WAYLAND wayland-client wayland-cursor wayland-egl)
    if(WAYLAND_FOUND)
        add_definitions(-DHAS_WAYLAND)
    endif()

    # Check for XCB support
    pkg_check_modules(XCB xcb)
    if(XCB_FOUND)
        add_definitions(-DHAS_XCB)
    endif()

    set(LINUX_LIBRARIES
            ${X11_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT}
    )

    if(WAYLAND_FOUND)
        list(APPEND LINUX_LIBRARIES ${WAYLAND_LIBRARIES})
    endif()

    if(XCB_FOUND)
        list(APPEND LINUX_LIBRARIES ${XCB_LIBRARIES})
    endif()
endif ()

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${Vulkan_INCLUDE_DIRS}
)

# The source code to tbe compiled with the application
# MACOSX_BUNDLE helps create an application for Mac devices, and only compiles for MacOS
# TODO : Create subdirectories to sort code
add_executable(TrinVK MACOSX_BUNDLE
        main.cpp
        Engine/Source/Helpers/Console.h
        Engine/Source/Runtime/Core/Engine.cpp
        Engine/Source/Runtime/Core/Engine.h
        Engine/Source/Platform/Macos/Macos.h
        Engine/Source/Platform/Windows/Windows.h
        Engine/Source/Platform/Linux/Linux.h
        Engine/Source/Platform/IWindow.h
        Engine/Source/Platform/Windows/Win32Window.cpp
        Engine/Source/Platform/Windows/Win32Window.h
        Engine/Source/Platform/Macos/MacWindow.mm
        Engine/Source/Platform/Macos/MacWindow.h
)

# Provide fast access to some folders for includes
target_include_directories(TrinVK PRIVATE
        Engine # Main Folder
        Engine/Source/Platform # Platform Code
        Engine/Source/Runtime # Runtime Code
        Engine/Source/Helpers # Helper code
)

target_link_libraries(TrinVK PRIVATE ${Vulkan_LIBRARIES})

# TODO : Add executable icons to platform specific linking

# Platform-specific linking
if(PLATFORM_WINDOWS)
    # Link our Windows libraries
    target_link_libraries(TrinVK PRIVATE winmm)
elseif(PLATFORM_MACOS)
    # Link our MacOS libraries
    target_link_libraries(TrinVK PRIVATE ${MACOS_LIBRARIES})

    # Make sure that the application isn't just binary
    set_target_properties(${PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE_GUI_IDENTIFIER "TrinVK.macos"
            MACOSX_BUNDLE_BUNDLE_NAME "TrinVK"
            MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
    )
elseif(PLATFORM_LINUX)
    # Link our Linux libraries
    target_link_libraries(TrinVK PRIVATE ${LINUX_LIBRARIES})
endif()
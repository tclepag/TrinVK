cmake_minimum_required(VERSION 3.30)
project(TrinVK)

# TrinVK Properties
set(CMAKE_CXX_STANDARD 20)
set(TRIN_PROJECT_DIR Engine)
set(TRIN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/TrinityVulkan)

# TrinVK Platforms

## See Engine/CMakeLists.txt

# TrinVK Packages

## Get Vulkan Graphics/Tools
### Check if the PC environment exists
if (NOT DEFINED ENV{VULKAN_SDK})
    message(WARNING "VULKAN_SDK ENV VARIABLE NOT SET, VULKAN WILL NOT BE FOUND")
else ()
    set(VULKAN_SDK $ENV{VULKAN_SDK})
endif ()


### Get the Vulkan Graphics API
find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    message(STATUS "FOUND VULKAN: ${Vulkan_VERSION}")
    message(STATUS "VULKAN LIBRARIES: ${Vulkan_LIBRARIES}")
    message(STATUS "VULKAN INCLUDE DIR: ${Vulkan_INCLUDE_DIRS}")
    set(VMA_INCLUDE_DIR "${VULKAN_SDK}/include/vma")
    if(NOT EXISTS "${VMA_INCLUDE_DIR}/vk_mem_alloc.h")
        message(FATAL_ERROR "VMA not found at ${VMA_INCLUDE_DIR}. You may need to set VMA_INCLUDE_DIR manually.")
    endif()
else ()
    message(FATAL_ERROR
            "VULKAN IS REQUIRED FOR TRINVK TO BUILD AND RUN, VULKAN CAN BE FOUND AT: https://vulkan.lunarg.com/sdk/home"
    )
endif ()

## Get GLFW
find_package(glfw3 CONFIG REQUIRED)

# TrinVK Build
## Check to see if our current build profile is called Release
### Debug shouldn't be copying over code to the output
## Get all files recursively
file(GLOB_RECURSE ENGINE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Engine/*")

## Make sure our foreach loop will ignore these files

### Do not include any CMakeLists files
list(FILTER ENGINE_FILES EXCLUDE REGEX ".*CMakeLists\\.txt$")
### Do not include the entry point
list(FILTER ENGINE_FILES EXCLUDE REGEX ".*main\\.cpp$")
### Ignore all source code, giving developers an idea of what the code's interface is would cut down on size of file
list(FILTER ENGINE_FILES EXCLUDE REGEX ".*\\.cpp$")

message(STATUS "Number of files to copy: ${ENGINE_FILES}")

## Loop through the entire structure to copy to our output
### Allows developers to modify some parts of the engine
foreach(FILE ${ENGINE_FILES})
    file(RELATIVE_PATH REL_FILE "${CMAKE_SOURCE_DIR}/Engine" ${FILE})
    get_filename_component(DIR ${REL_FILE} DIRECTORY)
    message(STATUS "Copying ${FILE} to ${TRIN_OUTPUT_DIR}/${DIR}")
    file(MAKE_DIRECTORY ${TRIN_OUTPUT_DIR}/${DIR})
    file(COPY ${FILE} DESTINATION ${TRIN_OUTPUT_DIR}/${DIR})
endforeach()

## Set binary outputs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${TRIN_OUTPUT_DIR}/Binary)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${TRIN_OUTPUT_DIR}/Binary)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${TRIN_OUTPUT_DIR}/Library)

# TrinVK Executable
add_subdirectory(Engine)
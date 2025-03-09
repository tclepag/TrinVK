set(RUNTIME_SOURCES
        Rendering/Vulkan/GraphicsPipeline.cpp
        Rendering/Vulkan/GraphicsPipeline.h
        Core/Engine.cpp
        Core/Engine.h
        Core/Window.cpp
        Core/Window.h
        Core/VulkanContext.cpp
        Core/VulkanContext.h
        Rendering/Vulkan/Viewport.cpp
        Rendering/Vulkan/Viewport.h
)

add_library(Trin_Runtime ${RUNTIME_SOURCES})
target_include_directories(Trin_Runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
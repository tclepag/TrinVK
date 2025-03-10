set(RUNTIME_SOURCES
        Core/Engine.cpp
        Core/Engine.h
        Core/VulkanCore.cpp
        Core/VulkanCore.h
        Core/Window.cpp
        Core/Window.h
        Rendering/VulkanRenderer.cpp
        Rendering/VulkanRenderer.h
        Rendering/Commands/VulkanCMDS.cpp
        Rendering/Commands/VulkanCMDS.h
)

add_library(Trin_Runtime ${RUNTIME_SOURCES})
target_include_directories(Trin_Runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
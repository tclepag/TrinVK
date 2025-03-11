set(RUNTIME_SOURCES
        Core/Engine.cpp
        Core/Engine.h
        Core/Window.cpp
        Core/Window.h
        Core/VulkanContext.cpp
        Core/VulkanContext.h
        Rendering/VulkanRenderer.cpp
        Rendering/VulkanRenderer.h
        Graphics/VulkanProgram.cpp
        Graphics/VulkanProgram.h
        Graphics/VulkanShader.cpp
        Graphics/VulkanShader.h
        Core/VulkanCommandBuffer.cpp
        Core/VulkanCommandBuffer.h
        Rendering/RenderTargets/VulkanRenderTarget.cpp
        Rendering/RenderTargets/VulkanRenderTarget.h
        Rendering/RenderTargets/VulkanScreenRenderTarget.cpp
        Rendering/RenderTargets/VulkanScreenRenderTarget.h
)

add_library(Trin_Runtime ${RUNTIME_SOURCES})
target_include_directories(Trin_Runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
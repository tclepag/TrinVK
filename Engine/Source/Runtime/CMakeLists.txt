set(RUNTIME_SOURCES
        Core/Engine.cpp
        Core/Engine.h
        Core/Window.cpp
        Core/Window.h
        Core/VulkanContext.cpp
        Core/VulkanContext.h
)

add_library(Trin_Runtime ${RUNTIME_SOURCES})
target_include_directories(Trin_Runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
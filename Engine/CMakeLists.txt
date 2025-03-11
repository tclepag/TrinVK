add_executable(TrinVK
        main.cpp
        Source/Helpers/File.h
)

set(HELPERS
        Source/Helpers/Console.h
        Source/Helpers/System.h
        Source/Helpers/Types.h
        Source/Helpers/File.h
)

set(MATH
        Source/Math/Vector2.h
        Source/Math/Vector3.h
        Source/Math/Vector4.h
)

target_compile_definitions(TrinVK PRIVATE
        GLFW_INCLUDE_VULKAN
)

# Target Trin_Source
add_subdirectory(Source/Runtime)

target_include_directories(Trin_Runtime PUBLIC
        ${Vulkan_INCLUDE_DIRS}
        ${VMA_INCLUDE_DIR}
        Source
        ${HELPERS}
        ${MATH}
)
target_link_libraries(Trin_Runtime PUBLIC
        ${Vulkan_LIBRARIES}
        glfw
)

# Link TrinVK Libraries
target_link_libraries(TrinVK PRIVATE
        Trin_Runtime
)
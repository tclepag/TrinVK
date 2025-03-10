set(HELPERS
        Source/Helpers/Console.h
        Source/Helpers/System.h
        Source/Helpers/Types.h
)

set(MATH
)

add_executable(TrinVK
        main.cpp
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
        SDL3::SDL3
)

# Link TrinVK Libraries
target_link_libraries(TrinVK PRIVATE
        Trin_Runtime
)
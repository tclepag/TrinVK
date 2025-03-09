set(HELPERS
        Source/Helpers/Console.h
        Source/Helpers/System.h
        Source/Helpers/System.cpp
)

add_executable(TrinVK
        main.cpp
        resource.rc
        ${HELPERS}
)

# Setting TrinVK application settings
set_target_properties(TrinVK PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${TRIN_OUTPUT_DIR}
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
)

# Is Microsoft Visual Studio being used to compile?
if(MSVC)
    # Unicode support
    add_compile_definitions(UNICODE _UNICODE)

    # Enable multi-processor compilation
    target_compile_options(TrinVK PRIVATE /MP)

    # Specify C++ standard
    set_property(TARGET TrinVK PROPERTY CXX_STANDARD 17)
endif()

# Target Trin_Source
add_subdirectory(Source/Runtime)

target_include_directories(Trin_Runtime PUBLIC
        ${Vulkan_INCLUDE_DIRS}
        ${VMA_INCLUDE_DIR}
        Source
)
target_link_libraries(Trin_Runtime PUBLIC ${Vulkan_LIBRARIES})

if(WIN32)
    target_compile_definitions(Trin_Runtime PRIVATE VK_USE_PLATFORM_WIN32_KHR)
endif()

# Link TrinVK Libraries
target_link_libraries(TrinVK PRIVATE
        Trin_Runtime
        winmm.lib
        Shlwapi.lib
)
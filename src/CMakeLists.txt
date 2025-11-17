add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(input)

add_subdirectory(ui)
add_subdirectory(audio)

add_executable(bilsim main.cpp)
target_include_directories(bilsim PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Ensure imgui is built before bilsim
if(TARGET imgui)
    add_dependencies(bilsim imgui)
endif()

# Link all required libraries
# On Windows, we need to explicitly link imgui because MSVC/MinGW don't
# automatically resolve symbols from transitive static library dependencies
# Link imgui BEFORE ui to ensure proper symbol resolution
target_link_libraries(bilsim PRIVATE
    imgui  # Link imgui FIRST - critical for MSVC symbol resolution
    core
    graphics
    input
    ui
    audio
    threepp::threepp
)

# On MSVC, use /WHOLEARCHIVE to ensure ALL symbols from imgui are included
# This forces the linker to pull in all object files from imgui.lib, not just
# the ones directly referenced by bilsim.exe
if(MSVC)
    target_link_options(bilsim PRIVATE
        "/WHOLEARCHIVE:$<TARGET_FILE:imgui>"
    )
endif()


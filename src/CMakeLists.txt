add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(input)

add_subdirectory(ui)
add_subdirectory(audio)

add_executable(bilsim main.cpp)
target_include_directories(bilsim PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link all required libraries
# Note: ui library now contains ImGui implementation compiled directly into it
target_link_libraries(bilsim PRIVATE
    core
    graphics
    input
    ui
    audio
    threepp::threepp
)



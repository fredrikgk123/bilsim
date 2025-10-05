add_subdirectory(core)
add_subdirectory(graphics)
add_subdirectory(input)
add_subdirectory(ui)
add_subdirectory(audio)

add_executable(bilsim main.cpp)
target_link_libraries(bilsim PUBLIC
        core
        graphics
        input
        ui
        audio
)

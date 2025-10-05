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

# Copy audio file to build directory automatically
add_custom_command(TARGET bilsim POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/src/audio/carnoise.wav
        $<TARGET_FILE_DIR:bilsim>/carnoise.wav
    COMMENT "Copying audio file to build directory"
)

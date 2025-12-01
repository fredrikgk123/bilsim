# Test executable - mark as ALL to ensure it's built by default
add_executable(run_tests
    test_vehicle.cpp
    test_gameObject.cpp
    test_collision.cpp
    test_validation.cpp
)

# Ensure test executable is built as part of the default ALL target
set_target_properties(run_tests PROPERTIES
    EXCLUDE_FROM_ALL FALSE
)

# Add include directories
target_include_directories(run_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Link against core library, audio library, Catch2, and threading library
target_link_libraries(run_tests PRIVATE
    core
    audio
    Catch2::Catch2WithMain
    Threads::Threads
)

# Ensure assets are copied before running tests
add_dependencies(run_tests copy_assets)

# For multi-config generators, copy assets to the test executable directory
add_custom_command(TARGET run_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:run_tests>/assets
    COMMENT "Copying assets to test output directory"
    VERBATIM
)

# Enable testing
enable_testing()

# Add test - Use $<TARGET_FILE:run_tests> to get the full path to the executable
# Set working directory to where the executable is located (where assets are copied)
add_test(NAME AllTests
    COMMAND $<TARGET_FILE:run_tests>
)

# Set the working directory to the directory containing the test executable
# This ensures the test can find assets in the relative "assets/" subdirectory
# For multi-config generators (Visual Studio), this will be build/tests/Release or build/tests/Debug
# For single-config generators (Ninja, Makefiles), this will be build/tests
set_tests_properties(AllTests PROPERTIES
    WORKING_DIRECTORY $<TARGET_FILE_DIR:run_tests>
)

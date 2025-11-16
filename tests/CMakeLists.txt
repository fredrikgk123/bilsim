# Test executable
add_executable(run_tests
    test_vehicle.cpp
    test_gameObject.cpp
    test_collision.cpp
    test_validation.cpp
)

# Add include directories
target_include_directories(run_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# Link against core library, audio library, and Catch2
target_link_libraries(run_tests PRIVATE
    core
    audio
    Catch2::Catch2WithMain
)

# Ensure assets are copied before running tests
add_dependencies(run_tests copy_assets)

# Enable testing
enable_testing()

# Use generator expression to handle multi-config generators (Visual Studio, Xcode)
# This ensures CTest can find the executable in Release/, Debug/, etc. subdirectories
# Set working directory to the build directory so assets can be found
add_test(NAME AllTests COMMAND $<TARGET_FILE:run_tests>)
set_tests_properties(AllTests PROPERTIES
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)


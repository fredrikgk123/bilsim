# Test executable
add_executable(run_tests
    test_vehicle.cpp
    test_gameObject.cpp
    test_collision.cpp
    test_validation.cpp
)

# Link against core library, audio library, and Catch2
target_link_libraries(run_tests PRIVATE
    core
    audio
    Catch2::Catch2WithMain
)

# Enable testing
enable_testing()
add_test(NAME AllTests COMMAND run_tests)


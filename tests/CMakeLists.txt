# Test executable
add_executable(run_tests
    test_vehicle.cpp
    test_gameObject.cpp
    test_collision.cpp
    test_validation.cpp
)

# Link against core library and Catch2
target_link_libraries(run_tests PRIVATE
    core
    Catch2::Catch2WithMain
)

# Discover and register tests with CTest
include(CTest)
include(Catch)
catch_discover_tests(run_tests)


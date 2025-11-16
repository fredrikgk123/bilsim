# Create a library for input handling components
add_library(input
    input_handler.cpp
)

# Make headers available to other directories
target_include_directories(input PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Link against the core library for Vehicle class access
target_link_libraries(input PUBLIC
    core
    threepp::threepp
)

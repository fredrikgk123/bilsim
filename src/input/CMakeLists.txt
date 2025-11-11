# Create a library for input handling components
add_library(input
    input_handler.cpp
    input_handler.hpp
)

# Make headers available to other directories
target_include_directories(input PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/core/interfaces  # Add interfaces directory
)

# Link against the core library for Vehicle class access
target_link_libraries(input PUBLIC
    core
    threepp::threepp
)

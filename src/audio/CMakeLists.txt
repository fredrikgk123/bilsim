# Create a library for audio components
add_library(audio
    audioManager.cpp
    audioManager.hpp
)

# Make headers available to other directories
target_include_directories(audio PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link against the core library for Vehicle class access
target_link_libraries(audio PUBLIC
    core
)

# Add miniaudio include directory
target_include_directories(audio PRIVATE
    ${miniaudio_SOURCE_DIR}
)


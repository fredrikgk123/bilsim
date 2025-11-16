# Create a library for audio components
add_library(audio
    audio_manager.cpp
)

# Make headers available to other directories
target_include_directories(audio PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Link against the core library for Vehicle class access
target_link_libraries(audio PUBLIC
    core
)

# Add miniaudio include directory
target_include_directories(audio PRIVATE
    ${miniaudio_SOURCE_DIR}
)

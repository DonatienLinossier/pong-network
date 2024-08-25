cmake_minimum_required(VERSION 3.28)
project(pong)

set(CMAKE_CXX_STANDARD 17)

# Set the path to the SFML installation
set(SFML_DIR "C:/Users/donat/OneDrive/Documents/Programmation/C++/pong/lib/SFML-2.6.1/lib/cmake/SFML")

# Find SFML package (make sure to use the correct components)
find_package(SFML 2.6 COMPONENTS system window graphics network audio REQUIRED)

# Add the executable
add_executable(pong src/main.cpp
        src/main.cpp
        src/ball.cpp
        include/ball.h
        src/Paddle.cpp
        include/Paddle.h
)

# Link SFML libraries to the 'pong' target
target_link_libraries(pong sfml-system sfml-window sfml-graphics sfml-audio sfml-network)

# Copy necessary DLLs to the output directory after build (Windows only)
if (WIN32)
    add_custom_command(TARGET pong POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_DIR}/../../../bin/sfml-system-2.dll"
            "${SFML_DIR}/../../../bin/sfml-window-2.dll"
            "${SFML_DIR}/../../../bin/sfml-graphics-2.dll"
            "${SFML_DIR}/../../../bin/sfml-audio-2.dll"
            "${SFML_DIR}/../../../bin/sfml-network-2.dll"
            $<TARGET_FILE_DIR:pong>)
endif()
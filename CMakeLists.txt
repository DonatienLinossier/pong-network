cmake_minimum_required(VERSION 3.28)
project(pong)

set(CMAKE_CXX_STANDARD 17)

# Set the path to the SFML installation
set(SFML_DIR "C:/Users/donat/OneDrive/Documents/Programmation/C++/pong/lib/SFML-2.6.1/lib/cmake/SFML")

# Find SFML package (make sure to use the correct components)
find_package(SFML 2.6 COMPONENTS system window graphics network audio REQUIRED)
# Add the executable
add_executable(pong_room src/pong_room.cpp
        include/const.h
        src/ball.cpp
        include/ball.h
        src/Paddle.cpp
        include/Paddle.h
        src/GameRoom.cpp
        include/GameRoom.h
        include/drawable.h
        include/PongGame.cpp
        include/PongGame.h
        src/Drawable.cpp
)

add_executable(pong_client src/pong_client.cpp
        include/const.h
        src/ball.cpp
        include/ball.h
        src/Paddle.cpp
        include/Paddle.h
        include/drawable.h
        include/PongGame.cpp
        include/PongGame.h
        src/Drawable.cpp
)

add_executable(main_serv src/main_server.cpp
        include/const.h
        include/const.h
        src/GameRoom.cpp
        include/GameRoom.h
        src/Drawable.cpp
        src/Drawable.cpp
)

# Link SFML libraries to the 'pong' target
target_link_libraries(pong_room sfml-system sfml-window sfml-graphics sfml-audio sfml-network Ws2_32)
target_link_libraries(pong_client sfml-system sfml-window sfml-graphics sfml-audio sfml-network Ws2_32)
target_link_libraries(main_serv Ws2_32 sfml-window)

# Copy necessary DLLs to the output directory after build (Windows only)
if (WIN32)
    add_custom_command(TARGET pong_room POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SFML_DIR}/../../../bin/sfml-system-2.dll"
            "${SFML_DIR}/../../../bin/sfml-window-2.dll"
            "${SFML_DIR}/../../../bin/sfml-graphics-2.dll"
            "${SFML_DIR}/../../../bin/sfml-audio-2.dll"
            "${SFML_DIR}/../../../bin/sfml-network-2.dll"
            $<TARGET_FILE_DIR:pong_room>)
endif()
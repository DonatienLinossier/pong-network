//
// Created by donat on 25/08/2024.
//

#include <SFML/Graphics.hpp>
#include "../include/Ball.h"
#include "../include/Paddle.h"
#include "../include/const.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#define WIDTH 800
#define HEIGHT 600

int main() {

    WSADATA wsaData;
    int result;

    // Step 1: Initialize Winsock
    result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    // Step 2: Create a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Step 3: Connect to the server
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(MAIN_SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // Connect to localhost

    result = connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR) {
        std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server!" << std::endl;

    // Step 4: Send and receive data
    const char* initialMessage = (std::to_string(GAME_ROOM_REQUEST_ROOM_STARTED)).c_str();
    send(clientSocket, initialMessage, strlen(initialMessage), 0);

    closesocket(clientSocket);
    WSACleanup();
    // Create a window with a size of 800x600 pixels and the title "SFML Window"


    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML Window");

    // Create a circle shape with a radius of 50 pixels
    sf::CircleShape shape(50);
    shape.setFillColor(sf::Color::Green);  // Set the color of the shape to green


    Paddle paddle_1(10, 100, 50, 100, sf::Color::White);
    Paddle paddle_2(10, 100, WIDTH - 50,  100, sf::Color::White);


    Paddle* paddleArray[2];
    paddleArray[0] = &paddle_1;
    paddleArray[1] = &paddle_2;

    Ball ball(5, sf::Color::Red, 100, 100);

    // Main game loop: runs as long as the window is open
    while (window.isOpen()) {
        sf::Event event;

        // Handle events (e.g., window close, keyboard/mouse input)
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();  // Close the window when the user clicks the close button
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
            paddle_1.playerInput(-0.1);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            paddle_1.playerInput(0.1);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            paddle_2.playerInput(-0.1);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            paddle_2.playerInput(0.1);
        }


        // Clear the window with a black color
        window.clear(sf::Color::Black);

        ball.physics(WIDTH, HEIGHT, paddleArray, 2);
        ball.draw(window);
        paddle_1.draw(window);
        paddle_2.draw(window);

        // Draw the circle shape
        //window.draw(shape);

        // Display everything we've drawn (i.e., render the frame)
        window.display();
    }

    return 0;
}
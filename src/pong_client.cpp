//
// Created by donat on 25/08/2024.
//
#include <SFML/Graphics.hpp>
#include "../include/Ball.h"
#include "../include/Paddle.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

#define MAIN_SERVER_PORT 50148

#define WIDTH 800
#define HEIGHT 600

int main()
{
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
    serverAddr.sin_port = htons(MAIN_SERVER_PORT); // Port 8080
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
    const char* initialMessage = "LIST_ROOMS";
    send(clientSocket, initialMessage, strlen(initialMessage), 0);

    while(1) {
        char buffer[1024];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            std::cout << "Received from server: " << std::string(buffer, 0, bytesReceived) << std::endl;
        }

        std::string message;
        std::getline(std::cin, message);


        int result = send(clientSocket, message.c_str(), message.size(), 0);
        if (result == SOCKET_ERROR) {
            std::cerr << "Send failed." << std::endl;
            break;
        }
        if(message == "exit")
        {
            break;
        }


    }
    // Step 5: Clean up
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
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
#include <limits>
#include <string.h>
#include <sstream>

#pragma comment(lib, "Ws2_32.lib")



#define WIDTH 800
#define HEIGHT 600

std::vector<std::string> splitString(const std::string& str, char delimiter) {
    std::vector<std::string> substrings;
    std::stringstream ss(str);
    std::string token;

    // Use getline to split the string by the delimiter
    while (std::getline(ss, token, delimiter)) {
        substrings.push_back(token);
    }

    return substrings;
}


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
    const char* initialMessage = (std::to_string(CLIENT_REQUEST_ALL_ROOMS)).c_str();
    send(clientSocket, initialMessage, strlen(initialMessage), 0);
    while(1) {
        char buffer[1024];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            std::cout << "Received from server: " << std::string(buffer, 0, bytesReceived) << std::endl;
        }

        std::string message;
        int code = -1;

        while (true) {
            std::cout << "Enter a number between 1001 and 1004: \n 1001 : Request all rooms. \n 1002 : Create new room. \n 1003;xxxxxx : Join room (xxx being the room you wanna join). \n 1004 : Quit.\n";
            std::cin >> message;
            std::cout << message;

            code = stoi(splitString(message, TRANSFERT_DELIMITER)[0]);
            // Check if the input is valid and within the range
            if (std::cin.fail() || code < 1001 || code > 1004) {
                std::cerr << "Invalid input. Please enter a number between 100 and 200." << std::endl;
                std::cin.clear();  // Clear the error flag
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard invalid input
            } else {
                break;  // Valid input, break the loop
            }
        }

        // Convert the integer message to a string for sending over the socket

        // Send the message to the server
        int result = send(clientSocket, message.c_str(), message.size(), 0);
        if (result == SOCKET_ERROR) {
            std::cerr << "Send failed." << std::endl;
            // Handle the error (e.g., break the loop or return from the function)
        }


        if(code == 1004)
        {
            char buffer[1024];
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived > 0) {
                std::cout << "Received from server: " << std::string(buffer, 0, bytesReceived) << std::endl;
            }
            break;
        }


    }
    // Step 5: Clean up
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
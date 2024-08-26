//
// Created by donat on 25/08/2024.
//
#include <csignal>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <sstream>
#include "../include/const.h"

#pragma comment(lib, "Ws2_32.lib")


// Port for the main server to listen on

// Structure to represent a game room
struct GameRoom {
    int port;
    int id;
    int numberOfPlayer;
    std::string name;
};

// Map to store active game rooms
std::map<int, GameRoom> gameRooms;

bool running = true;

// Signal handler to stop the server gracefully
void signalHandler(int signum) {
    std::cout << "\nInterrupt signal (" << signum << ") received. Stopping server..." << std::endl;
    running = false;
}

// Function to start a game room server as a separate process
void startGameRoomServer() {
    std::string command = "pong_room.exe ";
    _spawnlp(_P_NOWAIT, command.c_str(), command.c_str(), nullptr);
}

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

// Function to handle incoming client connections
void handleClient(SOCKET clientSocket) {
    bool client_alive = true;
    while(client_alive)
    {
        char buffer[256];
        int recvSize = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (recvSize > 0) {
            buffer[recvSize] = '\0';
            std::cout << buffer;
            std::vector<std::string> result = splitString(buffer, TRANSFERT_DELIMITER);
            int requestCode = std::stoi(result[0]);  // Convert received buffer to integer code

            switch (requestCode) {
            case CLIENT_REQUEST_ALL_ROOMS: {
                    std::cout << "Client requested list of room\n";
                    std::string response = "Available rooms:\n";
                    for (const auto& room : gameRooms) {
                        response += "Room Name: " + room.second.name + ", Port: " + std::to_string(room.second.port) + "\n";
                    }
                    send(clientSocket, response.c_str(), response.size(), 0);
                    break;
            }

            case CLIENT_REQUEST_ROOM_CREATION: {

                    startGameRoomServer();
                    std::cout << "Starting new room.." << std::endl;
                    std::string response = "Starting new room..";
                    send(clientSocket, response.c_str(), response.size(), 0);
                    break;
            }

            case CLIENT_REQUEST_JOIN_ROOM: {

                    int port = std::stoi(result[1]);
                    if (gameRooms.find(port) != gameRooms.end()) {
                        std::cout << "Client joining room on port " + std::to_string(port) << std::endl;
                        std::string response = "Joining room on port " + std::to_string(port);
                        send(clientSocket, response.c_str(), response.size(), 0);
                    } else {
                        std::string response = "    Room not found.";
                        send(clientSocket, response.c_str(), response.size(), 0);
                    }
                    break;
            }

            case CLIENT_REQUEST_QUIT: {
                    std::string response = "Exiting.";
                    send(clientSocket, response.c_str(), response.size(), 0);
                    client_alive = false;
                    break;
            }

            case GAME_ROOM_REQUEST_ROOM_STARTED: {
                    std::cout << "Game room started";
                    break;
            }

            default: {
                    std::cout << "Unrecognized request.\n";
                    std::string response = "Unrecognized request.";
                    send(clientSocket, response.c_str(), response.size(), 0);
                    break;
            }
            }
        }
    }

    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    SOCKET listenSocket = INVALID_SOCKET;
    SOCKET clientSocket = INVALID_SOCKET;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(MAIN_SERVER_PORT);

    if (bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed." << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Main server listening on port " << MAIN_SERVER_PORT << std::endl;

    // Register signal handler for graceful shutdown
    signal(SIGINT, signalHandler);

    while (running) {
        // Accept a client connection (non-blocking check for exit)
        clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &clientAddrSize);
        if (clientSocket == INVALID_SOCKET) {
            if (WSAGetLastError() == WSAEWOULDBLOCK && !running) {
                break;  // Exit the loop if we're shutting down
            }
            std::cerr << "Accept failed." << std::endl;
            continue;
        }

        std::cout << "Client connected." << std::endl;

        handleClient(clientSocket);

        // Close client socket after handling
        closesocket(clientSocket);
        std::cout << "Client disconnected." << std::endl;
    }

    std::cout << "Shutting down server..." << std::endl;

    // Close the listening socket
    closesocket(listenSocket);

    // Cleanup Winsock
    WSACleanup();

    std::cout << "Server stopped." << std::endl;
    return 0;
}
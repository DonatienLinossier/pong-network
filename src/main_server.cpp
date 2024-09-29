//
// Created by donat on 25/08/2024.
//
#define FD_SETSIZE 1024
#include <iostream>
#include <string>
#include <vector>
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <sstream>
#include "../include/const.h"
#include "../include/GameRoom.h"

#pragma comment(lib, "Ws2_32.lib")


// Port for the main server to listen on

// Structure to represent a game room


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
std::vector<GameRoom> gameRooms;



// Signal handler to stop the server gracefully

// Function to start a game room server as a separate process
void startGameRoomServer() {
    std::string command = "start cmd /k pong_room.exe";
    int result = std::system(command.c_str());
    if (result != 0) {
        // Handle error
    }
}


void shutdown(fd_set currentSockets)
{
    std::cout << "Shutting down server..." << std::endl;

    for(int i =0; i< FD_SETSIZE; i++)
    {
        if(FD_ISSET(i, &currentSockets))
        {
            FD_CLR(i, &currentSockets);
            closesocket(i);
        }
    }
    WSACleanup();

    std::cout << "Server stopped." << std::endl;
}

// Function to handle incoming client connections
int handleClient(SOCKET clientSocket) {
        char buffer[256];
        int recvSize = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (recvSize > 0) {
            buffer[recvSize] = '\0';
            std::cout << buffer << " - ";
            std::vector<std::string> result = splitString(buffer, TRANSFERT_DELIMITER);
            int requestCode = std::stoi(result[0]);  // Convert received buffer to integer code

            switch (requestCode) {
                case CLIENT_REQUEST_ALL_ROOMS: {
                        std::cout << "Client requested list of room\n";
                        std::string response = "Available rooms:\n";
                        for (const auto& room : gameRooms) {
                            response += "Port: " + std::to_string(room.getPort()) + " | " + std::to_string(room.getNumberOfPlayer()) + "/" + std::to_string(room.getMaxNumberOfPlayer()) + " \n";
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

                /*case CLIENT_REQUEST_JOIN_ROOM: {

                        int port = std::stoi(result[1]);
                        for(int i =0; i<gameRooms.size(); i++)
                        {
                            if(gameRooms.at(i).getPort() == port)
                            {
                                std::cout << "Client joining room on port " + std::to_string(port) << std::endl;
                                std::string response = "Joining room on port " + std::to_string(port);
                                send(clientSocket, response.c_str(), response.size(), 0);
                                break;
                            }
                        }
                        std::string response = "    Room not found.";
                        send(clientSocket, response.c_str(), response.size(), 0);
                        break;
                }*/

                case GAME_ROOM_REQUEST_UPDATE_STATUS_MS: {

                        int port = std::stoi(result[1]);
                        int nbPlayer = std::stoi(result[2]);
                        for(int i =0; i<gameRooms.size(); i++)
                        {
                            if(gameRooms.at(i).getPort() == port)
                            {
                                gameRooms.at(i).setNumberOfPlayer(nbPlayer);
                                std::cout << "Game room on port " + std::to_string(port) + " updated its status to "
                                            + std::to_string(gameRooms.at(i).getNumberOfPlayer()) + "/"
                                            + std::to_string(gameRooms.at(i).getMaxNumberOfPlayer()) << std::endl;


                                std::string response = "Received";
                                send(clientSocket, response.c_str(), response.size(), 0);
                                break;
                            }
                        }
                        std::string response = "    Room not found.";
                        send(clientSocket, response.c_str(), response.size(), 0);
                        break;
                }

                case CLIENT_REQUEST_QUIT_MS: {
                        std::string response = "Exiting.";
                        send(clientSocket, response.c_str(), response.size(), 0);
                        return 1;
                }

                case GAME_ROOM_REQUEST_ROOM_STARTED: {
                        int gameRoom_port = std::stoi(result[1]);
                        std::cout << "Game room started on port " << gameRoom_port << std::endl;
                        GameRoom newGameRoom(gameRoom_port);
                        gameRooms.push_back(newGameRoom);

                        std::string response = std::to_string(ACK);
                        std::cout << "Sending :" << response << std::endl;
                        send(clientSocket, response.c_str(), response.size(), 0);

                        break;
                }

                case GAME_ROOM_REQUEST_END_CONNECTION:
                    {
                        int gameRoom_port = std::stoi(result[1]);
                        std::cout << "Game room on port " << gameRoom_port << " ended connection" << std::endl;

                        // Use an iterator to find and erase the element
                        for (auto it = gameRooms.begin(); it != gameRooms.end(); ++it)
                        {
                            if (it->getPort() == gameRoom_port)
                            {
                                gameRooms.erase(it);  // Erase the element using the iterator
                                break;  // Break out of the loop after erasing
                            }
                        }
                        return 1;
                    }

                default: {
                        std::cout << "Unrecognized request.\n";
                        std::string response = "Unrecognized request.";
                        send(clientSocket, response.c_str(), response.size(), 0);
                        break;
                }
            }
            return 0;
    } else
    {
        return -1; //Clear SOCKET !!!
    }
}

SOCKET setupServer(int port)
{
    WSADATA wsaData;
    SOCKET listenSocket = INVALID_SOCKET;

    struct sockaddr_in serverAddr;

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
    serverAddr.sin_port = htons(port);

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

    std::cout << "Main server listening on port " << port << std::endl;

    return listenSocket;
}

SOCKET acceptConnection(SOCKET serverSocket)
{
    SOCKET clientSocket = INVALID_SOCKET;
    struct sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddr, &clientAddrSize);
    return clientSocket;
}

int main() {

    SOCKET listenSocket = setupServer(MAIN_SERVER_PORT);

    bool running = true;

    fd_set currentSockets, readySockets;

    FD_ZERO(&currentSockets);
    FD_SET(listenSocket, &currentSockets);
    std::cout << "Listening socket : " << listenSocket << std::endl;

    try
    {
        while (running) {
            readySockets = currentSockets;

            timeval timeout;
            timeout.tv_sec = 2;
            timeout.tv_usec = 0;


            int resultSelect = select(FD_SETSIZE, &readySockets, NULL, NULL, &timeout);
            if(resultSelect<0)
            {
                std::cout << "error";
            }

            for(int i = 0; i<FD_SETSIZE; i++)
            {
                //VRm de la merde FD_ISSET. En gros on a FD_SET qui contient des valeurs de FD. Genre 54 23 5.
                // FD_ISSET(0, ..) verifie que l ELEMENT 0 soit dans ..., pas le que 0eme element soit défini.
                // Du coup on teste toutes les valeurs de 0 à 1024 parce que les FD ont des valeurs un peu random..
                //Puis bon, on prie un peu pour qu'aucun FD ne soit au dessus de 1024 sinon il ne sera traité
                // Bref en conclusion select ne parait pas si bien ? A gere soit meme avec des tabl ou sinon
                // regarder du coté de IOCP
                if(FD_ISSET(i, &readySockets))
                {
                    //std::cout << i;
                    if(i==listenSocket) //Because 0 is the socket of the server (that is listening for new connection) [see l.188]
                    {
                        int newClientSocket = acceptConnection(listenSocket);
                        FD_SET(newClientSocket ,&currentSockets);
                        std::cout << "New connection" << std::endl;
                    } else
                    {
                        int returnCodeClient = handleClient(i);

                        switch(returnCodeClient){
                            case -1:
                                std::cout << "Connection with client ended prematurly - Clearing socket.";
                            case 1:
                                {
                                    FD_CLR(i, &currentSockets);
                                    closesocket(i);
                                }

                        }
                    }
                }
            }
        }
    }

    catch(...)
    {
        shutdown(currentSockets);
        return 1;

    };

    shutdown(currentSockets);
    return 0;
}
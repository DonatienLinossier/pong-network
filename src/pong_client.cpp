//
// Created by donat on 25/08/2024.
//
#include "../include/const.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <limits>
#include <string.h>
#include <sstream>

#include "../include/PongGame.h"

#pragma comment(lib, "Ws2_32.lib")



#define WIDTH 800
#define HEIGHT 600

int setNonBlocking(SOCKET socket) {
    u_long mode = 1;  // 1 to enable non-blocking mode
    return ioctlsocket(socket, FIONBIO, &mode);
}

std::vector<std::string> splitStringc(const std::string& str, char delimiter) {
    std::vector<std::string> substrings;
    std::stringstream ss(str);
    std::string token;

    // Use getline to split the string by the delimiter
    while (std::getline(ss, token, delimiter)) {
        substrings.push_back(token);
    }

    return substrings;
}

int enterRoomGame(SOCKET port)
{
    std::cout << "Connecting to port : " << port << std::endl;
    int sockfd;
    char codeBuffer[4];
    char buffer[1024];
    const char *message = std::to_string(CLIENT_REQUEST_CONNECT_GR).c_str();
    struct sockaddr_in serverAddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation failed." << std::endl;
        return -1;
    }

    // Filling server information
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET; // IPv4
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    socklen_t addrLen = sizeof(serverAddr);

    // Send the message to the server
    sendto(sockfd, (const char *)message, strlen(message), 0, (const struct sockaddr *)&serverAddr, addrLen);
    std::cout << "Message sent to server." << std::endl;

    short playerID = -1;
    int preparationLoop = 1;
    while(preparationLoop) {

        // Receive the server's response
        int n = recvfrom(sockfd, (char *)codeBuffer, 4, 0, (struct sockaddr *)&serverAddr, &addrLen);
        if(n<=0)
        {
            std::cout << "Error with recv : " << n << ". " << WSAGetLastError();
            return 1;
        }
        codeBuffer[n] = '\0';  // Null-terminate the received data
        std::cout << "Server: " << codeBuffer << std::endl;
        switch (std::stoi(codeBuffer))
        {
        case EXT:
            {
                std::cout << "Room already full" << std::endl;
                break;
            }
        default:
            {
                std::cout << "unexpected data !" << codeBuffer << std::endl;
                break;
            }
        case ACK:
            {
                playerID = 1;
                std::cout << "Client connected, waiting for 2nd client." << std::endl;
                memset(buffer, 0, sizeof(buffer));

                const char *message = std::to_string(CLIENT_REQUEST_GET_STATUS).c_str();
                sendto(sockfd, (const char *)message, strlen(message), 0, (const struct sockaddr *)&serverAddr, addrLen);

                int t = recvfrom(sockfd, (char *)buffer, 1026, 0, (struct sockaddr *)&serverAddr, &addrLen);
                buffer[t] = '\0';  // Null-terminate the received data
                std::cout << 1 << buffer << std::endl;
                break;
            }
        case GAME_ROOM_REQUEST_GAME_STARTING:
            {
                if (playerID == -1)
                {
                    playerID = 2;
                }
                std::cout << "Party starting" << buffer;
                preparationLoop = 0;
                break;
            }
        }
    }

    PongGame pongGame(playerID);
    pongGame.init(WIDTH, HEIGHT);

    setNonBlocking(sockfd);

    while (pongGame.getGameRunning()) {

        memset(buffer, 0, sizeof(buffer));
        int t = recvfrom(sockfd, (char *)buffer, 1026, 0, (struct sockaddr *)&serverAddr, &addrLen);
        if (t > 0) {
            pongGame.loadSerializedData(buffer);
        } else
        {
            //often no data retrieved
            //TODO: add error handling
        }

        pongGame.events();
        //pongGame.physics();
        pongGame.render();


        std::vector<char> serializedData = pongGame.getPlayerSerializedData();
        sendto(sockfd, serializedData.data(), serializedData.size(), 0, (const struct sockaddr *)&serverAddr, addrLen);
        //pongGame.loadSerializedData(pongGame.getSerializedData().data());

    }



    closesocket(sockfd);
    return 0;
}

int main()
{

    //Setup Winsock

    WSADATA wsaData;

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    //Create socket that will connect to MS
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    //Connect serv to MS
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


    //Send first request(Requesting all available rooms)
    const char* initialMessage = (std::to_string(CLIENT_REQUEST_ALL_ROOMS)).c_str();
    send(clientSocket, initialMessage, strlen(initialMessage), 0);


    //First recv data
    //Then wait for MS respond
    //Ask the client what does he wanna do
    //Send the request to the MS
    short code;
    while(1) {
        char buffer[1024];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            std::cout << "Received from server: " << std::string(buffer, 0, bytesReceived) << std::endl;
        }

        std::string message;


        while (true) {
            code = -1;
            std::cout << "Enter a number between 1001 and 1004: \n"
                         " 2101 : Request all rooms. \n"
                         " 2102 : Create new room. \n"
                         " 2103|xxxxxx : Join room (xxxxx being the room you wanna join). \n"
                         " 2104 : Quit.\n";

            std::cin >> message;
            std::cout << message;

            code = stoi(splitStringc(message, TRANSFERT_DELIMITER)[0]);
            // Check if the input is valid and within the range
            if (std::cin.fail() || code < 2101 || code > 2104) {
                std::cerr << "Invalid input. Please enter a number between 1001 and 1004." << std::endl;
                std::cin.clear();  // Clear the error flag
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // Discard invalid input
            } else {
                break;  // Valid input, break the loop
            }
        }

        if(code == CLIENT_REQUEST_JOIN_ROOM)
        {
            SOCKET port = stoi(splitStringc(message, TRANSFERT_DELIMITER)[1]);
            enterRoomGame(port);
            break;
        }

        // Convert the integer message to a string for sending over the socket
        // Send the message to the server
        int result = send(clientSocket, message.c_str(), message.size(), 0);
        if (result == SOCKET_ERROR) {
            std::cerr << "Send failed." << std::endl;

        }

        //If code is 1004 wait for serv ACK and then break the loop
        if(code == CLIENT_REQUEST_QUIT_MS)
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
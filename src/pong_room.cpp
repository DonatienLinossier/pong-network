//
// Created by donat on 25/08/2024.
//

#include "../include/const.h"
#include "../include/GameRoom.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#include "../include/PongGame.h"


#pragma comment(lib, "Ws2_32.lib")

#define WIDTH 800
#define HEIGHT 600

int setNonBlockingb(SOCKET socket) {
    u_long mode = 1;  // 1 to enable non-blocking mode
    return ioctlsocket(socket, FIONBIO, &mode);
}

std::vector<std::string> splitStringcbis(const std::string& str, char delimiter) {
    std::vector<std::string> substrings;
    std::stringstream ss(str);
    std::string token;

    // Use getline to split the string by the delimiter
    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) { // Optional: skip empty tokens
            substrings.push_back(token);
        }
    }

    return substrings;
}

std::vector<std::string> splitString(const std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}


int handleUDP(SOCKET UDP_socket, sockaddr_in client_addr, socklen_t client_addr_len, char* buffer, GameRoom gameRoom)
{
    std::vector<std::string> data = splitString(buffer, TRANSFERT_DELIMITER);
    int code = std::stoi(data.at(0));
    std::cout << code << " " << (code==CLIENT_REQUEST_GET_STATUS);
    switch (code)
    {
    case CLIENT_REQUEST_GET_STATUS:
        {
            const char *status_msg = gameRoom.getData().c_str();
            sendto(UDP_socket, status_msg, strlen(status_msg), 0, (struct sockaddr *)&client_addr, client_addr_len);
            break;
        }

    default:
        {
            std::cout << "Unrecognized request : {Code: '" << code << "' ; buffer: " << buffer << std::endl;
            break;
        }
    }
    return 0;
}


int main() {



    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return 1;
    }






    //Setting listening UDP socket
    SOCKET UDP_socket = INVALID_SOCKET;
    struct sockaddr_in serverAddr;



    UDP_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (UDP_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed." << std::endl;
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(0); //OS automatically assign an available port

    if (bind(UDP_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(UDP_socket);
        WSACleanup();
        return 1;
    }

    // Retrieve the actual port assigned by the OS
    int addrLen = sizeof(serverAddr);


    if (getsockname(UDP_socket, (SOCKADDR*)&serverAddr, &addrLen) == SOCKET_ERROR) {
        std::cerr << "getsockname() failed." << std::endl;
        closesocket(UDP_socket);
        WSACleanup();
        return 1;
    }

    int listeningPort = ntohs(serverAddr.sin_port);
    GameRoom gameRoom(listeningPort);
    std::cout << "Game room listening on port " << listeningPort << std::endl;







    int result;

    // Step 2: Create MS socket
    SOCKET mainServer_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mainServer_socket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Step 3: Connect to the server
    sockaddr_in mainServerAddr;
    mainServerAddr.sin_family = AF_INET;
    mainServerAddr.sin_port = htons(MAIN_SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &mainServerAddr.sin_addr); // Connect to localhost

    result = connect(mainServer_socket, (sockaddr*)&mainServerAddr, sizeof(mainServerAddr));
    if (result == SOCKET_ERROR) {
        std::cerr << "Connect failed: " << WSAGetLastError() << std::endl;
        closesocket(mainServer_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server!" << std::endl;

    // Step 4: Send and receive data
    const char* initialMessage = (std::to_string(GAME_ROOM_REQUEST_ROOM_STARTED) + "|" +  std::to_string(listeningPort)).c_str();
    send(mainServer_socket, initialMessage, strlen(initialMessage), 0);
    std::cout << "Sending : " <<  initialMessage << std::endl;


    char buffer[256];
    recv(mainServer_socket, buffer, sizeof(buffer), 0);
    if(std::stoi(buffer) != ACK)
    {
        std::cerr << "Unexpected receved data : " << buffer << std::endl;
    }


    std::cout<<"waiting for client connection\n";


    struct sockaddr_in client_addr;
    struct sockaddr_in clients[MAX_CLIENTS_PER_ROOM];
    socklen_t client_addr_len = sizeof(client_addr);
    char client_ip[INET_ADDRSTRLEN];

    char bufferBis[256];
    int clientCount = 0;
    while (1) {
        // Receive a datagram
        std::cout << "waiting for data\n";
        ssize_t recv_len = recvfrom(UDP_socket, bufferBis, sizeof(bufferBis) - 1, 0,
                                    (struct sockaddr *)&client_addr, &client_addr_len);
        //std::cout << "receved something";
        if (recv_len < 0) {
            perror("receive failed");
            continue;
        }
        if(recv_len == 0)
        {
            continue;
        }

        bufferBis[recv_len] = '\0';  // Null-terminate the received message

        //inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

        // Convert client IP to string
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

        printf("Received message from %s:%d - %s\n", client_ip, ntohs(client_addr.sin_port), bufferBis);

        // Check if we are accepting new clients
        std::cout << clientCount << "/" << MAX_CLIENTS_PER_ROOM << std::endl;


        int known_client = -1;  // Initialize to -1 to represent "not found"
        for (int i = 0; i < clientCount; i++) {
            if (clients[i].sin_addr.s_addr == client_addr.sin_addr.s_addr &&
                clients[i].sin_port == client_addr.sin_port) {
                    known_client = i;
                    break;
                }
        }

        if (known_client != -1) {
            // Handle game data from known clients
            printf("Received valid data from a known client (Client %d): %s\n", known_client + 1, buffer);
            handleUDP(UDP_socket, client_addr, client_addr_len, bufferBis, gameRoom);
        } else {
            // If the client is not known, check if there's room for a new client
            if (clientCount < MAX_CLIENTS_PER_ROOM) {
                // Store the client's address
                clients[clientCount] = client_addr;
                clientCount++;
                gameRoom.setNumberOfPlayer(gameRoom.getNumberOfPlayer()+1);


                printf("New client %d connected: %s:%d\n", clientCount, client_ip, ntohs(client_addr.sin_port));

                // If both clients are connected, notify them that the game can start
                if (clientCount == MAX_CLIENTS_PER_ROOM) {
                    const char *start_msg = std::to_string(GAME_ROOM_REQUEST_GAME_STARTING).c_str();
                    for (int i = 0; i < MAX_CLIENTS_PER_ROOM; i++) {
                        sendto(UDP_socket, start_msg, strlen(start_msg), 0,
                               (struct sockaddr *)&clients[i], sizeof(clients[i]));
                    }
                    printf("Both clients connected. Game started.\n");
                    break;  // Exit the loop if the game has started
                } else
                {
                    // Send acknowledgment to the client
                    const char *ack_msg = std::to_string(ACK).c_str();
                    sendto(UDP_socket, ack_msg, strlen(ack_msg), 0, (struct sockaddr *)&client_addr, client_addr_len);
                }
            } else {
                // No room for new clients, ignore the data
                const char *ext_msg = std::to_string(EXT).c_str();
                printf("Received data from an unknown client, but room is full. Ignoring.\n");
                sendto(UDP_socket, ext_msg, strlen(ext_msg), 0,(struct sockaddr *)&client_addr, client_addr_len);
                continue;
            }
        }
    }

    PongGame pongGame(0);
    pongGame.init(WIDTH, HEIGHT);



    setNonBlockingb(UDP_socket);
    while (pongGame.getGameRunning()) {

        pongGame.events();
        pongGame.physics();
        pongGame.render();

        std::vector<char> serializedData = pongGame.getSerializedData();
        const char* dataToSend = serializedData.data(); // Pointer to the vector data
        size_t dataSize = serializedData.size();        // Size of the serialized data

        for (int i = 0; i < MAX_CLIENTS_PER_ROOM; i++) {
            sendto(UDP_socket, dataToSend, dataSize, 0,
                   (struct sockaddr *)&clients[i], sizeof(clients[i]));
        }

        //pongGame.loadSerializedData(pongGame.getSerializedData().data());




        for (int i = 0; i < MAX_CLIENTS_PER_ROOM; i++) {
            ssize_t recv_len = recvfrom(UDP_socket, bufferBis, sizeof(bufferBis) - 1, 0,
                                        (struct sockaddr *)&client_addr, &client_addr_len);
            if (recv_len > 0) {
                pongGame.loadSerializedData(bufferBis);
            } else if (recv_len==0)
            {
                //Often just that we received nothing
                //TODO: error handling
            }


        }
    }

    const char* message = (std::to_string(GAME_ROOM_REQUEST_END_CONNECTION) + "|" + std::to_string(listeningPort)).c_str();
    send(mainServer_socket, message, strlen(message), 0);


    // Close socket
    closesocket(UDP_socket);
    closesocket(mainServer_socket);
    WSACleanup();

    return 0;
}
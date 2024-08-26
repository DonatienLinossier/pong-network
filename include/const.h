//
// Created by donat on 26/08/2024.
//

#ifndef CONST_H
#define CONST_H

#define MAIN_SERVER_PORT 50148

#define MAIN_SERVER 0
#define CLIENT 1
#define GAME_ROOM 2

//Exemple for code:
//          1    |    0    |    0      4
//
//   From client | To serv | Operation ID
#define CLIENT_REQUEST_ALL_ROOMS 1001
#define CLIENT_REQUEST_ROOM_CREATION 1002
#define CLIENT_REQUEST_JOIN_ROOM 1003
#define CLIENT_REQUEST_QUIT 1004

#define GAME_ROOM_REQUEST_ROOM_STARTED 2101


#define TRANSFERT_DELIMITER '|'
#endif //CONST_H

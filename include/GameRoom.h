//
// Created by donat on 02/09/2024.
//

#ifndef GAMEROOM_H
#define GAMEROOM_H

#include <time.h>
#include <iostream>
#include <string>
#include <iomanip>
#include <sstream>
#include "../include/const.h"



class GameRoom {
public:
    GameRoom(int port);
    int getPort() const;
    int getNumberOfPlayer() const;
    int getMaxNumberOfPlayer() const;

    void setNumberOfPlayer(int newNumberOfPlayer);
    void setMaxNumberOfPlayer(int newMaxNumberOfPlayer);
    std::string getData();



private:
    int port;
    int numberOfPlayer;
    int maxNumberOfPlayer;
    time_t startTime;
};


#endif //GAMEROOM_H

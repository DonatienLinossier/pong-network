//
// Created by donat on 02/09/2024.
//

#include "../include/GameRoom.h"




GameRoom::GameRoom(int port)
  : port(port), numberOfPlayer(0), maxNumberOfPlayer(2) {
    time(&startTime);
    std::cout << startTime;

}


int GameRoom::getPort() const { return port; }
int GameRoom::getNumberOfPlayer() const { return numberOfPlayer; }
int GameRoom::getMaxNumberOfPlayer() const { return maxNumberOfPlayer; }

void GameRoom::setNumberOfPlayer(int newNumberOfPlayer) {numberOfPlayer = newNumberOfPlayer;}
void GameRoom::setMaxNumberOfPlayer(int newMaxNumberOfPlayer) {maxNumberOfPlayer = newMaxNumberOfPlayer;}

std::string GameRoom::getData() {
    std::stringstream ss;
    // Format each value with specified width
    ss << std::setw(5) << port << TRANSFERT_DELIMITER
       << std::setw(1) << numberOfPlayer << TRANSFERT_DELIMITER
       << std::setw(1) << maxNumberOfPlayer << TRANSFERT_DELIMITER
        << startTime;
    //std::cout << ss.str();
    return ss.str();
}
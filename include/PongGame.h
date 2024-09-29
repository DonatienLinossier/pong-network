//
// Created by donat on 29/09/2024.
//

#ifndef PONGGAME_H
#define PONGGAME_H
#include <SFML/Graphics.hpp>

#include "Ball.h"
#include "Drawable.h"
#include "Paddle.h"


class PongGame {
private:

    short player; //What is the view of this instance [-1: unknown; 0: spectator; default: player X]
    bool gameRunning; //Is the game still running ?

    std::vector<Drawable*> objects; //Contains all the drawables of the scene.
    std::vector<Paddle*> paddleArray; //Contains all the paddles of the scene. (Basicaly for ball colision)
    std::vector<Ball*> ballsArray; //Contains all the balls of the scene. (Often only 1)
    sf::RenderWindow window; // The renderWindow
public:
    PongGame();
    ~PongGame();


    void init(); //Initialize the game
    void events(); // Handle all the events [windows closed & player input for now]
    void physics(); // Do the physics calculations of the scene[ball/paddle and ball/wall collision]
    void render() const; // Update the frame

    void sendInstance(); //Send a copy of this instance
    void loadInstance(); //Load the data a specific instance

    bool getGameRunning();

};



#endif //PONGGAME_H

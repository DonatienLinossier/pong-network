//
// Created by donat on 29/09/2024.
//

#ifndef PONGGAME_H
#define PONGGAME_H
#include <memory>
#include <SFML/Graphics.hpp>


#include "Ball.h"
#include "Drawable.h"
#include "Paddle.h"
#include "const.h"


class PongGame {
private:

    int mIdCounter;
    std::map<int, std::shared_ptr<Drawable>> mIDMap;
    unsigned short mPlayer; //What is the view of this instance [0: spectator; default: player X]
    bool mGameRunning; //Is the game still running ?
    bool mInitialized;
    unsigned short mWidth;
    unsigned short mHeight;
    std::shared_ptr<Paddle> mPlayerPaddle;

    std::vector<std::shared_ptr<Paddle>> mPaddleArray; //Contains all the paddles of the scene. (Basicaly for ball colision)
    std::vector<std::shared_ptr<Ball>> mBallsArray; //Contains all the balls of the scene. (Often only 1)
    std::unique_ptr<sf::RenderWindow> mWindow; // The renderWindow
public:
    PongGame(unsigned short playerView);
    ~PongGame();


    int init(unsigned short width, unsigned short height); //Initialize the game
    int events(); // Handle all the events [windows closed & player input for now]
    void physics(); // Do the physics calculations of the scene[ball/paddle and ball/wall collision]
    int render() const; // Update the frame

    bool getGameRunning() const;
    std::vector<char> getSerializedData() const;
    int loadSerializedData(const char* buffer);

    void addPaddle(float width, float height, float x_position, float y_position, const sf::Color& color);
    void addBall(float radius, float x_position, float y_position, const sf::Color& color);
};



#endif //PONGGAME_H

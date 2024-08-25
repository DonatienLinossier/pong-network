//
// Created by donat on 25/08/2024.
//

#ifndef BALL_H
#define BALL_H
#include <SFML/Graphics.hpp>
#include "Paddle.h"


class Ball {
    public:
        Ball(float radius, const sf::Color& color, float x, float y);

    private :

        float x_position;
        float y_position;
        float x_speed;
        float y_speed;

        float radius;

        sf::CircleShape ballShape;


    public:
        void physics(int WIDTH, int HEIGHT, Paddle** paddleArray, int paddleArray_size);
        void draw(sf::RenderWindow& window);

    private:
        void checkCollision_wall(int WIDTH, int HEIGHT);
        void checkCollision_paddle(Paddle* paddle);
        bool checkCollision(Paddle* paddle) const;

};



#endif //BALL_H

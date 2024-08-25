//
// Created by donat on 25/08/2024.
//

#ifndef PADDLE_H
#define PADDLE_H
#include <SFML/Graphics.hpp>
#include <tuple>


class Paddle {

    public :
    /**
         *
         * @param width width of the paddle
         * @param height height of the paddle
         * @param x_position x position of the paddle
         * @param y_position y position of the paddle
         * @param color color of the paddle
         */
        Paddle(float width, float height, float x_position, float y_position, const sf::Color& color);
    private:
        float x_position;
        float y_position;
        float height;
        float width;

        sf::RectangleShape rectShape;




    public:
        void draw(sf::RenderWindow& window);
        std::tuple<float, float, float, float> getHitbox() const;
        void playerInput(float y_movement);

};



#endif //PADDLE_H

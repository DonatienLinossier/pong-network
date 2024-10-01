//
// Created by donat on 25/08/2024.
//

#ifndef PADDLE_H
#define PADDLE_H
#include <SFML/Graphics.hpp>
#include <tuple>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "Drawable.h"
#include "../include/const.h"


class Paddle : public Drawable{

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
        std::string getData() override;
        void draw(sf::RenderWindow& window) override;
        std::tuple<float, float, float, float> getHitbox() const;
        void playerInput(float y_movement);
        void loadData(std::string data) override;
        void serialize(std::vector<char>& buffer) const override;
        void deserialize(const char* buffer, size_t& offset) override;
};



#endif //PADDLE_H

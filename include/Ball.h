//
// Created by donat on 25/08/2024.
//

#ifndef BALL_H
#define BALL_H
#include <memory>
#include <SFML/Graphics.hpp>
#include "Paddle.h"
#include "Drawable.h"


class Ball : public Drawable{
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
        void physics(int WIDTH, int HEIGHT, std::vector<std::shared_ptr<Paddle>> &paddleArray, int paddleArray_size);
        void draw(sf::RenderWindow& window) override;
        std::string getData() override;
        void loadData(std::string buffer) override;

        void serialize(std::vector<char>& buffer) const override;
        void deserialize(const char* buffer, size_t& offset) override;

    private:
        void checkCollision_wall(int WIDTH, int HEIGHT);
        void checkCollision_paddle(Paddle* paddle);
        void checkCollision_paddle(std::shared_ptr<Paddle> &paddle);
        bool checkCollision(Paddle* paddle) const;
        bool checkCollision(std::shared_ptr<Paddle> const &paddle) const;


};



#endif //BALL_H

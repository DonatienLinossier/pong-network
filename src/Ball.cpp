//
// Created by donat on 25/08/2024.
//

#include "../include/Ball.h"

Ball::Ball(float radius, const sf::Color& color, float x, float y)
        : x_position(x), y_position(y), x_speed(0.02), y_speed(0.02), radius(radius)
{
    // Initialize the ball shape
    ballShape.setRadius(radius);
    ballShape.setFillColor(color);
    ballShape.setPosition(x, y);

}

void Ball::physics(int const WIDTH, int const HEIGHT, Paddle** paddleArray, int paddleArray_size)
{
    x_position += x_speed;
    y_position += y_speed;



    checkCollision_wall(WIDTH,HEIGHT);
    for(int i = 0; i < paddleArray_size; i++)
    {
        checkCollision_paddle(paddleArray[i]);
    }

}

void Ball::draw(sf::RenderWindow& window)
{
    this->ballShape.setPosition(x_position,y_position);
    window.draw(this->ballShape);
}

void Ball::checkCollision_wall(int const WIDTH, int const HEIGHT)
{
    if( x_position + radius > WIDTH)
    {
        x_position = WIDTH - radius;
        x_speed = -x_speed;
    } else if(x_position-radius<0)
    {
        x_position = radius;
        x_speed = -x_speed;
    }

    if( y_position + radius > HEIGHT)
    {
        y_position = HEIGHT - radius;
        y_speed = -y_speed;
    } else if (y_position - radius < 0)
    {
        y_position = radius;
        y_speed = -y_speed;
    }
}

bool Ball::checkCollision(Paddle* paddle) const
{

    float paddleLeft, paddleTop, paddleWidth, paddleHeight;
    std::tie(paddleLeft, paddleTop, paddleWidth, paddleHeight) = paddle->getHitbox();


    // Find the closest point on the paddle to the ball
    float closestX = std::max(paddleLeft, std::min(x_position, paddleLeft + paddleWidth));
    float closestY = std::max(paddleTop, std::min(y_position, paddleTop + paddleHeight));

    // Calculate the distance between the ball's center and this closest point
    float distanceX = x_position - closestX;
    float distanceY = y_position - closestY;
    float distanceSquared = distanceX * distanceX + distanceY * distanceY;

    // Check if the distance is less than or equal to the ball's radius squared
    return distanceSquared <= (radius * radius);

}
void Ball::checkCollision_paddle(Paddle* paddle)
{
    //std::tuple<float, float, float, float> paddleHitbox = paddle->getHitbox();
    if (checkCollision(paddle)) {
        x_position -= x_speed;
        x_speed = -x_speed;
    }

}



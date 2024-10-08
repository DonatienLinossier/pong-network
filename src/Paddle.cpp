//
// Created by donat on 25/08/2024.
//

#include "../include/Paddle.h"

std::vector<std::string> splitStringp(const std::string& str, char delimiter) {
    std::vector<std::string> substrings;
    std::stringstream ss(str);
    std::string token;

    // Use getline to split the string by the delimiter
    while (std::getline(ss, token, delimiter)) {
        substrings.push_back(token);
    }

    return substrings;
}

Paddle::Paddle(float width, float height, float x_position, float y_position, const sf::Color& color)
                : width(width), height(height), x_position(x_position), y_position(y_position)
{
    mType = PADDLE_ID;
    rectShape.setSize(sf::Vector2f(width, height));
    rectShape.setPosition(x_position, y_position);
    rectShape.setFillColor(color);
}

/**
 * 
 * @param window The window you want to draw on
 */
void Paddle::draw(sf::RenderWindow& window)
{
    this->rectShape.setPosition(x_position-(width/2), y_position-(height/2));
    window.draw(rectShape);
}

std::tuple<float, float, float, float> Paddle::getHitbox() const
{
    return std::make_tuple(x_position-(width/2), y_position-(height/2), width, height);
}

void Paddle::playerInput(float y_movement)
{
    y_position += y_movement;
}

std::string Paddle::getData()
{
    std::stringstream ss;

    ss << PADDLE_ID << TRANSFERT_DELIMITER << x_position << TRANSFERT_DELIMITER << y_position;
    return ss.str();
}

void Paddle::loadData(std::string buffer)
{
    try
    {
        std::vector<std::string> data = splitStringp(buffer, TRANSFERT_DELIMITER);
        if(stoi(data.at(0))!=PADDLE_ID)
        {
            std::cout << "Error: object is not a Paddle !";
            return;
        }
        x_position = stof(data.at(1));
        y_position = stof(data.at(2));
    } catch(...)
    {
        std::cout << "Error in paddle";
    }

}

void Paddle::serialize(std::vector<char>& buffer) const {
    // Serialize the paddle's position and size
    buffer.insert(buffer.end(), reinterpret_cast<const char*>(&x_position), reinterpret_cast<const char*>(&x_position) + sizeof(x_position));
    buffer.insert(buffer.end(), reinterpret_cast<const char*>(&y_position), reinterpret_cast<const char*>(&y_position) + sizeof(y_position));
}

void Paddle::deserialize(const char* buffer, size_t& offset) {
    // Deserialize the paddle's position and size
    memcpy(&x_position, buffer + offset, sizeof(x_position));
    offset += sizeof(x_position);
    memcpy(&y_position, buffer + offset, sizeof(y_position));
    offset += sizeof(y_position);
}

void Paddle::skipDeserialize(size_t& offset) const {
    // Deserialize the paddle's position and size
    offset += sizeof(x_position);
    offset += sizeof(y_position);
}
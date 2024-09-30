//
// Created by donat on 28/09/2024.
//

#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <SFML/Graphics/RenderWindow.hpp>

#include "NetworkSerializable.h"


class Drawable : public NetworkSeriasable
{
public:
    Drawable();
    virtual void draw(sf::RenderWindow& window) = 0;
    int getType() const;

protected:
    int mType;
};

#endif //DRAWABLE_H

//
// Created by donat on 28/09/2024.
//

#ifndef DRAWABLE_H
#define DRAWABLE_H

#include "NetworkSerializable.h"


class Drawable : public NetworkSeriasable
{
public:
    virtual void draw(sf::RenderWindow& window) = 0;

};

#endif //DRAWABLE_H

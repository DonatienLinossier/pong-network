//
// Created by donat on 28/09/2024.
//
#include <iostream>


#ifndef NETWORKSERIALIZABLE_H
#define NETWORKSERIALIZABLE_H

class NetworkSeriasable {
public:
    virtual std::string getData() = 0;
    virtual void loadData(std::string data) = 0;

};

#endif //NETWORKSERIALIZABLE_H

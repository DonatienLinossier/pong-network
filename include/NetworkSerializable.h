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
    //virtual std::vector<char> serializeData() const = 0;
    virtual void serialize(std::vector<char>& buffer) const = 0;
    virtual void deserialize(const char* buffer, size_t& offset) = 0;
};

#endif //NETWORKSERIALIZABLE_H

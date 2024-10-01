//
// Created by donat on 29/09/2024.
//

#include "../include/PongGame.h"

#include <memory>


PongGame::PongGame(unsigned short playerView) :
    mPlayer(playerView),
    mGameRunning(false),
    mInitialized(false),
    mWidth(0),
    mHeight(0),
    mIdCounter(0),
    mPlayerPaddle(nullptr){}

PongGame::~PongGame()
{

}

void PongGame::addBall(float radius, float x_position, float y_position, const sf::Color& color)
{
    auto ball = std::make_shared<Ball>(radius, color, x_position, y_position);
    mBallsArray.push_back(ball);
    mIDMap.insert(std::make_pair(mIdCounter, ball));
    mIdCounter++;

}

void PongGame::addPaddle(float width, float height, float x_position, float y_position, const sf::Color& color)
{
    auto paddle = std::make_shared<Paddle>(width, height, x_position, y_position, color);
    mPaddleArray.push_back(paddle);
    mIDMap.insert(std::make_pair(mIdCounter, paddle));
    mIdCounter++;
}



int PongGame::init(unsigned short width, unsigned short height)
{

    //Window init
    mWidth = width;
    mHeight = height;

    if(mPlayer == 0)
    {
        mWindow = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), "Game Room");
    } else
    {
        mWindow = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), "Client number " + std::to_string(mPlayer));
    }

    //Paddle inits
    addPaddle(10, 100, 50.0f, 100, sf::Color::White);
    addPaddle(10, 100, static_cast<float>(mWidth) - 50,  100, sf::Color::White);

    //Ball init
    addBall(5, 100, 100, sf::Color::Red);

    //Player view init
    if(mPlayer==0) //spectator
    {
        mPlayerPaddle=nullptr; //If spectator, the view cannot control any paddle
    } else if(mPlayer>0)
    {
        if(mPlayer-1<mPaddleArray.size())
        {
            mPlayerPaddle = mPaddleArray.at(mPlayer-1);

        } else
        {
            printf("Error: Player %d cannot control paddle %d because it doesn't exist. Size of mPaddleArray is %d", mPlayer, mPlayer, mPaddleArray.size());
            return 1;
        }

    } else
    {
        printf("Error: playerPaddle init. (Value must be positive)");
        return 2;
    }



    //End of initilization
    mInitialized = true;
    mGameRunning = true;
    return 0;
}

int PongGame::render() const
{
    mWindow->clear(sf::Color::Black);

    for(auto &paddle: mPaddleArray)
    {
        paddle->draw(*mWindow);
    }
    for(auto &ball: mBallsArray)
    {
        ball->draw(*mWindow);
    }

    mWindow->display();
    return 0;
}


int PongGame::events()
{
    sf::Event event;
    while(mWindow->pollEvent(event))
    {
        if (event.type == sf::Event::Closed) {
            mWindow->close();  // Close the window when the user clicks the close button
            mGameRunning=false;
            return 1;
        }
    }

    if(mPlayer != 0) // If the view is not a spectator
    {
        if (mPlayerPaddle != nullptr) { // Check if mPlayerPaddle is not null
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                mPlayerPaddle->playerInput(-0.1);
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                mPlayerPaddle->playerInput(0.1);
            }
        } else {
            // Handle the case where mPlayerPaddle is null
            std::cerr << "Warning: mPlayerPaddle is nullptr!" << std::endl;
        }
    }
    return 0;
}

void PongGame::physics()
{
    for(auto &ball: mBallsArray)
    {
        ball->physics(mWidth, mHeight, mPaddleArray, mPaddleArray.size());
    }
}


bool PongGame::getGameRunning() const
{
    return mGameRunning;
}

std::vector<char> PongGame::getSerializedData() const
{
    std::vector<char> buffer;

    // Protocol version
    unsigned int protocolVersion = 1;
    buffer.insert(buffer.end(), reinterpret_cast<const char*>(&protocolVersion),
                  reinterpret_cast<const char*>(&protocolVersion) + sizeof(protocolVersion));

    // Number of objects
    unsigned int objNumber = mIDMap.size();
    buffer.insert(buffer.end(), reinterpret_cast<const char*>(&objNumber),
                  reinterpret_cast<const char*>(&objNumber) + sizeof(objNumber));

    // Serialize all objects
    for (const auto& elt : mIDMap) {
        int type = elt.second->getType();
        int id = elt.first;

        //Type of the obj
        buffer.insert(buffer.end(), reinterpret_cast<const char*>(&type),
                      reinterpret_cast<const char*>(&type) + sizeof(type));

        //id of the obj
        buffer.insert(buffer.end(), reinterpret_cast<const char*>(&id),
                      reinterpret_cast<const char*>(&id) + sizeof(id));

        // Serialize object data
        elt.second->serialize(buffer);  // Serialize based on the object type
    }

    return buffer;
}

/*TODO: Could be interesting to create a shorter version of this serialisaition/Deseralization protocol in order to only send/receive pertinent data
        such as speed and position. Less data, faster transfer
*/
int PongGame::loadSerializedData(const char* buffer)
{
    //TODO: test !!!
    unsigned int protocolVersion;
    memcpy(&protocolVersion, buffer, sizeof(protocolVersion));

    if (protocolVersion == 1) {
        size_t offset = sizeof(protocolVersion);

        // Number of objects
        unsigned int objNumber;
        memcpy(&objNumber, buffer + offset, sizeof(objNumber));
        offset += sizeof(objNumber);

        // Deserialize each object
        for (unsigned int i = 0; i < objNumber; ++i) {
            int type, id;
            memcpy(&type, buffer + offset, sizeof(type));
            offset += sizeof(type);
            memcpy(&id, buffer + offset, sizeof(id));
            offset += sizeof(id);

            // Check if object exists in mIDMap
            if (mIDMap.find(id) != mIDMap.end()) {
                auto& obj = mIDMap.at(id);

                // Ensure the type matches
                if (obj->getType() != type) {
                    std::cerr << "Type mismatch for ID " << id << std::endl;
                    return 3;
                }

                // Deserialize object data
                obj->deserialize(buffer, offset);
            } else {
                std::cerr << "Object with ID " << id << " not found" << std::endl;
                return 2;
            }
        }
        return 0;
    } else {
        std::cerr << "Unhandled protocol version: " << protocolVersion << std::endl;
        return 1;
    }
}







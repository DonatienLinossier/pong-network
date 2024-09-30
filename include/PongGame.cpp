//
// Created by donat on 29/09/2024.
//

#include "PongGame.h"

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
    mWindow = std::make_unique<sf::RenderWindow>(sf::VideoMode(width, height), "SFML Window Client");


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
    //TODO: test!!!
    std::vector<char> buffer;
    //Protocol version
    unsigned int protocolVersion = 1;
    buffer.insert(buffer.end(), reinterpret_cast<const char*>(&protocolVersion),
                                            reinterpret_cast<const char*>(&protocolVersion) + sizeof(protocolVersion));

    //Number of objects
    unsigned int objNumber = mIDMap.size();
    buffer.insert(buffer.end(), reinterpret_cast<const char*>(&objNumber),
                                        reinterpret_cast<const char*>(&objNumber) + sizeof(objNumber));

    for(auto elt: mIDMap)
    {
        int type = elt.second->getType();
        buffer.insert(buffer.end(), reinterpret_cast<const char*>(&type),
                                        reinterpret_cast<const char*>(&type) + sizeof(type));
        int id = elt.first;
        buffer.insert(buffer.end(), reinterpret_cast<const char*>(&id),
                                        reinterpret_cast<const char*>(&id) + sizeof(id));
    }
    for(auto elt: mIDMap)
    {
        buffer.insert(buffer.end(), reinterpret_cast<const char*>(&(*(elt.second))),
                                        reinterpret_cast<const char*>(&(*(elt.second))) + sizeof(*(elt.second)));
    }
    /*std::ofstream outFile("output/testSerialisation", std::ios::binary); // Use binary mode
    outFile.write(buffer.data(), buffer.size());

    // Close the file
    outFile.close();*/
    return buffer;
}

/*TODO: Could be interesting to create a shorter version of this serialisaition/Deseralization protocol in order to only send/receive pertinent data
        such as speed and position. Less data, faster transfer
*/
int PongGame::loadSerializedData(const char* buffer)
{
    //TODO: test !!!
    unsigned int protocolVersion;
    memcpy(&protocolVersion, buffer, sizeof(unsigned int));

    switch(protocolVersion)
    {
    case 1:
        {
            unsigned int objNumber;
            memcpy(&objNumber, buffer+sizeof(unsigned int), sizeof(objNumber));

            int headerSize = sizeof(unsigned int)*2 +(sizeof(int)*2)*objNumber;
            int compteur = 0;
            for(int i =0; i<objNumber; ++i)
            {
                int type;
                memcpy(&type, buffer+ sizeof(unsigned int)*2 +(sizeof(int)*2)*i, sizeof(type)); //memcpy(&test, buffer + sizeof(header) + sizeOfPreviousHeaderdata, sizeof(int));

                int ID;
                memcpy(&type, buffer+ sizeof(unsigned int)*2 +(sizeof(int)*2)*i + sizeof(type), sizeof(ID)); //memcpy(&test, buffer + sizeof(header) + sizeOfPreviousHeaderdata + sizeOf(type), sizeof(int));

                if(mIDMap.find(ID)==mIDMap.end()) //The ID is not is the map
                {
                    /*TODO: We ll need to create it when we will move the creation of the game on the serv side(The first connection will send all the objs, and we will create them.
                            Could also be done in another fonc, which seems better. [Do not forget to add the pointers of the elt to the differents tabs]
                            In all case, for now we will just raise an error as all elements we receive should be already initiliazed*/
                    std::cerr << "Element with ID " << ID << "cannot be find in nIDMap. The type of the elt is " << type << ".(See const.h to have the correspond type)" << std::endl;
                    return 2;
                }

                if(mIDMap.at(ID)->getType()!=type)
                {
                    std::cerr << "Mismatched type for ID " << ID << " ! Trying to fill a " << mIDMap.at(ID)->getType() << " with a " << type << ". (See const.h to have the correspond types)" << std::endl;
                    return 3;
                }


                memcpy(&(*(mIDMap.at(ID))), buffer + headerSize + compteur, sizeof(*mIDMap.at(ID)));
                // So here, we copy to the adresse of the obj stored by the shared ptr with the id ID of the nIDmap. The size of the obcj is sizeof(*mIDMap(ID))
                compteur+=sizeof(*mIDMap.at(ID));
            }
            return 0;
        }

    default:
        {
            std::cerr << "Unhandled protocol version" << std::endl;
            return 1;
        }
    }
}







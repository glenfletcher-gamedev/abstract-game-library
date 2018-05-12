#ifndef GAME_H
#define GAME_H
#include <SFML/Graphics.hpp>
#include <string>
#include "gameobjectmanager.h"

int main(int argc, char *argv[]);

class Game
{
    public:
        static Game &getGame() { return *MyGame; }
        virtual ~Game();
    protected:
        Game(GameObjectManager *objMgr = nullptr);
        void setSize(int width, int height) { m_size = sf::Vector2i(width, height); }
        void setSize(sf::Vector2i size) { m_size = size; }
        void setTitle(std::string title) { m_title = title; }
        void setFPS(float FPS) { m_FPS = FPS; }
        void setRefresh(float refresh) { m_refresh = refresh;}
        void close() { if (m_window) m_window->close(); }
        void createWindow() {
            if (m_window) delete m_window;
            m_window = new sf::RenderWindow(sf::VideoMode(m_size.x, m_size.y), m_title);
            }
        GameObjectManager &objectManager() { return *m_objects; }
    private:
        virtual bool onInit()=0;
        virtual void onExit() {};
        virtual void onEvent(sf::Event &e)=0;
        virtual void onUpdate(float dt)=0;
        virtual void onDraw(sf::RenderTarget &canvas, float alpha)=0;
        void run();

        sf::RenderWindow *m_window = nullptr;
        sf::Vector2i m_size;
        std::string m_title;
        sf::Clock m_clock;
        float m_FPS;
        float m_refresh;

        GameObjectManager *m_objects;

        static Game *MyGame;

    friend int main(int argc, char *argv[]);
};


#define AGL_IMPLIEMENT_GAME(MY_GAME, ...) \
int main(int argc, char *argv[]) \
{ \
    Game::MyGame = new MY_GAME{__VA_ARGS__}; \
    if (!Game::MyGame->onInit()) { \
        delete Game::MyGame; \
        return EXIT_FAILURE; \
    } \
    Game::MyGame->createWindow(); \
    Game::MyGame->run(); \
    Game::MyGame->onExit(); \
    delete Game::MyGame; \
    return EXIT_SUCCESS; \
} \

#endif // GAME_H

#include "game.h"

Game::Game(GameObjectManager *objMgr)
{
    if (objMgr) {
        m_objects = objMgr;
    } else {
        m_objects = new GameObjectManager();
    }
}

Game::~Game()
{
    if (m_window) delete m_window;
    if (m_objects) delete m_objects;
}

void Game::run() {
    float remaining = 0.f;
    float dt = 1.f/m_FPS;
    #ifdef DEBUG
    m_window->setFramerateLimit(m_FPS);
    #endif // DEBUG
    while (m_window and m_window->isOpen())
    {
        #ifndef DEBUG
        sf::Time eps = m_clock.restart();
        remaining += eps.asSeconds();
        #endif // DEBUG

        sf::Event e;
        while (m_window->pollEvent(e)) {
            if (e.type == sf::Event::Closed) close();
            onEvent(e);
        }

        #ifndef DEBUG
        if (remaining*m_refresh > 1.f) return;
        #endif // DEBUG

        #ifdef DEBUG
        onUpdate(dt);
        #else
        while (remaining >= dt) {
            onUpdate(dt);
            remaining -= dt;
        }
        #endif // DEBUG

        m_window->clear();
        onDraw(*m_window, remaining*m_FPS);
        m_window->display();
    }
}

Game *Game::MyGame = nullptr;


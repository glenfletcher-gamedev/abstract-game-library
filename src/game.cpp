#include "game.h"

Game::Game()
{
    //ctor
}

Game::~Game()
{
    if (m_window) delete m_window;
}

void Game::run() {
    float remaining = 0.f;
    float dt = 1.f/m_FPS;
    while (m_window and m_window->isOpen())
    {
        sf::Time eps = m_clock.restart();
        remaining += eps.asSeconds();

        sf::Event e;
        while (m_window->pollEvent(e)) {
            if (e.type == sf::Event::Closed) close();
            onEvent(e);
        }

        if (remaining*m_refresh > 1.f) return;

        while (remaining >= dt) {
            onUpdate(dt);
            remaining -= dt;
        }

        m_window->clear();
        onDraw(*m_window, remaining*m_FPS);
        m_window->display();
    }
}

Game *Game::MyGame = nullptr;

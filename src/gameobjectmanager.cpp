#include "gameobjectmanager.h"

GameObjectManager::GameObjectManager()
{
    //ctor
}

GameObjectManager::~GameObjectManager()
{
    //dtor
}

void GameObjectManager::update(float dt, int state) {
    if (m_objects.size() == 0) return;
    for (auto it = m_objects.begin(); it!=m_objects.end(); it++) {
        it->second->update(dt, state);
    }
    float nt = 2;
    GameObject *plhs;
    GameObject *prhs;
    while (true) {
        auto lhs = m_objects.begin();
        auto rhs = lhs;
        while (true) {
            if (++rhs==m_objects.end()) {
                if (++lhs==m_objects.end()) { break; }
                rhs = lhs; continue;
            }
            {
                float ct;
                if (GameObject::checkCollide(*(lhs->second), *(rhs->second), state, ct)) {
                    if (ct < nt) {
                        nt = ct;
                        plhs = lhs->second;
                        prhs = rhs->second;
                    }
                }
            }
        }
        if (nt <= 1 and nt >= 0) {
            GameObject::doCollide(*plhs, *prhs, state, nt);
            nt = 2;
        } else {
            break;
        }
    }
}


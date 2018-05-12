#ifndef GAMEOBJECTMANAGER_H
#define GAMEOBJECTMANAGER_H

#include "gameobject.h"
#include <unordered_map>

typedef size_t EntityID;

class GameObjectManager
{
    public:

        /** Default constructor */
        GameObjectManager();
        /** Default destructor */
        virtual ~GameObjectManager();

        void reserve(size_t count) { m_objects.reserve(count); }
        template<typename Type, typename... Args> EntityID create(Args... args) { return this->add(new Type (args...)); }
        EntityID add(GameObject * obj) { m_objects.emplace(m_next_id, obj); return m_next_id++; }

        GameObject &get(EntityID id) {
            auto it = m_objects.find(id);
            if (it != m_objects.end()) {
                return *(it->second);
            } else { throw std::runtime_error("Tried to Fetch Dead Object"); }
        }

        bool destroy(EntityID id) {
            auto it = m_objects.find(id);
            if (it != m_objects.end()) {
                delete it->second;
                m_objects.erase(it);
                return true;
            }
            return false;
        }

        void destroyAll() {
            for (auto it = m_objects.begin(); it!=m_objects.end(); it++) {
                delete it->second;
            }
            m_objects.clear();
        }

        void update(float dt, int state);
        void draw(sf::RenderTarget &canvas, float alpha, int state) {
            for (auto it = m_objects.begin(); it!=m_objects.end(); it++) {
                it->second->draw(canvas, alpha, state);
            }
        }

        template<class Functor>
        void apply(Functor func) {
            for (auto it = m_objects.begin(); it!=m_objects.end(); it++) {
                func(*(it->second));
            }
        }
    protected:

    private:
        EntityID m_next_id;
        std::unordered_map<EntityID, GameObject *> m_objects;
};

#endif // GAMEOBJECTMANAGER_H

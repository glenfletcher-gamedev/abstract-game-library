#include "gameobject.h"

#include <stdexcept>
#include <iostream>


GameObject::GameObject()
{
    //ctor
}

GameObject::~GameObject()
{
    //dtor
}

void GameObject::update(float dt, int state) {
    this->onUpdate(dt, state);
}

void GameObject::draw(sf::RenderTarget& canvas, float alpha, int state) {
    this->onDraw(canvas, alpha, state);
}

bool GameObject::register_type_name(std::type_index id, std::string name) {
    #ifdef DEBUG
    std::cout << "Registeing C++ type '" << id.name() << "' as GameObject Type '" << name << "'" << std::endl;
    #endif // DEBUG
    for (auto it : typeMapping) {
        auto itp = collideHandleMapping.find(CollideMapping(name, it.first));
        if (itp != collideHandleMapping.end()) {
            collideHandles.emplace(CollideIndex(id, it.second), itp->second);
        }
        itp = collideHandleMapping.find(CollideMapping(it.first, name));
        if (itp != collideHandleMapping.end()) {
            collideHandles.emplace(CollideIndex(it.second, id), itp->second);
        }
    }
    typeMapping.emplace(name, id);
    typeName.emplace(id, name);
    return true;
}

bool GameObject::register_collision(std::string LHS, std::string RHS, CheckCollide<GameObject, GameObject> checkCollide, DoCollide<GameObject, GameObject> doCollide) {
    auto LHS_it = typeMapping.find(LHS);
    auto RHS_it = typeMapping.find(RHS);
    if (LHS_it != typeMapping.end() and RHS_it != typeMapping.end()) {
        return register_collision(LHS_it->second, RHS_it->second, checkCollide, doCollide);
    } else {
        collideHandleMapping.emplace(CollideMapping(LHS, RHS), CollideHandle(checkCollide, doCollide, false));
        if (LHS != RHS) {
            collideHandleMapping.emplace(CollideMapping(RHS, LHS), CollideHandle(checkCollide, doCollide, true));
        }
        return true;
    }
}

bool GameObject::register_collision(std::type_index LHS, std::type_index RHS, CheckCollide<GameObject, GameObject> checkCollide, DoCollide<GameObject, GameObject> doCollide) {
    collideHandles.emplace(CollideIndex(LHS, RHS), CollideHandle(checkCollide, doCollide, false));
    if (LHS != RHS) {
        collideHandles.emplace(CollideIndex(RHS, LHS), CollideHandle(checkCollide, doCollide, true));
    }
    return true;
}

bool GameObject::checkCollide(std::type_index LHS, std::type_index RHS, const GameObject& lhs, const GameObject& rhs, int state, float& time) {
    auto it = collideHandles.find(CollideIndex(LHS, RHS));
    if (it != collideHandles.end()) {
        auto handle = it->second;
        if (handle.reversed) {
            return handle.checkCollide(rhs, lhs, state, time);
        } else {
            return handle.checkCollide(lhs, rhs, state, time);
        }
    } else {
        return false;
    }
}

void GameObject::doCollide(std::type_index LHS, std::type_index RHS, GameObject& lhs, GameObject& rhs, int state, float time) {
    auto it = collideHandles.find(CollideIndex(LHS, RHS));
    if (it != collideHandles.end()) {
        auto handle = it->second;
        if (handle.reversed) {
            return handle.doCollide(rhs, lhs, state, time);
        } else {
            return handle.doCollide(lhs, rhs, state, time);
        }
    } else {
        throw InvalidCollisionError(LHS, RHS);
    }
}

GameObject::InvalidCollisionError::InvalidCollisionError(std::type_index LHS, std::type_index RHS) {
    std::string LHSname;
    std::string RHSname;
    int len;
    auto it = GameObject::typeName.find(LHS);
    if (it != GameObject::typeName.end()) {
        LHSname = it->second;
    } else {
        const char *fmt = "Unknown<%s>";
        len = snprintf(NULL, 0, fmt, LHS.name());
        char * name = new char[len+1];
        snprintf(name, len + 1, fmt, LHS.name());
        LHSname = std::string(name);
        delete name;
    }
    it = GameObject::typeName.find(RHS);
    if (it != GameObject::typeName.end()) {
        RHSname = it->second;
    } else {
        const char *fmt = "Unknown<%s>";
        len = snprintf(NULL, 0, fmt, RHS.name());
        char * name = new char[len+1];
        snprintf(name, len + 1, fmt, RHS.name());
        RHSname = std::string(name);
        delete name;
    }
    const char *fmt = "ERROR: Invalid Collision between GameObjects: %s and %s";
    len = snprintf(NULL, 0, fmt, LHSname.c_str(), RHSname.c_str());
    msg = new char[len+1];
    snprintf(msg, len + 1, fmt, LHSname.c_str(), RHSname.c_str());
}

GameObject::InvalidCollisionError::~InvalidCollisionError() {
    delete msg;
}

const char * GameObject::InvalidCollisionError::what() const throw() {
    return msg;
}

std::unordered_map<GameObject::CollideIndex, GameObject::CollideHandle, GameObject::CollideIndexHasher> GameObject::collideHandles{};
std::unordered_map<GameObject::CollideMapping, GameObject::CollideHandle, GameObject::CollideMappingHasher> GameObject::collideHandleMapping{};
std::unordered_map<std::string, std::type_index> GameObject::typeMapping{};
std::unordered_map<std::type_index, std::string> GameObject::typeName{};

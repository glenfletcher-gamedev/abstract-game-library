#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <typeinfo>
#include <typeindex>

class GameObject
{
    friend class InvalidCollisionError;
    class InvalidCollisionError : std::exception {
        public:
            InvalidCollisionError(std::type_index LHS, std::type_index RHS);
            ~InvalidCollisionError();
            const char * what() const throw();
        private:
            char * msg;
    };
    private:
        template<typename LHS, typename RHS>
        using CheckCollide = bool (*)(const LHS&, const RHS&, int, float &);
        template<typename LHS, typename RHS>
        using DoCollide = void (*)(LHS&, RHS&, int, float);
        using CollideIndex =  std::tuple<std::type_index, std::type_index>;
        using CollideMapping = std::tuple<std::string, std::string>;

        struct CollideIndexHasher {
            using argument_type = CollideIndex;
            using result_type = std::size_t;
            result_type operator()(const argument_type& a) const {
                return std::hash<std::type_index>{}(std::get<0>(a)) + std::hash<std::type_index>{}(std::get<1>(a));
            }
        };

        struct CollideMappingHasher {
            using argument_type = CollideMapping;
            using result_type = std::size_t;
            result_type operator()(const argument_type& a) const {
                return std::hash<std::string>{}(std::get<0>(a)) + std::hash<std::string>{}(std::get<1>(a));
            }
        };

        struct CollideHandle {
            CheckCollide<GameObject, GameObject> checkCollide;
            DoCollide<GameObject, GameObject> doCollide;
            bool reversed;
            CollideHandle(CheckCollide<GameObject, GameObject> _checkCollide, DoCollide<GameObject, GameObject> _doCollide, bool _reversed) :
                checkCollide(_checkCollide), doCollide(_doCollide), reversed(_reversed) {}
        };

        static std::unordered_map<std::type_index, std::string> typeName;
        static std::unordered_map<std::string, std::type_index> typeMapping;
        static std::unordered_map<CollideMapping, CollideHandle, CollideMappingHasher> collideHandleMapping;
        static std::unordered_map<CollideIndex, CollideHandle, CollideIndexHasher> collideHandles;
    public:
        GameObject();
        virtual ~GameObject();

        void update(float dt, int state);
        void draw(sf::RenderTarget &canvas, float alpha, int state);

        // Handle Collisions for any Object
        static bool checkCollide(const GameObject &lhs, const GameObject &rhs, int state, float &time)
            { return checkCollide(typeid(lhs), typeid(rhs), lhs, rhs, state, time); }
        static void doCollide(GameObject &lhs, GameObject &rhs, int state, float time)
            { doCollide(typeid(lhs), typeid(rhs), lhs, rhs, state, time); }
        template <typename T>
        static bool register_type()
            { return T::register_type(); }
    protected:
        template <typename T>
        static bool register_type_name(std::string name)
            { return register_type_name(typeid(T), name); }
        template <typename LHS, typename RHS>
        static bool register_collision(CheckCollide<LHS, RHS> checkCollide, DoCollide<LHS, RHS> doCollide)
            {
                return register_collision(typeid(LHS), typeid(RHS),
                                          [checkCollide](const GameObject &lhs, const GameObject &rhs, int state, float &time) -> bool
                                          { return checkCollide(static_cast<const LHS &>(lhs), static_cast<const RHS &>(rhs), state, time); },
                                          [doCollide](GameObject &lhs, GameObject &rhs, int state, float time) -> void
                                          { doCollide(static_cast<LHS &>(lhs), static_cast<RHS &>(rhs), state, time); }
                                         );
            }
        static bool register_collision(std::string LHS, std::string RHS, CheckCollide<GameObject, GameObject> checkCollide, DoCollide<GameObject, GameObject> doCollide);
    private:
        static bool register_type_name(std::type_index, std::string);
        static bool register_collision(std::type_index, std::type_index, CheckCollide<GameObject, GameObject>, DoCollide<GameObject, GameObject>);
        static bool checkCollide(std::type_index, std::type_index, const GameObject &, const GameObject &, int, float &);
        static void doCollide(std::type_index, std::type_index, GameObject &, GameObject &, int, float);

        virtual void onUpdate(float dt, int state) = 0;
        virtual void onDraw(sf::RenderTarget &canvas, float alpha, int state) = 0;
};


#endif // GAMEOBJECT_H

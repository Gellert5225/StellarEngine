#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/Log.h"
#include "Stellar/Scene/Scene.h"

#include <entt.h>

namespace Stellar {
    class STLR_API Entity {
    public:
        Entity() = default;
        Entity(entt::entity entity, Scene* scene);
        Entity(const Entity& other) = default;

        template<typename T, typename... Args>
        T& addComponent(Args&&... args) {
            STLR_CORE_ASSERT(!hasComponent<T>(), "Entity already has component");
            return m_Scene->m_Registry.emplace<T>(m_Entity, std::forward<Args>(args)...);
        }

        template<typename T>
        T& getComponent() {
            STLR_CORE_ASSERT(hasComponent<T>(), "Entity does not have component");
            return m_Scene->m_Registry.get<T>(m_Entity);
        }

        template<typename T>
        bool hasComponent() {
            return m_Scene->m_Registry.all_of<T>(m_Entity);
        }

        template<typename T>
        void removeComponent() {
            STLR_CORE_ASSERT(hasComponent<T>(), "Entity does not have component");
            m_Scene->m_Registry.remove<T>(m_Entity);
        }
    private:
        entt::entity m_Entity;
        Scene* m_Scene;
    };
}
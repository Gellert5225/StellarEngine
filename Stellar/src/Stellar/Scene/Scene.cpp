#include "stlrpch.h"
#include "Scene.h"

#include <glm/glm.hpp>

namespace Stellar {
    Scene::Scene() {
        struct TransformComponent {
            glm::mat4 transform;

            TransformComponent() = default;
            TransformComponent(const TransformComponent&) = default;
            TransformComponent(const glm::mat4& transform) : transform(transform) {}
            operator const glm::mat4&() const { return transform; }
            operator glm::mat4&() { return transform; }
        };

        entt::entity entity = m_Registry.create();
        m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));
    }

    Scene::~Scene() {

    }

    void Scene::onUpdate(Timestep ts) {
        
    }
}
#include "stlrpch.h"
#include "Scene.h"
#include "Components.h"

#include "Stellar/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

namespace Stellar {
    Scene::Scene() {

    }

    Scene::~Scene() {

    }

    entt::entity Scene::createEntity() {
        return m_Registry.create();
    }

    void Scene::onUpdate(Timestep ts) {
        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {
            auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            Renderer2D::DrawQuad(transform.transform, sprite.color, sprite.texture);
        }
    }
}
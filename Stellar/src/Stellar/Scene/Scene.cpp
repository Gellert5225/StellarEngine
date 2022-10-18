#include "stlrpch.h"
#include "Scene.h"
#include "Components.h"
#include "Entity.h"

#include "Stellar/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

namespace Stellar {
    Scene::Scene() {

    }

    Scene::~Scene() {

    }

    Entity Scene::createEntity(const std::string& name) {
        Entity entity = { m_Registry.create(), this };
        
        entity.addComponent<TransformComponent>();
        auto& tag = entity.addComponent<TagComponent>();
        tag.tag = name == "" ? "Entity" : name ;

        return entity;
    }

    void Scene::onUpdate(Timestep ts) {
        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {
            auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            Renderer2D::DrawQuad(transform.transform, sprite.color, sprite.texture);
        }
    }
}
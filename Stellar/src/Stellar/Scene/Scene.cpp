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
        Camera* mainCamera = nullptr;
        auto view = m_Registry.view<CameraComponent, TransformComponent>();
        for (auto entity : view) {
            auto [camera, transform] = view.get<CameraComponent, TransformComponent>(entity);
            if (camera.primary) {
                mainCamera = &camera.camera;
                break;
            }
        }

        if (mainCamera) {
            Renderer2D::BeginScene(*mainCamera);
            auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
            for (auto entity : group) {
                auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
                Renderer2D::DrawQuad(transform.transform, sprite.color, sprite.texture);
            }
            Renderer2D::EndScene();
        }
    }

    void Scene::onViewportResize(uint32_t width, uint32_t height) {
        m_ViewportWidth = width;
        m_ViewportHeight = height;

        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view) {
            auto camera = view.get<CameraComponent>(entity);
            if (!camera.fixedAspectRatio) {
                
            }
        }
    }
}
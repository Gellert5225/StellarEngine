#include "stlrpch.h"
#include "Scene.h"
#include "Components.h"
#include "Entity.h"

#include "Stellar/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

namespace Stellar {
	Scene::Scene() { }

	Scene::~Scene() { }

	Entity Scene::createEntity(const std::string& name) {
		return createEntity(UUID(), name);
	}

	Entity Scene::createEntity(UUID id, const std::string& name) {
		Entity entity = { m_Registry.create(), this };
		
		entity.addComponent<IDComponent>(id);
		entity.addComponent<TransformComponent>();
		auto& tag = entity.addComponent<TagComponent>();
		tag.tag = name == "" ? "Entity" : name ;

		return entity;
	}

	void Scene::destroyEntity(Entity entity) {
		m_Registry.destroy(entity);
	}

	void Scene::onUpdate(STLR_Ptr<Renderer2D>& renderer2D, Timestep ts) {
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		auto view = m_Registry.view<CameraComponent, TransformComponent>();
		for (auto entity : view) {
			auto [camera, transform] = view.get<CameraComponent, TransformComponent>(entity);
			if (camera.primary) {
				mainCamera = &camera.camera;
				cameraTransform = transform.getTransform();
				break;
			}
		}

		if (mainCamera) {
			renderer2D->resetStats();
			renderer2D->beginScene(*mainCamera, cameraTransform);
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group) {
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				renderer2D->drawQuad(transform.getTransform(), sprite.color, sprite.texture, 1.0f);
			}
			renderer2D->endScene();
		}
	}

	void Scene::onEditorUpdate(STLR_Ptr<Renderer2D>& renderer2D, Timestep ts, EditorCamera& camera) {
		renderScene(renderer2D, camera);
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height) {
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view) {
			auto& camera = view.get<CameraComponent>(entity);
			if (!camera.fixedAspectRatio) {
				camera.camera.setViewPortSize(width, height);
			}
		}
	}

	void Scene::renderScene(STLR_Ptr<Renderer2D>& renderer2D, EditorCamera& camera) {
		renderer2D->resetStats();
		renderer2D->beginScene(camera);
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group) {
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			renderer2D->drawQuad(transform.getTransform(), sprite.color, sprite.texture, sprite.tilingFactor);
		}
		renderer2D->endScene();
	}
}
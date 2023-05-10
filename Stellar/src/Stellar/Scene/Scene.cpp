#include "stlrpch.h"
#include "Scene.h"
#include "Components.h"
#include "Entity.h"

#include "Stellar/Renderer/Renderer2D.h"

#include <glm/glm.hpp>

namespace Stellar {
	Scene::Scene() {
		m_Renderer2D = STLR_Ptr<Renderer2D>::Create();
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

	void Scene::destroyEntity(Entity entity) {
		m_Registry.destroy(entity);
	}

	void Scene::onUpdate(Timestep ts) {
		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		auto view = m_Registry.view<CameraComponent, TransformComponent>();
		for (auto entity : view) {
			auto [camera, transform] = view.get<CameraComponent, TransformComponent>(entity);
			if (camera.primary) {
				mainCamera = &camera.camera;
				cameraTransform = transform.getTransform();
				STLR_CORE_INFO("Camera transform: {0}", cameraTransform[0][0]);
				break;
			}
		}

		if (mainCamera) {
			m_Renderer2D->beginScene(*mainCamera, cameraTransform);
			auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
			for (auto entity : group) {
				auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
				m_Renderer2D->drawQuad(transform.getTransform(), sprite.color, sprite.texture, 1.0f);
			}
			m_Renderer2D->endScene();
		}
	}

	void Scene::onEditorUpdate(Timestep ts, EditorCamera& camera) {
		renderScene(camera);
	}

	void Scene::onViewportResize(uint32_t width, uint32_t height) {
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view) {
			auto camera = view.get<CameraComponent>(entity);
			if (!camera.fixedAspectRatio) {
				camera.camera.setViewPortSize(width, height);
			}
		}
	}

	void Scene::renderScene(EditorCamera& camera) {
		m_Renderer2D->beginScene(camera);
		auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
		for (auto entity : group) {
			auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
			m_Renderer2D->drawQuad(transform.getTransform(), sprite.color, sprite.texture, 1.0f);
		}
		m_Renderer2D->endScene();
	}
}
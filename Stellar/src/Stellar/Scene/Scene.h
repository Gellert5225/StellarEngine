#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/UUID.h"
#include "Stellar/Core/Timestep.h"
#include "Stellar/Core/STLRBase.h"
#include "Stellar/Editor/EditorCamera.h"

#include "Stellar/Renderer/Renderer2D.h"

#include <entt/entt.hpp>

namespace Stellar {
	class Entity;

	class Scene : public STLR_Base {
	public:
		Scene();
		~Scene();

		Entity createEntity(const std::string& name = "");
		Entity createEntity(UUID id, const std::string& name = "");
		void destroyEntity(Entity entity);

		void onUpdate(STLR_Ptr<Renderer2D>& renderer2D, Timestep ts);
		void onEditorUpdate(STLR_Ptr<Renderer2D>& renderer2D, Timestep ts, EditorCamera& camera);
		void onViewportResize(uint32_t width, uint32_t height);
	private:
		void renderScene(STLR_Ptr<Renderer2D>& renderer2D, EditorCamera& camera);
	private:
		entt::registry m_Registry;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		friend class Entity;
		friend class SceneSerializer;
		friend class SceneHierarchyPanel;
	};
}
#include "stlrpch.h"
#include "Entity.h"

namespace Stellar {
	Entity::Entity(entt::entity entity, Scene* scene)
		: m_Entity(entity), m_Scene(scene) {}
}   
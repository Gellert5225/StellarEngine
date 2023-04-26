#pragma once

#pragma once

#include "Stellar/Core/Core.h"
#include "Scene.h"

namespace Stellar {
	class STLR_API SceneSerializer {
	public:
		SceneSerializer(const STLR_Ptr<Scene>& scene);

		void serialize(const std::string& path);
		void serializeRT(const std::string& path);

		bool deserialize(const std::string& path);
		void deserializeRT(const std::string& path);
	private:
		STLR_Ptr<Scene> m_Scene;
	};
}
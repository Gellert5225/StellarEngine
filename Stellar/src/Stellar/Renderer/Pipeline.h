#pragma once

#include "Stellar/Core/Core.h"

#include "Shader.h"
// #include "RenderPass.h"

namespace Stellar {
	enum class PrimitiveTopology {
		None = 0,
		Points,
		Lines,
		Triangles,
		LineStrip,
		TriangleStrip,
		TriangleFan
	};

	struct PipelineSpecification {
		Ref<Shader> Shader;
		//Ref<RenderPass> RenderPass;
		PrimitiveTopology Topology = PrimitiveTopology::Triangles;
		bool BackfaceCulling = true;
		bool DepthTest = true;
		bool DepthWrite = true;
		bool Wireframe = false;
		float LineWidth = 1.0f;

		std::string DebugName;
	};

	class STLR_API Pipeline {
	public:
		virtual ~Pipeline() = default;

		// virtual 
	};
}
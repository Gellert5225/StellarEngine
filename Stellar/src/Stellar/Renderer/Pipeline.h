#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/STLRBase.h"

#include "Shader.h"
#include "RenderPass.h"
#include "Buffer.h"

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
		STLR_Ptr<Shader> shader;
		STLR_Ptr<RenderPass> renderPass;
		PrimitiveTopology topology = PrimitiveTopology::Triangles;
		bool backfaceCulling = true;
		bool depthTest = true;
		bool depthWrite = true;
		bool wireframe = false;
		float lineWidth = 1.0f;

		std::string debugName;
	};

	class Pipeline : public STLR_Base {
	public:
		virtual ~Pipeline() = default;

		virtual PipelineSpecification& getSpecification() = 0;
		virtual const PipelineSpecification& getSpecification() const = 0;

		virtual void invalidate() = 0;
		virtual void setUniformBuffer(STLR_Ptr<Buffer> uniformBuffer, uint32_t binding, uint32_t set = 0) = 0;

		// TEMP: remove this when render command buffers are a thing
		virtual void bind() = 0;

		static STLR_Ptr<Pipeline> Create(const PipelineSpecification& spec);
	};
}
#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/STLRBase.h"
#include "Stellar/Renderer/Image.h"
#include <vector>

namespace Stellar {
	enum class FramebufferBlendMode {
		None = 0,
		OneZero,
		SrcAlphaOneMinusSrcAlpha,
		Additive,
		Zero_SrcColor
	};

	struct FramebufferTextureSpecification {
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(ImageFormat format) : format(format) {}

		ImageFormat format;
		bool blend = true;
		FramebufferBlendMode blendMode = FramebufferBlendMode::SrcAlphaOneMinusSrcAlpha;
		// TODO: filtering/wrap
	};

	struct FramebufferAttachmentSpecification {
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(const std::initializer_list<FramebufferTextureSpecification>& attachments)
			: attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> attachments;
	};

	struct FrameBufferSpec {
		uint32_t width, height;
		bool swapChainTarget = false;
		FramebufferAttachmentSpecification attachments;
	};

	class FrameBuffer : public STLR_Base {
	public:
		virtual ~FrameBuffer() = default;

		static STLR_Ptr<FrameBuffer> Create(const FrameBufferSpec& spec);

		virtual void resize(uint32_t width, uint32_t height) = 0;
		virtual const FrameBufferSpec& getSpecification() const = 0;
		virtual STLR_Ptr<Image2D> getAttachmentImage() = 0;
		virtual STLR_Ptr<Image2D> getDepthAttachmentImage() = 0;
	protected:
		FrameBufferSpec m_Spec;
		std::vector<STLR_Ptr<Image2D>> m_AttachmentImages;
		STLR_Ptr<Image2D> m_DepthAttachmentImage;
		uint32_t m_Width, m_Height;
	};
} 
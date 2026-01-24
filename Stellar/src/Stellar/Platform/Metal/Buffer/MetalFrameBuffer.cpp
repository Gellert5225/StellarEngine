#include "MetalFrameBuffer.h"
#include "Stellar/Platform/Metal/Device/MetalDevice.h"
#include "Stellar/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Stellar {
	MetalFrameBuffer::MetalFrameBuffer(const FrameBufferSpec& spec) {
		m_Width = spec.width;
		m_Height = spec.height;

		m_FrameBuffer = MTL::RenderPassDescriptor::alloc()->init();
		m_Texture = nullptr;
		m_DepthTexture = nullptr;

		invalidate();
	}

	MetalFrameBuffer::~MetalFrameBuffer() {
		if (m_Texture) m_Texture->release();
		if (m_DepthTexture) m_DepthTexture->release();
		m_FrameBuffer->release();
	}

	void MetalFrameBuffer::invalidate() {
		// Release existing textures
		if (m_Texture) {
			m_Texture->release();
			m_Texture = nullptr;
		}
		if (m_DepthTexture) {
			m_DepthTexture->release();
			m_DepthTexture = nullptr;
		}

		// Get content scale for Retina displays
		float xscale = 1.0f, yscale = 1.0f;
		glfwGetWindowContentScale(Application::Get().getWindow().getGLFWWindow(), &xscale, &yscale);
		m_Scale = xscale;
		
		uint32_t scaledWidth = static_cast<uint32_t>(m_Width * m_Scale);
		uint32_t scaledHeight = static_cast<uint32_t>(m_Height * m_Scale);

		MTL::TextureDescriptor* pTextureDesc = MTL::TextureDescriptor::alloc()->init();
		pTextureDesc->setWidth(scaledWidth);
		pTextureDesc->setHeight(scaledHeight);
		pTextureDesc->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
		pTextureDesc->setTextureType(MTL::TextureType2D);
		pTextureDesc->setStorageMode(MTL::StorageModePrivate);
		pTextureDesc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);

		MTL::TextureDescriptor* pDepthTextureDesc = MTL::TextureDescriptor::alloc()->init();
		pDepthTextureDesc->setWidth(scaledWidth);
		pDepthTextureDesc->setHeight(scaledHeight);
		pDepthTextureDesc->setPixelFormat(MTL::PixelFormatDepth32Float);
		pDepthTextureDesc->setTextureType(MTL::TextureType2D);
		pDepthTextureDesc->setStorageMode(MTL::StorageModePrivate);
		pDepthTextureDesc->setUsage(MTL::TextureUsageRenderTarget);

		m_DepthTexture = MetalDevice::GetInstance()->getDevice()->newTexture(pDepthTextureDesc);
		m_Texture = MetalDevice::GetInstance()->getDevice()->newTexture(pTextureDesc);

		auto colorAttachment = m_FrameBuffer->colorAttachments()->object(0);
		colorAttachment->setClearColor({ 0.66f, 0.9f, 0.96f, 1.0f });
		colorAttachment->setLoadAction(MTL::LoadActionClear);
		colorAttachment->setStoreAction(MTL::StoreActionStore);
		colorAttachment->setTexture(m_Texture);

		auto depthAttach = m_FrameBuffer->depthAttachment();
		depthAttach->setTexture(m_DepthTexture);
		depthAttach->setClearDepth(1.0);
		depthAttach->setStoreAction(MTL::StoreAction::StoreActionDontCare);

		pTextureDesc->release();
		pDepthTextureDesc->release();
	}

	void MetalFrameBuffer::release() {
		if (m_Texture) {
			m_Texture->release();
			m_Texture = nullptr;
		}
		if (m_DepthTexture) {
			m_DepthTexture->release();
			m_DepthTexture = nullptr;
		}
	}

	void MetalFrameBuffer::resize(uint32_t width, uint32_t height) {
		if (width == 0 || height == 0 || width > 8192 || height > 8192) {
			return;
		}

		if (m_Width == width && m_Height == height) {
			return;
		}

		m_Spec.width = width;
		m_Spec.height = height;
		m_Width = width;
		m_Height = height;

		invalidate();
	}
}

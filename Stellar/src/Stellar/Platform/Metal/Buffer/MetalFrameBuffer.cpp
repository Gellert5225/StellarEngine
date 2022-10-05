#include "MetalFrameBuffer.h"
#include "Stellar/Platform/Metal/Device/MetalDevice.h"

namespace Stellar {
    MetalFrameBuffer::MetalFrameBuffer(const FrameBufferSpec& spec) {
        m_Width = spec.width;
        m_Height = spec.height;

        m_FrameBuffer = MTL::RenderPassDescriptor::alloc()->init();

        MTL::TextureDescriptor* pTextureDesc = MTL::TextureDescriptor::alloc()->init();
        pTextureDesc->setWidth(m_Width);
        pTextureDesc->setHeight(m_Height);
        pTextureDesc->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
        pTextureDesc->setTextureType(MTL::TextureType2D);
        pTextureDesc->setStorageMode(MTL::StorageModePrivate);
        pTextureDesc->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsageShaderRead);

        m_Texture = MetalDevice::GetInstance()->getDevice()->newTexture(pTextureDesc);

        auto colorAttachment =m_FrameBuffer->colorAttachments()->object(0);
        colorAttachment->setClearColor({ 0.66f, 0.9f, 0.96f, 1.0f });
        colorAttachment->setLoadAction(MTL::LoadActionClear);
        colorAttachment->setStoreAction(MTL::StoreActionStore);
        colorAttachment->setTexture(m_Texture);

        pTextureDesc->release();
    }

    MetalFrameBuffer::~MetalFrameBuffer() {
        m_Texture->release();
        m_FrameBuffer->release();
    }

    void MetalFrameBuffer::resize(uint32_t width, uint32_t height) {
        m_Spec.width = width;
        m_Spec.height = height;
        m_Width = width;
        m_Height = height;
    }
}
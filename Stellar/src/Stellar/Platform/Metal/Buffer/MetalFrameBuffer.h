#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/FrameBuffer.h"

#include "Stellar/Platform/Metal/MetalAPI.h"

namespace Stellar {
    class STLR_API MetalFrameBuffer : public FrameBuffer {
    public:
        MetalFrameBuffer(const FrameBufferSpec& spec);
        ~MetalFrameBuffer();

        void resize(uint32_t width, uint32_t height) override;

        void invalidate();
        void release();
        STLR_Ptr<Image2D> getAttachmentImage() override { return m_AttachmentImages[0]; }
        STLR_Ptr<Image2D> getDepthAttachmentImage() override { return m_DepthAttachmentImage; }
        const FrameBufferSpec& getSpecification() const override { return m_Spec; }

        // metal
        MTL::Texture* getAttachmentTexture() { return m_Texture; }
        MTL::RenderPassDescriptor* getFrameBuffer() { return m_FrameBuffer; }
    private:
        MTL::RenderPassDescriptor* m_FrameBuffer;
        MTL::Texture* m_Texture;
    };
}
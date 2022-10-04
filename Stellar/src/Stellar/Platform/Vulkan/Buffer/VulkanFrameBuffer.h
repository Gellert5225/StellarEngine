#pragma once

#include "Stellar/Renderer/FrameBuffer.h"

#include "Stellar/Platform/Vulkan/Image/VulkanImage.h"
#include "Stellar/Platform/Vulkan/RenderPass/StandardRenderPass.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace Stellar {
    class STLR_API VulkanFrameBuffer : public FrameBuffer {
    public:
        VulkanFrameBuffer(const FrameBufferSpec& spec);
        ~VulkanFrameBuffer();

        void resize(uint32_t width, uint32_t height) override;

        void invalidate();
        void release();
        Image2D* getAttachmentImage() override { return m_AttachmentImage; }

        [[nodiscard]] VkFramebuffer getFramebuffer() const;
        [[nodiscard]] VkRenderPass getRenderPass() const;
        [[nodiscard]] size_t getFramebufferSize() const;
        const FrameBufferSpec& getSpecification() const { return m_Spec; };
    private:
        VkFramebuffer m_Framebuffer;
        StandardRenderPass* m_RenderPass;
    };
}
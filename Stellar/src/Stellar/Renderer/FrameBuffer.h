#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Image.h"
#include <vector>

namespace Stellar {
    struct FrameBufferSpec {
        uint32_t width, height;
        bool swapChainTarget = false;
        std::vector<ImageFormat> attachments;
    };

    class STLR_API FrameBuffer {
    public:
        virtual ~FrameBuffer() = default;

        static FrameBuffer* Create(const FrameBufferSpec& spec);

        virtual void resize(uint32_t width, uint32_t height) = 0;
        virtual const FrameBufferSpec& getSpecification() const = 0;
        virtual Image2D* getAttachmentImage() = 0;
    protected:
        FrameBufferSpec m_Spec;
        Image2D* m_AttachmentImage;
        uint32_t m_Width, m_Height;
    };
} 
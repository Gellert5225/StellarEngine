#pragma once

#include "Stellar/Core/Core.h"
#include <cstdint>

namespace Stellar {
    enum class ImageFormat {
		None = 0,
		RED8UN,
		RED8UI,
		RED16UI,
		RED32UI,
		RED32F,
		RG8,
		RG16F,
		RG32F,
		RGBA,
		RGBA16F,
		RGBA32F,

		B10R11G11UF,

		SRGB,

		DEPTH32FSTENCIL8UINT,
		DEPTH32F,
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8,
	};

	enum class ImageUsage {
		None = 0,
		Texture,
		Attachment,
		Storage
	};

    struct ImageSpecification {
		ImageFormat format = ImageFormat::RGBA;
		ImageUsage usage = ImageUsage::Texture;
		uint32_t width = 1;
		uint32_t height = 1;
		uint32_t mips = 1;
		uint32_t layers = 1;
	};

    class STLR_API Image {
    public:
        virtual ~Image() = default;
        virtual void invalidate() = 0;
        virtual void release() = 0;
        virtual ImageSpecification& GetSpecification() = 0;
        virtual void* getImageInfo() = 0;
    };

    class STLR_API Image2D : public Image {
    public:
        static Ref<Image2D> Create(ImageSpecification specification);
    };

	namespace Utils {
		inline bool IsDepthFormat(ImageFormat format) {
			if (format == ImageFormat::DEPTH24STENCIL8 || 
				format == ImageFormat::DEPTH32F || 
				format == ImageFormat::DEPTH32FSTENCIL8UINT)
				return true;
			return false;
		}
	}
}

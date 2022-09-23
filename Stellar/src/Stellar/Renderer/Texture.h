#pragma once

#include "Stellar/Core.h"
#include "Stellar/Renderer/Image.h"

#include <string>
#include <utility>

namespace Stellar {
    class STLR_API Texture2D {
    public:
        virtual ~Texture2D() = default;
        virtual Image2D* getImage() const = 0;
        virtual void bind() = 0;

        static Texture2D* Create(const std::string& filePath);
    protected:
        explicit Texture2D(std::string  path) : m_Path(std::move(path)) {}

        virtual bool loadImage(const std::string& filePath) = 0;
        std::string m_Path;
        Image2D* m_Image{};
    };
}
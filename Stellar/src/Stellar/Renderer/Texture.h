#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Image.h"

#include <string>
#include <utility>
#include <vector>

namespace Stellar {
    class STLR_API Texture2D {
    public:
        virtual ~Texture2D() = default;

        static Ref<Texture2D> Create(const std::string& filePath);
    protected:
        explicit Texture2D(std::string  path) : m_Path(std::move(path)) {}

        virtual bool loadImage(const std::string& filePath) = 0;
        std::string m_Path;
        Ref<Image2D> m_Image{};
    };
}
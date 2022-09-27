#pragma once

#include "Stellar/Core.h"

#include <string>
#include <unordered_map>

namespace Stellar {
    enum class ShaderType {
        None = 0,
        Vertex = 1,
        Fragment = 2
    };

    class STLR_API Shader {
    public:
        static Shader* Create(const std::string& filePath);
        static std::string ReadFile(const std::string& fileName);

    protected:
        std::string m_Name;

        const std::string extractName(const std::string& filePath) const;
    };

    class STLR_API ShaderLibrary {
    public:
        ShaderLibrary();
        ~ShaderLibrary();

        void load(const std::string& filePath);
    private:
        std::unordered_map<std::string, Shader> m_Shaders;
    };
}
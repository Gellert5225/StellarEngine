#pragma once

#include "Stellar/Core.h"

#include <string>
#include <unordered_map>

namespace Stellar {
    class STLR_API Shader {
    public:
        static Shader* Create(const std::string& filePath);
        static std::vector<char> ReadFile(const std::string& fileName);

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
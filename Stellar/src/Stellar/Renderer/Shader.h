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
        static const std::string ReadFile(const std::string& fileName);

        Shader(const std::string& filePath);
        virtual ~Shader() {};

        virtual const std::string& getName() const { return m_Name; }
    protected:
        std::string m_Name;

        const std::string extractName(const std::string& filePath) const;
    };

    class STLR_API ShaderLibrary {
    public:
        ShaderLibrary() {};
        ~ShaderLibrary() {};

        void add(Shader* shader);
        void add(const std::string& name, Shader* shader);
        void load(const std::string& filePath);
        void load(const std::string& name, const std::string& filePath);
        Shader* get(const std::string& name);
    private:
        std::unordered_map<std::string, Shader*> m_Shaders;
    };
}
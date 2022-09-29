#include "stlrpch.h"
#include "Renderer2D.h"

#include "Stellar/Renderer/Buffer.h"
#include "Stellar/Renderer/Shader.h"

namespace Stellar {
    struct Renderer2DData {
        Buffer* quadVertexBuffer;
        Buffer* quadIndexBuffer;
        Shader* quadShader;
    };

    static Renderer2DData s_Data;

    void Renderer2D::Init() {
        const std::vector<Stellar::Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f}},
            {{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f}},
            {{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f}},
            {{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f}}
        };

        const std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0
        };

        // vertex buffer
        auto vertexBufferSize = sizeof(vertices[0]) * vertices.size();
        auto* stagingBuffer = Stellar::Buffer::Create(Stellar::BufferType::Vertex,
                                                    vertexBufferSize,
                                                    vertices.data());
        s_Data.quadVertexBuffer = Stellar::Buffer::Create(Stellar::BufferType::Vertex, vertexBufferSize);
        stagingBuffer->copy(*s_Data.quadVertexBuffer);

        delete stagingBuffer;

        // index buffer
        auto indexBufferSize = sizeof(indices[0]) * indices.size();
        auto indexStagingBuffer = Stellar::Buffer::Create(Stellar::BufferType::Index,
                                                        indexBufferSize,
                                                        indices.data());
        s_Data.quadIndexBuffer = Stellar::Buffer::Create(Stellar::BufferType::Index,  indexBufferSize);
        indexStagingBuffer->copy(*s_Data.quadIndexBuffer);

        delete indexStagingBuffer;
    }

    void Renderer2D::ShutDown() {

    }
    
    void Renderer2D::BeginScene(const Camera camera) {

    }

    void Renderer2D::EndScene() {

    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec3& color) {

    }
} 
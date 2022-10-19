#include "stlrpch.h"
#include "Renderer2D.h"

#include "Stellar/Renderer/Buffer.h"
#include "Stellar/Renderer/Shader.h"
#include "Stellar/Renderer/Uniforms.h"
#include "Stellar/Renderer/Renderer.h"

namespace Stellar {
    struct Renderer2DData {
        Buffer* quadVertexBuffer;
        Buffer* quadIndexBuffer;
        Shader* quadShader;

        uint32_t indexCount;
    };

    static Renderer2DData* s_Data;

    void Renderer2D::Init() {
        s_Data = new Renderer2DData();

        const std::vector<Vertex> vertices = {
            {{-1.0f, -1.0f}, {1.0f, 0.0f}, 1.0f},
            {{ 1.0f, -1.0f}, {0.0f, 0.0f}, 1.0f},
            {{ 1.0f,  1.0f}, {0.0f, 1.0f}, 1.0f},
            {{-1.0f,  1.0f}, {1.0f, 1.0f}, 1.0f}
        };

        const std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0
        };

        s_Data->indexCount = indices.size();

        // vertex buffer
        auto vertexBufferSize = sizeof(vertices[0]) * vertices.size();
        s_Data->quadVertexBuffer = Buffer::Create(BufferType::Vertex, vertexBufferSize, vertices.data());

        // index buffer
        auto indexBufferSize = sizeof(indices[0]) * indices.size();
        s_Data->quadIndexBuffer = Buffer::Create(BufferType::Index, indexBufferSize, indices.data());
    }

    void Renderer2D::ShutDown() {
        delete s_Data->quadVertexBuffer;
        delete s_Data->quadIndexBuffer;
        delete s_Data;
    }
    
    void Renderer2D::BeginScene(const Camera camera) {
        GlobalUniforms ubo{};
        ubo.viewProjection = camera.getViewProjectionMatrix();

        Renderer::BindUbo(ubo);
        Renderer::BeginRenderPass();

        Renderer::RenderGrid(s_Data->quadVertexBuffer, s_Data->quadIndexBuffer, s_Data->indexCount);
    }

    void Renderer2D::EndScene() {
        Renderer::EndRenderPass();
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
        //Renderer::RenderGeometry(s_Data->quadVertexBuffer, s_Data->quadIndexBuffer, color, s_Data->indexCount);
    }

    void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, Texture2D* texture) {
        Renderer::RenderGeometry(s_Data->quadVertexBuffer, s_Data->quadIndexBuffer, texture, color, s_Data->indexCount, transform);
    }
} 
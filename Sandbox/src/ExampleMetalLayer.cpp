
#include "ExampleMetalLayer.h"

ExampleMetalLayer::ExampleMetalLayer() {

}

void ExampleMetalLayer::onUpdate(Stellar::Timestep ts) {
    Stellar::Renderer::BeginScene(m_Camera);
    Stellar::Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
    Stellar::Renderer::BeginRenderPass();
    //Stellar::Renderer::RenderGeometry(m_VertexBuffer, m_IndexBuffer, indices.size(), transform);
    Stellar::Renderer::EndRenderPass();
    Stellar::Renderer::EndScene();
}

void ExampleMetalLayer::onDetach() {

}

void ExampleMetalLayer::onEvent(Stellar::Event &event) {
}

void ExampleMetalLayer::onImGuiRender() {
}

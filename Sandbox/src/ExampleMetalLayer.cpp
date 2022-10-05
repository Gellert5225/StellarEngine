
#include "ExampleMetalLayer.h"
#include "imgui.h"

ExampleMetalLayer::ExampleMetalLayer() {
    auto vertexBufferSize = sizeof(vertices[0]) * 3;
    m_VertexBuffer = Stellar::Buffer::Create(Stellar::BufferType::Vertex,
                                             vertexBufferSize,
                                             vertices);

    auto colorBufferSize = sizeof(colors[0]) * 3;
    m_ColorBuffer = Stellar::Buffer::Create(Stellar::BufferType::Vertex,
                                            colorBufferSize,
                                            colors);
}

void ExampleMetalLayer::onUpdate(Stellar::Timestep ts) {
    //Stellar::Renderer::BeginScene(m_Camera);
    Stellar::Renderer::SetClearColor({ 0.66f, 0.9f, 0.96f, 1.0f });
    Stellar::Renderer::BeginRenderPass();
    Stellar::Renderer::RenderGeometry(m_VertexBuffer, m_ColorBuffer, nullptr, glm::vec3(1.0f, 0.0f,0.0f));
    Stellar::Renderer::EndRenderPass();
    //Stellar::Renderer::EndScene();
}

void ExampleMetalLayer::onDetach() {
    delete m_VertexBuffer;
    delete m_ColorBuffer;
}

void ExampleMetalLayer::onEvent(Stellar::Event &event) {
}

void ExampleMetalLayer::onImGuiRender() {
    auto appInfo = Stellar::Application::Get().getAppInfo();

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGuiWindowFlags host_window_flags = 0;
    host_window_flags |= ImGuiWindowFlags_NoTitleBar | 
                         ImGuiWindowFlags_NoCollapse | 
                         ImGuiWindowFlags_NoResize | 
                         ImGuiWindowFlags_NoMove;
    host_window_flags |= ImGuiWindowFlags_MenuBar | 
                         ImGuiWindowFlags_NoBringToFrontOnFocus | 
                         ImGuiWindowFlags_NoNavFocus;
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        host_window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(255,255,255,0));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(255,255,255,0));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(255,255,255,0));

    ImGui::Begin("DockSpace Window", nullptr, host_window_flags);
    ImGui::PopStyleVar(4);
    ImGui::PopStyleColor(3);

    ImGuiID dockspaceID = 0;
    
    dockspaceID = ImGui::GetID("HUB_DockSpace");
    ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspace_flags, nullptr);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Exit"))
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::SetNextWindowDockID(dockspaceID , ImGuiCond_FirstUseEver);
    ImGui::Begin("Info");
    ImGui::Text("GPU: %s", appInfo.graphicsInfo.c_str());
    ImGui::Text("%s", appInfo.vulkanVersion.c_str());
    ImGui::Text(
            "Frame time: %.3f ms, FPS: %.1f FPS",
            1000.0f / ImGui::GetIO().Framerate,
            ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::End();

    // view port
    ImGuiIO& io = ImGui::GetIO();
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(255,255,255,0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Editor" , nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    auto viewPortSize = ImGui::GetContentRegionAvail();
    auto perspective = viewPortSize.x / viewPortSize.y;
    //m_Camera.setOrtho(-perspective, perspective, -1, 1, -10, 10);
    m_Camera.setPerspectiveProjection(glm::radians(60.0f), perspective, 0.1f, 100.0f);
    Stellar::Renderer::ResizeFrameBuffer(viewPortSize.x, viewPortSize.y);
    Stellar::UI::ImageFromFB(Stellar::Renderer::GetFrameBuffer(), viewPortSize);

    ImGui::End();
}

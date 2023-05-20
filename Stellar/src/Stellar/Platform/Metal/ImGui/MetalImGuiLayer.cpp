#include "stlrpch.h"
#include "MetalImGuiLayer.h"

#include <imgui.h>
#include "Stellar/ImGui/imgui_impl_glfw.h"
#include "Stellar/ImGui/WebFont.h"
#include "Stellar/ImGui/ImGuiFont.h"

#include "Stellar/Platform/Metal/ImGui/imgui_impl_metal.h"
#include "Stellar/Platform/Metal/Device/MetalDevice.h"
#include "Stellar/Platform/Metal/SwapChain/MetalSwapChain.h"

#include "Stellar/Core/Application.h"

namespace Stellar {

    MetalImGuiLayer::MetalImGuiLayer() { }

    MetalImGuiLayer::~MetalImGuiLayer() { }

    void MetalImGuiLayer::onAttach() {
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

		setDarkThemeColors();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		io.Fonts->AddFontFromFileTTF("Resources/Fonts/OpenSans/static/OpenSans/OpenSans-Bold.ttf", 18.0f);
		io.Fonts->AddFontFromFileTTF("Resources/Fonts/OpenSans/static/OpenSans/OpenSans-Regular.ttf", 18.0f);
		io.Fonts->AddFontFromFileTTF("Resources/Fonts/SourceCodePro/static/SourceCodePro-Bold.ttf", 18.0f);
		io.Fonts->AddFontFromFileTTF("Resources/Fonts/SourceCodePro/static/SourceCodePro-Medium.ttf", 18.0f);
		io.Fonts->AddFontFromFileTTF("Resources/Fonts/SourceCodePro/static/SourceCodePro-ExtraBold.ttf", 18.0f);

		io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Fonts/OpenSans/static/OpenSans/OpenSans-SemiBold.ttf", 15.0f);

		static const ImWchar s_FontAwesomeRanges[] = { STLR_ICON_MIN_FA, STLR_ICON_MAX_FA, 0 };
		UI::FontConfiguration fontAwesome;
		fontAwesome.FontName = "FontAwesome";
		fontAwesome.FilePath = "Resources/Fonts/FontAwesome/fa-solid-900.ttf";
		fontAwesome.Size = 15.0f;
		fontAwesome.GlyphRanges = s_FontAwesomeRanges;
		fontAwesome.MergeWithLast = true;
		UI::Fonts::Add(fontAwesome);
        
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        auto window = Application::Get().getWindow().getGLFWWindow();
        ImGui_ImplGlfw_InitForOther(window, true);
        ImGui_ImplMetal_Init(MetalDevice::GetInstance()->getDevice());

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
    }

    void MetalImGuiLayer::begin() {
        //NS::AutoreleasePool* autoreleasePool = NS::AutoreleasePool::alloc()->init();
        
        auto swapChain = (MetalSwapChain*)Application::Get().getWindow().getSwapChain();
        auto colorAttachment = swapChain->getRenderPass()->colorAttachments()->object(0);
        colorAttachment->setClearColor({1, 1, 1, 1});
        colorAttachment->setLoadAction(MTL::LoadActionClear);
        colorAttachment->setStoreAction(MTL::StoreActionStore);
        colorAttachment->setTexture(swapChain->getCurrentFrameBuffer()->texture());

        ImGui_ImplMetal_NewFrame(swapChain->getRenderPass());
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //autoreleasePool->release();
        
    }

    void MetalImGuiLayer::end() {
        //NS::AutoreleasePool* autoreleasePool = NS::AutoreleasePool::alloc()->init();
        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

        auto time = (float)glfwGetTime();
        io.DeltaTime = m_Time > 0.0 ? (time - m_Time) : (1.0f / 60.0f);

        ImGui::Render();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            auto window = Application::Get().getWindow().getGLFWWindow();
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        auto swapChain = (MetalSwapChain*)Application::Get().getWindow().getSwapChain();
        auto commandBuffer = MetalDevice::GetInstance()->getCommandQueue()->commandBuffer();
        auto encoder = commandBuffer->renderCommandEncoder(swapChain->getRenderPass());
        
        ImGui::Render();
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, encoder);

        encoder->endEncoding();
        commandBuffer->commit();
        
        //autoreleasePool->release();
    }

    void MetalImGuiLayer::onDetach() {
        ImGui_ImplMetal_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void MetalImGuiLayer::onImGuiRender() { }

	void MetalImGuiLayer::onEvent(Event& e) {}
}

#include "stlrpch.h"
#include "ImGuiLayer.h" 

#include "Stellar/Renderer/RendererAPI.h"
#include "Stellar/Renderer/Image.h"

#include "Stellar/Core/Log.h"
#include "Stellar/ImGui/Colors.h"

#if defined __linux__ || defined _WIN64
#include "Stellar/Platform/Vulkan/ImGui/imgui_impl_vulkan.h"
#include "Stellar/Platform/Vulkan/ImGui/VulkanImGuiLayer.h"
#include "Stellar/Platform/Vulkan/Image/VulkanImage.h"
#include "Stellar/Platform/Vulkan/Texture/VulkanTexture.h"
#include "Stellar/Platform/Vulkan/Buffer/VulkanFrameBuffer.h"
#elif defined __APPLE__
#include "Stellar/Platform/Metal/ImGui/imgui_impl_metal.h"
#include "Stellar/Platform/Metal/ImGui/MetalImGuiLayer.h"
#include "Stellar/Platform/Metal/Buffer/MetalFrameBuffer.h"
#include "Stellar/Platform/Metal/Texture/MetalTexture.h"
#endif

namespace Stellar {
	ImGuiLayer *ImGuiLayer::Create() {
		switch (RendererAPI::Current()) {
			case RendererAPIType::Vulkan: 
				#if defined __linux__ || defined _WIN64
					return new VulkanImGuiLayer();
				#endif
			case RendererAPIType::Metal:  
				#if defined __APPLE__
					return new MetalImGuiLayer();
				#endif
			case RendererAPIType::None:   return nullptr;
		}
		STLR_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;

	}

	void ImGuiLayer::setDarkThemeColors() {
		auto& style = ImGui::GetStyle();
		auto& colors = ImGui::GetStyle().Colors;

		// Headers
		colors[ImGuiCol_Header]				= ImGui::ColorConvertU32ToFloat4(Colors::Theme::groupHeader);
		colors[ImGuiCol_HeaderHovered]		= ImGui::ColorConvertU32ToFloat4(Colors::Theme::groupHeader);
		colors[ImGuiCol_HeaderActive]		= ImGui::ColorConvertU32ToFloat4(Colors::Theme::groupHeader);

		// Buttons
		colors[ImGuiCol_Button]				= ImGui::ColorConvertU32ToFloat4(Colors::Theme::button);
		colors[ImGuiCol_ButtonHovered]		= ImGui::ColorConvertU32ToFloat4(Colors::Theme::buttonHovered);
		colors[ImGuiCol_ButtonActive]		= ImColor(56, 56, 56, 150);

		// Frame BG
		colors[ImGuiCol_FrameBg]			= ImGui::ColorConvertU32ToFloat4(Colors::Theme::propertyField);
		colors[ImGuiCol_FrameBgHovered]		= ImGui::ColorConvertU32ToFloat4(Colors::Theme::propertyField);
		colors[ImGuiCol_FrameBgActive]		= ImGui::ColorConvertU32ToFloat4(Colors::Theme::propertyField);

		// Tabs
		colors[ImGuiCol_Tab]				= ImGui::ColorConvertU32ToFloat4(Colors::Theme::titlebar);
		colors[ImGuiCol_TabHovered]			= ImGui::ColorConvertU32ToFloat4(Colors::Theme::tabHovered);
		colors[ImGuiCol_TabActive]			= ImGui::ColorConvertU32ToFloat4(Colors::Theme::tabActive);
		colors[ImGuiCol_TabUnfocused]		= ImGui::ColorConvertU32ToFloat4(Colors::Theme::titlebar);
		colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_TabActive];

		// Title
		colors[ImGuiCol_TitleBg]			= ImGui::ColorConvertU32ToFloat4(Colors::Theme::titlebar);
		colors[ImGuiCol_TitleBgActive]		= ImGui::ColorConvertU32ToFloat4(Colors::Theme::titlebar);
		colors[ImGuiCol_TitleBgCollapsed]	= ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

		// Resize Grip
		colors[ImGuiCol_ResizeGrip]			= ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
		colors[ImGuiCol_ResizeGripHovered]	= ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
		colors[ImGuiCol_ResizeGripActive]	= ImVec4(0.46f, 0.46f, 0.46f, 0.95f);

		// Scrollbar
		colors[ImGuiCol_ScrollbarBg]		= ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab]		= ImVec4(0.31f, 0.31f, 0.31f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);

		// Check Mark
		colors[ImGuiCol_CheckMark]			= ImColor(200, 200, 200, 255);

		// Slider
		colors[ImGuiCol_SliderGrab]			= ImVec4(0.51f, 0.51f, 0.51f, 0.7f);
		colors[ImGuiCol_SliderGrabActive]	= ImVec4(0.66f, 0.66f, 0.66f, 1.0f);

		// Text
		colors[ImGuiCol_Text]				= ImGui::ColorConvertU32ToFloat4(Colors::Theme::text);

		// Checkbox
		colors[ImGuiCol_CheckMark]			= ImGui::ColorConvertU32ToFloat4(Colors::Theme::text);

		// Separator
		colors[ImGuiCol_Separator]			= ImGui::ColorConvertU32ToFloat4(Colors::Theme::backgroundDark);
		colors[ImGuiCol_SeparatorActive]	= ImGui::ColorConvertU32ToFloat4(Colors::Theme::highlight);
		colors[ImGuiCol_SeparatorHovered]	= ImGui::ColorConvertU32ToFloat4(Colors::Theme::separaterHovered);

		// Window Background
		colors[ImGuiCol_WindowBg]			= ImGui::ColorConvertU32ToFloat4(Colors::Theme::titlebar);
		colors[ImGuiCol_ChildBg]			= ImGui::ColorConvertU32ToFloat4(Colors::Theme::background);
		colors[ImGuiCol_PopupBg]			= ImGui::ColorConvertU32ToFloat4(Colors::Theme::backgroundPopup);
		colors[ImGuiCol_Border]				= ImGui::ColorConvertU32ToFloat4(Colors::Theme::backgroundDark);

		// Tables
		colors[ImGuiCol_TableHeaderBg]		= ImGui::ColorConvertU32ToFloat4(Colors::Theme::groupHeader);
		colors[ImGuiCol_TableBorderLight]	= ImGui::ColorConvertU32ToFloat4(Colors::Theme::backgroundDark);

		// Menubar
		colors[ImGuiCol_MenuBarBg]			= ImGui::ColorConvertU32ToFloat4(Colors::Theme::menuBar);

		//========================================================
		/// Style
		style.FrameRounding = 2.5f;
		style.FrameBorderSize = 1.0f;
		style.IndentSpacing = 11.0f;
	}
}

namespace Stellar::UI {
	void* Image(STLR_Ptr<Texture2D> texture, const ImVec2& size) {
		#if defined __linux__ || defined _WIN64
		auto tex = texture.As<VulkanTexture>();
		auto image = tex->getImage();
		auto imageInfo = (VulkanImageInfo*)image->getImageInfo();
		const auto textureID = ImGui_ImplVulkan_AddTexture(imageInfo->sampler, imageInfo->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		ImGui::Image(textureID, size);
		return textureID;
		#elif defined __APPLE__
		ImGui::Image(texture.As<MetalTexture>()->getTexture(), size);
		return nullptr;
		#endif
	}

	void ImageFromFB(STLR_Ptr<FrameBuffer> frameBuffer, const ImVec2& size) {
		#if defined __linux__ || defined _WIN64
		auto imageInfo = (VulkanImageInfo*)frameBuffer->getAttachmentImage()->getImageInfo();
		const auto textureID = ImGui_ImplVulkan_AddTexture(imageInfo->sampler, imageInfo->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		ImGui::Image(textureID, size);
		#elif defined __APPLE__
		ImGui::Image(frameBuffer.As<MetalFrameBuffer>()->getAttachmentTexture(), size);
		#endif
	}

	void SetInputEnabled(bool enable) {
		auto& io = ImGui::GetIO();
		if (enable) {
			io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
			io.ConfigFlags &= ~ImGuiConfigFlags_NavNoCaptureKeyboard;
		} else {
			io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
			io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard;
		}
	}

	bool IsInputEnabled() {
		const auto& io = ImGui::GetIO();
		return (io.ConfigFlags & ImGuiConfigFlags_NoMouse) == 0 && (io.ConfigFlags & ImGuiConfigFlags_NavNoCaptureKeyboard) == 0;
	}

	bool ImageButton(STLR_Ptr<Texture2D> texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col) {
#if defined __linux__ || defined _WIN64
		auto tex = texture.As<VulkanTexture>();
		auto image = tex->getImage();
		auto imageInfo = (VulkanImageInfo*)image->getImageInfo();
		const auto textureID = ImGui_ImplVulkan_AddTexture(imageInfo->sampler, imageInfo->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		return ImGui::ImageButton(textureID, size, uv0, uv1, frame_padding, bg_col, tint_col);
#elif defined __APPLE__
		return ImGui::ImageButton(texture.As<MetalTexture>()->getTexture(), size);
#endif
	}
}
#pragma once
#include <imgui.h>

namespace Colors {
	static inline float Convert_sRGB_FromLinear(float theLinearValue);
	static inline float Convert_sRGB_ToLinear(float thesRGBValue);
	ImVec4 ConvertFromSRGB(ImVec4 colour);
	ImVec4 ConvertToSRGB(ImVec4 colour);

	// To experiment with editor theme live you can change these constexpr into static
	// members of a static "Theme" class and add a quick ImGui window to adjust the colour values
	namespace Theme
	{
		constexpr auto menuBar					= IM_COL32(35, 39, 51,  255);

		constexpr auto accent					= IM_COL32(236, 158, 36,  255);
		constexpr auto highlight				= IM_COL32(206, 212, 218, 255);
		constexpr auto niceBlue					= IM_COL32(83,  232, 254, 255);
		constexpr auto compliment				= IM_COL32(78,  151, 166, 255);

		constexpr auto background				= IM_COL32(48, 52, 64,  255);
		constexpr auto backgroundDark			= IM_COL32(35, 39, 51,  255);
		constexpr auto childBackground			= IM_COL32(48, 52, 64,  255);
		constexpr auto backgroundPopup			= IM_COL32(41, 44, 52,  255);

		constexpr auto border					= IM_COL32(71, 76, 90,  255);

		constexpr auto titlebar					= IM_COL32(35, 39, 51,  255);
		
		constexpr auto propertyField			= IM_COL32(37, 40, 50, 255);
		constexpr auto text						= IM_COL32(222, 226, 230, 255);
		constexpr auto textBrighter				= IM_COL32(210, 210, 210, 255);
		constexpr auto textDarker				= IM_COL32(128, 128, 128, 255);
		constexpr auto muted					= IM_COL32(77,  77,  77,  255);
		constexpr auto groupHeader				= IM_COL32(71, 76, 90,  255);

		constexpr auto tab						= IM_COL32(35, 39, 51, 255);
		constexpr auto tabActive				= IM_COL32(48, 52, 64, 255);
		constexpr auto tabHovered				= IM_COL32(93, 101, 126, 255);
		constexpr auto tabUnfocused				= IM_COL32(35, 39, 51,  255);
		//constexpr auto selection				= IM_COL32(191, 177, 155, 255);
		//constexpr auto selectionMuted			= IM_COL32(59, 57, 45, 255);
		constexpr auto selection				= IM_COL32(108, 117, 125, 255);
		constexpr auto selectionMuted			= IM_COL32(237, 201, 142, 23);

		constexpr auto button					= IM_COL32(63, 69, 89,  255);
		constexpr auto buttonHovered			= IM_COL32(73, 79, 99,  255);

		constexpr auto separaterHovered			= IM_COL32(35, 39, 51,  255);
		constexpr auto separaterActive			= IM_COL32(35, 39, 51,  255);
		constexpr auto separater				= IM_COL32(35, 39, 51,  255);
	}
}
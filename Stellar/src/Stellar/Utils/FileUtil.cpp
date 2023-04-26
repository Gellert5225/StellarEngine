#include "stlrpch.h"

#include "FileUtil.h"
#include "Stellar/Core/Log.h"

#include <nfd.h>

namespace Stellar {
	std::string FileDialogs::OpenFile(const char* filter) {
		NFD_Init();
		nfdchar_t *outPath;
		nfdfilteritem_t filterItem[1] = { { "Scenes", filter } };
		nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 1, NULL);
		std::string path = "";
		if (result == NFD_OKAY) {
			STLR_CORE_TRACE("Selected file: {0}", outPath);
			path = outPath;
			NFD_FreePath(outPath);
		} else if (result == NFD_CANCEL) {
			STLR_CORE_TRACE("Canceled file selection");
		} else {
			STLR_CORE_ERROR("Error when selecting file: {0}", NFD_GetError());
		}

    	NFD_Quit();
		return path;
	}

	std::string FileDialogs::SaveFile(const char* filter) {
		NFD_Init();
		nfdchar_t *outPath;
		nfdfilteritem_t filterItem[1] = { { "Scenes", filter } };
		nfdresult_t result = NFD_SaveDialog(&outPath, filterItem, 1, NULL, NULL);
		std::string path = "";
		if (result == NFD_OKAY) {
			STLR_CORE_TRACE("Saved file: {0}", outPath);
			path = outPath;
			NFD_FreePath(outPath);
		} else if (result == NFD_CANCEL) {
			STLR_CORE_TRACE("Canceled file saving");
		} else {
			STLR_CORE_ERROR("Error when Saving file: {0}", NFD_GetError());
		}

    	NFD_Quit();
		return path;
	}
}
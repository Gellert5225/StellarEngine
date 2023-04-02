#include "stlrpch.h"
#include "Layer.h"

namespace Stellar {
	Layer::Layer(const std::string& debugName)
	: m_DebugName(debugName) {}

	Layer::~Layer() {}
}
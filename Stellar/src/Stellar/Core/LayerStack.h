#pragma once

#include "Core.h"
#include "Layer.h"

#include <vector>

namespace Stellar {
    class STLR_API LayerStack {
    public:
        LayerStack();
        ~LayerStack();

        void pushLayer(std::shared_ptr<Layer> layer);
        void pushOverlay(std::shared_ptr<Layer> overlay);
        void popLayer(std::shared_ptr<Layer> layer);
        void popOverlay(std::shared_ptr<Layer> overlay);

        std::vector<std::shared_ptr<Layer>>::iterator begin() { return m_Layers.begin(); }
        std::vector<std::shared_ptr<Layer>>::iterator end() { return m_Layers.end(); }
    private:
        std::vector<std::shared_ptr<Layer>> m_Layers;
        std::vector<std::shared_ptr<Layer>>::iterator m_LayerInsert;
    };
}
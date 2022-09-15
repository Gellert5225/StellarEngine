#pragma once

#include <Stellar.h>

class ExampleMetalLayer : public Stellar::Layer {
public:
    ExampleMetalLayer();

    void onUpdate(Stellar::Timestep ts) override;

    void onDetach() override;

    void onEvent(Stellar::Event& event) override;

    void onImGuiRender() override;

private:

};

#pragma once

namespace Stellar {
    class Timestep {
    public:
        explicit Timestep(float time = 0.0f) : m_Time(time) {}

        [[nodiscard]] float getSeconds() const { return m_Time; }
        [[nodiscard]] float getMilliseconds() const { return m_Time * 1000.f; }

        operator float() const { return m_Time; }
    private:
        float m_Time;
    };
}
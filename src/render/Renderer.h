#pragma once

#include <cstdint>
#include <glm/glm.hpp>

class Renderer {
public:
    Renderer(glm::vec2 viewportSize);

    void Render();

    void OnResize(uint32_t width, uint32_t height);

    [[nodiscard]] std::uint32_t* GetFinalImageData() const { return m_ImageData; }
private:
    glm::vec4 perPixel(glm::vec2 coord);

    std::uint32_t*  m_ImageData;
    uint32_t m_Width, m_Height;
};
